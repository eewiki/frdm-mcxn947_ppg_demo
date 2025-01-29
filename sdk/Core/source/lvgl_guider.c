/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_debug_console.h"
#include "lvgl_support.h"
#include "pin_mux.h"
#include "peripherals.h"
#include "board.h"
#include "lvgl.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"
#include "lvgl_demo_utils.h"
#include "lvgl_freertos.h"

#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_smartdma.h"
#include "fsl_inputmux_connections.h"
#include "fsl_inputmux.h"

#include "max86916.h"
#include "ts_utils_simple.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_RELEASE_SDA_PORT  PORT4
#define I2C_RELEASE_SCL_PORT  PORT4
#define I2C_RELEASE_SDA_GPIO  GPIO4
#define I2C_RELEASE_SDA_PIN   0U
#define I2C_RELEASE_SCL_GPIO  GPIO4
#define I2C_RELEASE_SCL_PIN   1U
#define I2C_RELEASE_BUS_COUNT 100U

#define MAX86916_MAX_SAMPLES		32	// FIFO size
#define MAX86916_NUM_CHANNELS		4	// IR + red + green + blue
#define MAX86916_MAX_BUFF_SIZE		384	// 3 * MAX86916_NUM_CHANNELS * MAX86916_MAX_SAMPLES

#define FIR_FILTER_LEN				99
#define FIR_FILTER_ORDER			(FIR_FILTER_LEN - 1)
#define FIR_FILTER_GRPDELAY			(FIR_FILTER_ORDER >> 1)
#define FIR_TAPS_BUF_LEN			112 // ceil(FIR_FILTER_LEN / 16.0) * 16, i.e, divisible by 16
#define FIR_TAPS_PRIVATE_RAM		((void *)0xE0001000)

#define SAMPLE_BUF_LEN				598 // 500 is 10 seconds + N-1 filter taps

#define GUI_NUM_POINTS				200

#define IR_PROX_THRESH				4000

/*******************************************************************************
 * Variables
 ******************************************************************************/
static const float *firTaps = (float *)(uint32_t[FIR_TAPS_BUF_LEN]){
	0x38b90eac, 0x39526695, 0x39af4a54, 0x3a0093ce, 0x3a2fae77, 0x3a653694, 0x3a90b29d, 0x3ab23524, 0x3ad735c9, 0x3affc214,
	0x3b15f0eb, 0x3b2dcb6c, 0x3b476e4d, 0x3b62d413, 0x3b7ff3ec, 0x3b8f60d5, 0x3b9f96d8, 0x3bb09282, 0x3bc248a3, 0x3bd4ac64,
	0x3be7af51, 0x3bfb4168, 0x3c07a897, 0x3c11e5e0, 0x3c1c4e7a, 0x3c26d7b8, 0x3c317665, 0x3c3c1ed9, 0x3c46c506, 0x3c515c90,
	0x3c5bd8e2, 0x3c662d41, 0x3c704ce3, 0x3c7a2b07, 0x3c81dd86, 0x3c867842, 0x3c8adfa8, 0x3c8f0ddb, 0x3c92fd39, 0x3c96a869,
	0x3c9a0a63, 0x3c9d1e79, 0x3c9fe064, 0x3ca24c46, 0x3ca45eb7, 0x3ca614ca, 0x3ca76c11, 0x3ca862a4, 0x3ca8f723, 0x3ca928ba,
	0x3ca8f723, 0x3ca862a4, 0x3ca76c11, 0x3ca614ca, 0x3ca45eb7, 0x3ca24c46, 0x3c9fe064, 0x3c9d1e79, 0x3c9a0a63, 0x3c96a869,
	0x3c92fd39, 0x3c8f0ddb, 0x3c8adfa8, 0x3c867842, 0x3c81dd86, 0x3c7a2b07, 0x3c704ce3, 0x3c662d41, 0x3c5bd8e2, 0x3c515c90,
	0x3c46c506, 0x3c3c1ed9, 0x3c317665, 0x3c26d7b8, 0x3c1c4e7a, 0x3c11e5e0, 0x3c07a897, 0x3bfb4168, 0x3be7af51, 0x3bd4ac64,
	0x3bc248a3, 0x3bb09282, 0x3b9f96d8, 0x3b8f60d5, 0x3b7ff3ec, 0x3b62d413, 0x3b476e4d, 0x3b2dcb6c, 0x3b15f0eb, 0x3affc214,
	0x3ad735c9, 0x3ab23524, 0x3a90b29d, 0x3a653694, 0x3a2fae77, 0x3a0093ce, 0x39af4a54, 0x39526695, 0x38b90eac
};

