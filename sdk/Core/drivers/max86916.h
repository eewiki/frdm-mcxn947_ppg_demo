/*
 * max86216.h
 *
 *  Created on: Sep 14, 2024
 *      Author: Matt
 */

#ifndef MAX86916_H_
#define MAX86916_H_

#include "stdint.h"

#define MAX86916_LED_CURRENT_50MA	0x0
#define MAX86916_LED_CURRENT_100MA	0x1
#define MAX86916_LED_CURRENT_150MA	0x2
#define MAX86916_LED_CURRENT_200MA	0x3

#define MAX86916_LED_PW_70US		0x0
#define MAX86916_LED_PW_120US		0x1
#define MAX86916_LED_PW_220US		0x2
#define MAX86916_LED_PW_420US		0x3

#define MAX86916_LEDCX_OFF			0x0
#define MAX86916_LEDCX_LED1			0x1
#define MAX86916_LEDCX_LED2			0x2
#define MAX86916_LEDCX_LED3			0x3
#define MAX86916_LEDCX_LED4			0x4
#define MAX86916_LEDCX_PILOT_LED1	0x5
#define MAX86916_LEDCX_PILOT_LED2	0x6
#define MAX86916_LEDCX_PILOT_LED3	0x7
#define MAX86916_LEDCX_PILOT_LED4	0x8

#define MAX86916_SMP_AVE_1			0x0
#define MAX86916_SMP_AVE_2			0x1
#define MAX86916_SMP_AVE_4			0x2
#define MAX86916_SMP_AVE_8			0x3
#define MAX86916_SMP_AVE_16			0x4
#define MAX86916_SMP_AVE_32			0x5

#define MAX86916_ROLLOVER_DISABLE	0x0
#define MAX86916_ROLLOVER_ENABLE	0x1

#define MAX86916_ALD_DISABLE		0x1
#define MAX86916_ALD_ENABLE			0x0

#define MAX86916_ADC_RGE_4096		0x0
#define MAX86916_ADC_RGE_8192		0x1
#define MAX86916_ADC_RGE_16384		0x2
#define MAX86916_ADC_RGE_32768		0x3

#define MAX86916_SR_50				0x0
#define MAX86916_SR_100				0x1
#define MAX86916_SR_200				0x2
#define MAX86916_SR_400				0x3
#define MAX86916_SR_800				0x4
#define MAX86916_SR_1000			0x5
#define MAX86916_SR_1600			0x6
#define MAX86916_SR_3200			0x7

#define MAX86916_INT_A_FULL			0x80
#define MAX86916_INT_SMP_RDY		0x40
#define MAX86916_INT_ALC_OVF		0x20
#define MAX86916_INT_PROX_INT		0x10
#define MAX86916_INT_PWR_RDY		0x01


uint8_t MAX86916_getIntStatus();
void MAX86916_enableInterrupt(uint8_t en);
void MAX86916_setAverageSamples(uint8_t ave);
void MAX86916_setFIFORollOver(uint8_t en);
void MAX86916_setFIFOFreeAtInterrupt(uint8_t num);
void MAX86916_shutdown();
void MAX86916_run();
void MAX86916_reset();
void MAX86916_setAmbientLightDet(uint8_t ald);
void MAX86916_setFlexMode(); // TODO: change to _setMode(mode_t mode)
void MAX86916_setADCRange(uint8_t rge);
void MAX86916_setLEDPulseWidth(uint8_t pw);
void MAX86916_setSamplingRate(uint8_t sr);
void MAX86916_setLEDPulseAmplitude(uint8_t pa1, uint8_t pa2, uint8_t pa3, uint8_t pa4);
void MAX86916_setLEDRange(uint8_t rge1, uint8_t rge2, uint8_t rge3, uint8_t rge4);
void MAX86916_setPilotPA(uint8_t pa);
void MAX86916_setLEDSequence(uint8_t ledc1, uint8_t ledc2, uint8_t ledc3, uint8_t ledc4);
void MAX86916_setProximityInterruptThreshold(uint8_t thresh);
uint8_t MAX86916_getRev(void);
uint8_t MAX86916_getID(void);
uint8_t MAX86916_readFIFO(uint8_t *buffer);

#endif /* MAX86916_H_ */
