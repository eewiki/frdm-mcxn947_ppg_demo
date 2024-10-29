/*
 * max86916.c
 *
 *  Created on: Sep 14, 2024
 *      Author: Matt
 */


#include "max86916.h"
#include "peripherals.h"

#include "board.h"

// Status Registers
#define MAX86916_INTSTAT		0x00
#define MAX86916_INTENABLE		0x02

// FIFO Registers
#define MAX86916_FIFOWRITEPTR	0x04
#define MAX86916_OVERFLOWCNT	0x05
#define MAX86916_FIFOREADPTR	0x06
#define MAX86916_FIFODATA		0x07

// Configuration Registers
#define MAX86916_FIFOCONFIG		0x08
#define MAX86916_MODECONFIG1	0x09
#define MAX86916_MODECONFIG2	0x0A
#define MAX86916_LED1PA			0x0C
#define MAX86916_LED2PA			0x0D
#define MAX86916_LED3PA			0x0E
#define MAX86916_LED4PA			0x0F
#define MAX86916_LEDRANGE		0x11
#define MAX86916_PILOTPA		0x12
#define MAX86916_LEDSEQREG1		0x13
#define MAX86916_LEDSEQREG2		0x14

//Cross talk DAC
#define MAX86916_DAC1XTALK		0x26
#define MAX86916_DAC2XTALK		0x27
#define MAX86916_DAC3XTALK		0x28
#define MAX86916_DAC4XTALK		0x29

// Proximity Function Registers
#define MAX86916_PROXINTTHRESH	0x30

// LED Connectivity Test
#define MAX86916_LEDCOMPENABLE	0x31
#define MAX86916_LEDCOMPSTATUS	0x32

// Part ID Registers
#define MAX86916_REVISIONID		0xFE
#define MAX86916_PARTID			0xFF    // Should always be 0x2B.


lpi2c_master_transfer_t g_masterXfer =
		{.slaveAddress   = 0x57,
		 .direction      = kLPI2C_Write,
		 .subaddress     = 0x00,
		 .subaddressSize = 1,
		 .data           = NULL,
		 .dataSize       = 1,
		 .flags          = kLPI2C_TransferDefaultFlag,
		};

uint8_t g_fifoSampleSize;


void write_reg(uint8_t reg, uint8_t value);
uint8_t read_reg(uint8_t reg);
void read_regs(uint8_t reg, uint8_t *data, uint8_t cnt);

static void UTickDelay(uint32_t usec)
{
	SDK_DelayAtLeastUs(250, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
}


uint8_t MAX86916_getIntStatus()
{
	return read_reg(MAX86916_INTSTAT);

/* This should work!? */
//	uint8_t value;
//
//	LPI2C_MasterStart(LP_FLEXCOMM3_PERIPHERAL, 0x57, kLPI2C_Read);
//	LPI2C_MasterReceive(LP_FLEXCOMM3_PERIPHERAL, &value, 1);
//	LPI2C_MasterStop(LP_FLEXCOMM3_PERIPHERAL);
//
//	return value;
}

void MAX86916_enableInterrupt(uint8_t en)
{
	write_reg(MAX86916_INTENABLE, en);
}

void MAX86916_setAverageSamples(uint8_t ave)
{
	uint8_t value;

	value = read_reg(MAX86916_FIFOCONFIG);
	value &= ~0xE0;
	value |= ave << 5;

	write_reg(MAX86916_FIFOCONFIG, value);
}

void MAX86916_setFIFORollOver(uint8_t en)
{
	uint8_t value;

	value = read_reg(MAX86916_FIFOCONFIG);
	value &= ~0x10;
	value |= en << 4;

	write_reg(MAX86916_FIFOCONFIG, value);
}

void MAX86916_setFIFOFreeAtInterrupt(uint8_t num)
{
	uint8_t value;

	value = read_reg(MAX86916_FIFOCONFIG);
	value &= ~0x0F;
	value |= (num & 0x0F);

	write_reg(MAX86916_FIFOCONFIG, value);
}

void MAX86916_shutdown()
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG1);
	value |= 0x80;

	write_reg(MAX86916_MODECONFIG1, value);
}

void MAX86916_run()
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG1);
	value &= ~0x80;

	write_reg(MAX86916_MODECONFIG1, value);
}

void MAX86916_reset()
{
	write_reg(MAX86916_MODECONFIG1, 0x40);
	while (read_reg(MAX86916_MODECONFIG1) & 0x40)
	{
		UTickDelay(100);
	}

	g_fifoSampleSize = 0;
}

void MAX86916_setAmbientLightDet(uint8_t ald)
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG1);
	value &= ~0x04;
	value |= ald << 2;

	write_reg(MAX86916_MODECONFIG1, value);
}

// TODO: change to _setMode(mode_t mode)
void MAX86916_setFlexMode()
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG1);
	value |= 0x03;

	write_reg(MAX86916_MODECONFIG1, value);
}