static int16_t ppgScaled[MAX86916_NUM_CHANNELS][GUI_NUM_POINTS];

static volatile bool s_lvgl_initialized = false;
lv_ui guider_ui;
TaskHandle_t hPpgTask = NULL;
dynamic_scaler_t ppgScaler[4];
hb_detector_t hb_det[4];

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void AppTask(void *param);
static void PpgTask(void *param);
static void max86916_init();
void gui_init(void);
void gui_reset(void);

#if LV_USE_LOG
static void print_cb(const char *buf)
{
    PRINTF("\r%s\n", buf);
}
#endif

static uint32_t get_idle_time_cb(void)
{
    return (getIdleTaskTime() / 1000);
}

static void reset_idle_time_cb(void)
{
    resetIdleTaskTime();
}

void GPIO5_INT_0_IRQHANDLER(void)
{
	BaseType_t xHigherPriorityTaskWoken;
	uint8_t max86916_int_status;

	GPIO_GpioClearInterruptFlags(BOARD_MAX86916_INT_GPIO, 1U << BOARD_MAX86916_INT_PIN);
	LED_RED_TOGGLE();

	if (hPpgTask != NULL)
	{
		max86916_int_status = MAX86916_getIntStatus();

		xHigherPriorityTaskWoken = pdFALSE;
		xTaskNotifyFromISR(hPpgTask, (uint32_t)max86916_int_status, eSetBits, &xHigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************
 * Code
 ******************************************************************************/
void BOARD_I2C_ReleaseBus(void);

static void BOARD_InitSmartDMA(void)
{
    RESET_ClearPeripheralReset(kMUX_RST_SHIFT_RSTn);

    INPUTMUX_Init(INPUTMUX0);
    INPUTMUX_AttachSignal(INPUTMUX0, 0, kINPUTMUX_FlexioToSmartDma);

    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX0);

    SMARTDMA_InitWithoutFirmware();

    NVIC_EnableIRQ(SMARTDMA_IRQn);
    NVIC_SetPriority(SMARTDMA_IRQn, 3);
}


static void i2c_release_bus_delay(void)
{
    SDK_DelayAtLeastUs(100U, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}

void BOARD_I2C_ReleaseBus(void)
{
    uint8_t i = 0;
    gpio_pin_config_t pin_config;
    port_pin_config_t i2c_pin_config = {0};

    /* Config pin mux as gpio */
    i2c_pin_config.pullSelect = kPORT_PullUp;
    i2c_pin_config.mux        = kPORT_MuxAsGpio;

    pin_config.pinDirection = kGPIO_DigitalOutput;
    pin_config.outputLogic  = 1U;
    CLOCK_EnableClock(kCLOCK_Port4);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SCL_PIN, &i2c_pin_config);
    PORT_SetPinConfig(I2C_RELEASE_SCL_PORT, I2C_RELEASE_SDA_PIN, &i2c_pin_config);

    GPIO_PinInit(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, &pin_config);
    GPIO_PinInit(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, &pin_config);

    /* Drive SDA low first to simulate a start */
    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    /* Send 9 pulses on SCL and keep SDA high */
    for (i = 0; i < 9; i++)
    {
        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
        i2c_release_bus_delay();

        GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
        i2c_release_bus_delay();
        i2c_release_bus_delay();
    }

    /* Send stop */
    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 0U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SCL_GPIO, I2C_RELEASE_SCL_PIN, 1U);
    i2c_release_bus_delay();

    GPIO_PinWrite(I2C_RELEASE_SDA_GPIO, I2C_RELEASE_SDA_PIN, 1U);
    i2c_release_bus_delay();
}
/*!
 * @brief Main function
 */
int main(void)
{
    BaseType_t stat;

    /* Init board hardware. */
    /* attach FRO 12M to FLEXCOMM4 (debug console) */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u);
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* attach FRO 12M to FLEXCOMM2 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexcom2Clk, 1u);
    CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2);

    CLOCK_SetClkDiv(kCLOCK_DivFlexioClk, 1u);
    CLOCK_AttachClk(kPLL0_to_FLEXIO);

