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
	float scale_prev;
	float _m;
	uint16_t steps;
	uint16_t _step;
} dynamic_scaler_t;

void _scale_init(dynamic_scaler_t *s, float scale);
void scale_adjust(dynamic_scaler_t *s, float *data, uint16_t len, uint32_t target, uint32_t th_high, uint32_t th_low);
float _scale_dynamic(dynamic_scaler_t *s, float data, uint8_t skip);


#endif /* TS_UTILS_H_ */
