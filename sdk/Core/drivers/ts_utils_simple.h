/*
 * ts_utils.h
 *
 *  Created on: Sep 30, 2024
 *      Author: Matt
 */

#ifndef TS_UTILS_H_
#define TS_UTILS_H_

#include "stdint.h"

typedef struct {
	float scale;
	float peak;
	uint16_t cnt;
} dynamic_scaler_t;

typedef struct {
	float data_prev;
	float data_pprev;
	uint8_t bpm;
	uint16_t cnt;
	uint32_t rate;
	uint16_t rates[10];
	uint8_t i;
	uint8_t max_beats;
	uint8_t beats;
	uint16_t sps;
} hb_detector_t;

void scale_init(dynamic_scaler_t *s, float scale);
float scale_dynamic(dynamic_scaler_t *s, float data);
void hb_detect_init(hb_detector_t *hb, uint8_t rolling_beats, uint16_t sps);
uint8_t hb_detect_step(hb_detector_t *hb, float data, int16_t width);


#endif /* TS_UTILS_H_ */