//	SYSCON->LPCAC_CTRL &= ~1;   // rocky: enable LPCAC ICache
//	SYSCON->NVM_CTRL &= SYSCON->NVM_CTRL & ~(1<<2|1<<4); // enable flash Data cache

    BOARD_InitBootClocks();
    BOARD_I2C_ReleaseBus();
    BOARD_InitBootPins();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    DEMO_InitUsTimer();
    
    /* Init smartdma. */
    BOARD_InitSmartDMA();

    LED_RED_OFF();

    stat = xTaskCreate(AppTask, "lvgl", configMINIMAL_STACK_SIZE + 800, NULL, tskIDLE_PRIORITY + 2, NULL);

    if (pdPASS != stat)
    {
        PRINTF("Failed to create lvgl task");
        while (1)
            ;
    }

    vTaskStartScheduler();

    for (;;)
    {
    } /* should never get here */
}

static void AppTask(void *param)
{
#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif

    lv_timer_register_get_idle_cb(get_idle_time_cb);
    lv_timer_register_reset_idle_cb(reset_idle_time_cb);
    lv_port_pre_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    s_lvgl_initialized = true;

    setup_ui(&guider_ui);
    events_init(&guider_ui);
    custom_init(&guider_ui);

    //xTaskCreate(ProducerTask, "prod", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(PpgTask, "ppg", 6000, NULL, tskIDLE_PRIORITY + 3, &hPpgTask);

    for (;;)
    {
        lv_task_handler();
        vTaskDelay(5);
    }
}