void MAX86916_setADCRange(uint8_t rge)
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG2);
	value &= ~0x60;
	value |= rge << 5;

	write_reg(MAX86916_MODECONFIG2, value);
}

void MAX86916_setLEDPulseWidth(uint8_t pw)
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG2);
	value &= ~0x03;
	value |= pw;

	write_reg(MAX86916_MODECONFIG2, value);
}

void MAX86916_setSamplingRate(uint8_t sr)
{
	uint8_t value;

	value = read_reg(MAX86916_MODECONFIG2);
	value &= ~0x1C;
	value |= sr << 2;

	write_reg(MAX86916_MODECONFIG2, value);
}

void MAX86916_setLEDPulseAmplitude(uint8_t pa1, uint8_t pa2, uint8_t pa3, uint8_t pa4)
{
	g_fifoSampleSize = 0;

	// TODO: write_regs()
	write_reg(MAX86916_LED1PA, pa1);
	write_reg(MAX86916_LED2PA, pa2);
	write_reg(MAX86916_LED3PA, pa3);
	write_reg(MAX86916_LED4PA, pa4);

	if (pa1 > 0x0 || pa1 < 0x9)
	{
		g_fifoSampleSize += 3;
	}
	if (pa2 > 0x0 || pa2 < 0x9)
	{
		g_fifoSampleSize += 3;
	}
	if (pa3 > 0x0 || pa3 < 0x9)
	{
		g_fifoSampleSize += 3;
	}
	if (pa4 > 0x0 || pa4 < 0x9)
	{
		g_fifoSampleSize += 3;
	}
}

void MAX86916_setLEDRange(uint8_t rge1, uint8_t rge2, uint8_t rge3, uint8_t rge4)
{
	uint8_t value;

	value = (rge4 << 6) | (rge3 << 4) | (rge2 << 2) | rge1;
	write_reg(MAX86916_LEDRANGE, value);
}

void MAX86916_setPilotPA(uint8_t pa)
{
	write_reg(MAX86916_PILOTPA, pa);
}

void MAX86916_setLEDSequence(uint8_t ledc1, uint8_t ledc2, uint8_t ledc3, uint8_t ledc4)
{
	uint8_t value;

	value = (ledc2 << 4) | ledc1;
	write_reg(MAX86916_LEDSEQREG1, value);
	value = (ledc4 << 4) | ledc3;
	write_reg(MAX86916_LEDSEQREG2, value);
}

void MAX86916_setProximityInterruptThreshold(uint8_t thresh)
{
	write_reg(MAX86916_PROXINTTHRESH, thresh);
}

uint8_t MAX86916_getRev(void)
{
	return read_reg(MAX86916_REVISIONID);
}

uint8_t MAX86916_getID(void)
{
	return read_reg(MAX86916_PARTID);
}

uint8_t MAX86916_readFIFO(uint8_t *buffer)
{
	uint8_t tmp[3];
	uint8_t num_samples;
	uint8_t fifo_wr_ptr;
	uint8_t fifo_ovf_cnt;
	uint8_t fifo_rd_ptr;

	read_regs(MAX86916_FIFOWRITEPTR, tmp, 3);
	fifo_wr_ptr  = tmp[0];
	fifo_ovf_cnt = tmp[1];
	fifo_rd_ptr  = tmp[2];

	if (fifo_ovf_cnt == 0)
	{
		if (fifo_wr_ptr > fifo_rd_ptr)
		{
			num_samples = fifo_wr_ptr - fifo_rd_ptr;
		}
		else
		{
			num_samples = (fifo_wr_ptr + 32) - fifo_rd_ptr;
		}
	}
	else
	{
		num_samples = 32;
	}

	read_regs(MAX86916_FIFODATA, buffer, g_fifoSampleSize * num_samples);

	return num_samples;
}


void write_reg(uint8_t reg, uint8_t value)
{
	g_masterXfer.subaddress = reg;
	g_masterXfer.data = &value;
	g_masterXfer.dataSize = 1;
	g_masterXfer.direction = kLPI2C_Write;

    LPI2C_MasterTransferBlocking(LP_FLEXCOMM3_PERIPHERAL, &g_masterXfer);
}


uint8_t read_reg(uint8_t reg)
{
	uint8_t value;

	g_masterXfer.subaddress = reg;
	g_masterXfer.data = &value;
	g_masterXfer.dataSize = 1;
	g_masterXfer.direction = kLPI2C_Read;

	LPI2C_MasterTransferBlocking(LP_FLEXCOMM3_PERIPHERAL, &g_masterXfer);

	return value;
}

void read_regs(uint8_t reg, uint8_t *data, uint8_t cnt)
{
	g_masterXfer.subaddress = reg;
	g_masterXfer.data = data;
	g_masterXfer.dataSize = cnt;
	g_masterXfer.direction = kLPI2C_Read;

	LPI2C_MasterTransferBlocking(LP_FLEXCOMM3_PERIPHERAL, &g_masterXfer);
}