static void PpgTask(void *param)
{
	uint32_t max86916_status;
	uint8_t max86916_fifo_buffer[MAX86916_MAX_BUFF_SIZE];	// raw data
	uint8_t *fifoPtr;
	uint8_t max86916_num_samples;
	uint32_t ppgRawSamples[MAX86916_NUM_CHANNELS][SAMPLE_BUF_LEN];
	float ppgFilteredSamples[MAX86916_NUM_CHANNELS][SAMPLE_BUF_LEN];
	uint16_t ppgIdx;
	uint16_t shift;
	uint32_t sample;
	uint8_t i, j;
	pq_config_t pqConfig;
	uint8_t bar_gone;
	uint8_t display_cnt;
	uint16_t nullIdx;
	uint8_t bpm[4];

	char str[30];
	uint8_t len;

	/* Initialize optical sensor */
    max86916_init();

    /* Initialize GUI */
    gui_init();
    bar_gone = 0;
    display_cnt = 0;

    /* Initialize dynamic scaling and HB detection objects */
    for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
    {
    	scale_init(&ppgScaler[j], 1 / 40.0);
    	hb_detect_init(&hb_det[j], 6, 50);
    }

    /* Initialize PowerQuad */
	pqConfig.inputAFormat   = kPQ_Float;
	pqConfig.inputAPrescale = 0;
	pqConfig.inputBFormat   = kPQ_Float;
	pqConfig.inputBPrescale = 0;
	pqConfig.outputFormat   = kPQ_Float;
	pqConfig.outputPrescale = 0;
	pqConfig.tmpFormat      = kPQ_Float;
	pqConfig.tmpPrescale    = 0;
	pqConfig.machineFormat  = kPQ_Float;
	pqConfig.tmpBase        = (uint32_t *)0xE0000000;
	PQ_SetConfig(POWERQUAD, &pqConfig);

	/* Move FIR taps to PowerQuad's private RAM */
	PQ_MatrixScale(POWERQUAD, POWERQUAD_MAKE_MATRIX_LEN(16, FIR_TAPS_BUF_LEN / 16, 0), 1.0, firTaps, FIR_TAPS_PRIVATE_RAM);
	PQ_WaitDone(POWERQUAD);

	/* Configure PowerQuad for filtering 32-bit integers */
	pqConfig.inputAFormat   = kPQ_32Bit;
	PQ_SetConfig(POWERQUAD, &pqConfig);
    POWERQUAD->INBBASE = (uint32_t)(const uint32_t *)FIR_TAPS_PRIVATE_RAM;

    /* Main control loop */
    ppgIdx = 0;
    while (1)
    {
    	/* Wait for a signal from the optical sensor */
    	xTaskNotifyWait(0x00, 0xFFFFFFFF, &max86916_status, portMAX_DELAY);

    	/* If the sensor's FIFO is almost full */
    	if (max86916_status & MAX86916_INT_A_FULL)
    	{
    		/* Read raw data from FIFO */
    		max86916_num_samples = MAX86916_readFIFO(max86916_fifo_buffer);

    		/* Does data need to be shifted down? */
    		if ((ppgIdx + max86916_num_samples) > SAMPLE_BUF_LEN)
    		{
    			shift = ppgIdx - FIR_FILTER_ORDER;
    			for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
    			{
    				memcpy(ppgRawSamples[j], ppgRawSamples[j] + shift, FIR_FILTER_ORDER * sizeof(**ppgRawSamples));
    			}
				ppgIdx = FIR_FILTER_ORDER;
    		}

    		/* Format raw data into samples */
    		fifoPtr = max86916_fifo_buffer;
    		for (i = 0; i < max86916_num_samples; i++)
			{
				for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
				{
					sample = 0;
					sample |= ((uint32_t)(*fifoPtr) << 16) & 0x00070000;
					fifoPtr++;
					sample |= ((uint32_t)(*fifoPtr) << 8) & 0x0000FF00;
					fifoPtr++;
					sample |= (uint32_t)(*fifoPtr) & 0x000000FF;
					fifoPtr++;

					ppgRawSamples[j][ppgIdx] = sample;
				}

				/* If the user removed their finger from the sensor */
				if (ppgRawSamples[0][ppgIdx] < IR_PROX_THRESH)
    			{
    				MAX86916_enableInterrupt(//MAX86916_INT_A_FULL   |
											 MAX86916_INT_ALC_OVF  |
											 MAX86916_INT_PROX_INT |
											 MAX86916_INT_PWR_RDY);

    				ppgIdx = 0;
    				bar_gone = 0;
    				lv_anim_pause(guider_ui.screen_img_1, NULL);
    				break;
    			}

				ppgIdx++;
			}

    		/* LPF raw samples */
    		for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
    		{
    		    POWERQUAD->INABASE = (uint32_t)(const uint32_t *)ppgRawSamples[j];
    		    POWERQUAD->OUTBASE = (uint32_t)(uint32_t *)ppgFilteredSamples[j];
				PQ_FIRIncrement(POWERQUAD, i, FIR_FILTER_LEN, ppgIdx - i);
				PQ_WaitDone(POWERQUAD);
    		}

    		/* Calculate the difference (effective HPF) */
    		if ((ppgIdx - i) >= FIR_FILTER_GRPDELAY)
    		{
    			// remove progress bar
    			if (bar_gone == 0)
    			{
    				ui_animation(guider_ui.screen_bar_1, 200, 200, lv_obj_get_y(guider_ui.screen_bar_1), -10, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
    				ui_animation(guider_ui.screen_img_1, 300, 400, lv_obj_get_x(guider_ui.screen_img_1), 418, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
    				ui_animation(guider_ui.screen_label_1, 300, 400, lv_obj_get_x(guider_ui.screen_label_1), 30, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
    				ui_animation(guider_ui.screen_label_2, 300, 400, lv_obj_get_x(guider_ui.screen_label_2), 226, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_x, NULL, NULL, NULL);
    				lv_anim_resume(guider_ui.screen_img_1, NULL);
    				bar_gone = 1;
    			}

    			// HPF and display every other sample
    			for (; i > 0; i--)
    			{
    				/* Estimate heart rate from filtered data */
    				for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
    				{
    					ppgFilteredSamples[j][ppgIdx - i] = ppgRawSamples[j][ppgIdx - i - FIR_FILTER_GRPDELAY] - ppgFilteredSamples[j][ppgIdx - i];
    					bpm[j] = hb_detect_step(&hb_det[j], ppgFilteredSamples[j][ppgIdx - i], 25);
    				}

    				/* Display every other sample to keep the GUI responsive */
    				if ((display_cnt % 2) == 0)
    				{
        				lv_chart_set_next_value(guider_ui.screen_chart_1, guider_ui.screen_chart_1_0, (int)scale_dynamic(&ppgScaler[0], ppgFilteredSamples[0][ppgIdx - i]));
    					lv_chart_set_next_value(guider_ui.screen_chart_2, guider_ui.screen_chart_2_0, (int)scale_dynamic(&ppgScaler[1], ppgFilteredSamples[1][ppgIdx - i]));
    					lv_chart_set_next_value(guider_ui.screen_chart_3, guider_ui.screen_chart_3_0, (int)scale_dynamic(&ppgScaler[2], ppgFilteredSamples[2][ppgIdx - i]));
    					lv_chart_set_next_value(guider_ui.screen_chart_4, guider_ui.screen_chart_4_0, (int)scale_dynamic(&ppgScaler[3], ppgFilteredSamples[3][ppgIdx - i]));
    				}
    				display_cnt++;
    			}

    			// add a gap in circular buffer
    			nullIdx = guider_ui.screen_chart_1_0->start_point;
    			for (i = 0; i < 10; i++)
    			{
    				for (j = 0; j < MAX86916_NUM_CHANNELS; j++)
    				{
    					ppgScaled[j][nullIdx] = LV_CHART_POINT_NONE;
    				}

    				nullIdx++;
    				if (nullIdx >= GUI_NUM_POINTS)
    				{
    					nullIdx = 0;
    				}
    			}

    			len = sprintf(str, "%d BPM", (bpm[0] + bpm[1] + bpm[2] + bpm[3]) / 4);
    			lv_label_set_text(guider_ui.screen_label_2, str);
//    			len = sprintf(str, "%d, %d, %d, %d\r\n", bpm[0], bpm[1], bpm[2], bpm[3]);
//				LPUART_WriteBlocking((LPUART_Type *)LP_FLEXCOMM4, (uint8_t *)str, len);
    		}
    		else
    		{
    			lv_bar_set_value(guider_ui.screen_bar_1, ppgIdx, LV_ANIM_ON);
    		}

    	}

    	/* If the user placed their finger on the sensor */
    	if (max86916_status & MAX86916_INT_PROX_INT)
		{
    		gui_reset();

    		/* Enable the almost full interrupt along with the others */
    		MAX86916_enableInterrupt(MAX86916_INT_A_FULL   |
									 MAX86916_INT_ALC_OVF  |
									 MAX86916_INT_PROX_INT |
									 MAX86916_INT_PWR_RDY);
		}

    	/* If the Ambient Light Correction overflowed */
    	if (max86916_status & MAX86916_INT_ALC_OVF)
    	{
    		PRINTF("> ALC Overflow!\n\r");
    	}

    	/* If we recovered from brown-out */
    	if (max86916_status & MAX86916_INT_PWR_RDY)
		{
    		PRINTF("> PWR_RDY Flag!\n\r");
		}
    }

}

static void max86916_init()
{
	MAX86916_reset();
	//    MAX86916_shutdown();

	PRINTF("Part ID: 0x%X\n\r", MAX86916_getID());
	PRINTF("Revision ID: 0x%X\n\r", MAX86916_getRev());

	MAX86916_setFlexMode();
	MAX86916_setLEDRange(MAX86916_LED_CURRENT_100MA,
						 MAX86916_LED_CURRENT_100MA,
						 MAX86916_LED_CURRENT_100MA,
						 MAX86916_LED_CURRENT_100MA);
	MAX86916_setLEDPulseAmplitude(0x9F, 0xBF, 0xBF, 0xFF); // 0 -> 0mA, 0xFD -> LEDx_RGE
	MAX86916_setLEDSequence(MAX86916_LEDCX_LED1,
							MAX86916_LEDCX_LED2,
							MAX86916_LEDCX_LED3,
							MAX86916_LEDCX_LED4);

	MAX86916_setADCRange(MAX86916_ADC_RGE_32768);
	MAX86916_setLEDPulseWidth(MAX86916_LED_PW_70US);
	MAX86916_setSamplingRate(MAX86916_SR_800); // See Table on page 30 of DS for max SR based on config
	MAX86916_setAverageSamples(MAX86916_SMP_AVE_16);

	MAX86916_setFIFORollOver(MAX86916_ROLLOVER_ENABLE);
	MAX86916_setFIFOFreeAtInterrupt(0xF); // 0x00 -- 0x0F

	MAX86916_enableInterrupt(//MAX86916_INT_A_FULL   |
							 MAX86916_INT_ALC_OVF  |
							 MAX86916_INT_PROX_INT |
							 MAX86916_INT_PWR_RDY);
	MAX86916_setAmbientLightDet(MAX86916_ALD_ENABLE);
	MAX86916_setProximityInterruptThreshold(0x10); // 8 MSB bits of the ADC count
	MAX86916_setPilotPA(0x0F);

//    MAX86916_run();
	MAX86916_getIntStatus();
}

void gui_init(void)
{
	lv_chart_set_update_mode(guider_ui.screen_chart_1, LV_CHART_UPDATE_MODE_CIRCULAR);
	lv_chart_set_update_mode(guider_ui.screen_chart_2, LV_CHART_UPDATE_MODE_CIRCULAR);
	lv_chart_set_update_mode(guider_ui.screen_chart_3, LV_CHART_UPDATE_MODE_CIRCULAR);
	lv_chart_set_update_mode(guider_ui.screen_chart_4, LV_CHART_UPDATE_MODE_CIRCULAR);

	lv_chart_set_ext_y_array(guider_ui.screen_chart_1, guider_ui.screen_chart_1_0, ppgScaled[0]);
	lv_chart_set_ext_y_array(guider_ui.screen_chart_2, guider_ui.screen_chart_2_0, ppgScaled[1]);
	lv_chart_set_ext_y_array(guider_ui.screen_chart_3, guider_ui.screen_chart_3_0, ppgScaled[2]);
	lv_chart_set_ext_y_array(guider_ui.screen_chart_4, guider_ui.screen_chart_4_0, ppgScaled[3]);

	lv_chart_set_point_count(guider_ui.screen_chart_1, GUI_NUM_POINTS);
	lv_chart_set_point_count(guider_ui.screen_chart_2, GUI_NUM_POINTS);
	lv_chart_set_point_count(guider_ui.screen_chart_3, GUI_NUM_POINTS);
	lv_chart_set_point_count(guider_ui.screen_chart_4, GUI_NUM_POINTS);

	lv_chart_set_all_value(guider_ui.screen_chart_1, guider_ui.screen_chart_1_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_2, guider_ui.screen_chart_2_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_3, guider_ui.screen_chart_3_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_4, guider_ui.screen_chart_4_0, LV_CHART_POINT_NONE);

	lv_obj_set_x(guider_ui.screen_img_1, 481);
	ui_animation(guider_ui.screen_img_1, 100, 0, 256, 230, &lv_anim_path_linear, LV_ANIM_REPEAT_INFINITE, 1000, 100, 0, (lv_anim_exec_xcb_t)lv_img_set_zoom, NULL, NULL, NULL);
	lv_anim_pause(guider_ui.screen_img_1, NULL);

	lv_label_set_text(guider_ui.screen_label_2, "");
	lv_obj_set_x(guider_ui.screen_label_1, -357);
	lv_obj_set_x(guider_ui.screen_label_2, -152);
}

void gui_reset(void)
{
	lv_bar_set_value(guider_ui.screen_bar_1, 0, LV_ANIM_OFF);
	lv_obj_set_y(guider_ui.screen_bar_1, 0);

	lv_chart_set_all_value(guider_ui.screen_chart_1, guider_ui.screen_chart_1_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_2, guider_ui.screen_chart_2_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_3, guider_ui.screen_chart_3_0, LV_CHART_POINT_NONE);
	lv_chart_set_all_value(guider_ui.screen_chart_4, guider_ui.screen_chart_4_0, LV_CHART_POINT_NONE);

	lv_chart_set_x_start_point(guider_ui.screen_chart_1, guider_ui.screen_chart_1_0, 0);
	lv_chart_set_x_start_point(guider_ui.screen_chart_2, guider_ui.screen_chart_2_0, 0);
	lv_chart_set_x_start_point(guider_ui.screen_chart_3, guider_ui.screen_chart_3_0, 0);
	lv_chart_set_x_start_point(guider_ui.screen_chart_4, guider_ui.screen_chart_4_0, 0);

	lv_obj_set_x(guider_ui.screen_img_1, 481);
	lv_obj_set_x(guider_ui.screen_label_1, -357);
	lv_obj_set_x(guider_ui.screen_label_2, -152);
}

/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    PRINTF("Malloc failed. Increase the heap size.");

    for (;;)
        ;
}

/*!
 * @brief FreeRTOS tick hook.
 */
void vApplicationTickHook(void)
{
    if (s_lvgl_initialized)
    {
        lv_tick_inc(1);
    }
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    for (;;)
        ;
}
