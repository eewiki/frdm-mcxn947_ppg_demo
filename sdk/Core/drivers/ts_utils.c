/*
 * ts_utils.c
 *
 *  Created on: Sep 30, 2024
 *      Author: Matt
 */

#include "ts_utils.h"
#include "math.h"

void _scale_init(dynamic_scaler_t *s, float scale)
{
	s->scale = scale;
	s->scale_prev = scale;
	s->_m = 0;
	s->steps = 1;
	s->_step = 0;
}

void scale_adjust(dynamic_scaler_t *s, float *data, uint16_t len, uint32_t target, uint32_t th_high, uint32_t th_low)
{
	uint8_t stable_flag;
	uint16_t i;
	float max_mag;

	/* determine maximum magnitude */
	stable_flag = 0;
	max_mag = 0;
	for (i = 0; i < len; i++)
	{
		if (fabs(data[i]) > max_mag)
		{
			max_mag = data[i];
		}
	}

	/* adjust the scaler if needed */
	if (max_mag > (th_high / s->scale_prev))
	{
		if (stable_flag == 0)
		{
			s->scale = target / max_mag;
		}
		else
		{
			if ((s->scale_prev / 2) < (target / max_mag))
			{
				if ((max_mag * (s->scale_prev / 2)) <= th_high)
				{
					s->scale = s->scale_prev / 2;
				}
				else
				{
					s->scale = s->scale_prev;
					stable_flag -= 1;
				}
			}
			else
			{
				s->scale = target / max_mag;
			}
		}
	}
	else if (max_mag < (th_low / s->scale_prev))
	{
		if (stable_flag == 0)
		{
			s->scale = target / max_mag;
		}
		else
		{
			if ((s->scale_prev * 2) > (target / max_mag))
			{
				if (max_mag * (s->scale_prev * 2) >= th_low)
				{
					s->scale = s->scale_prev * 2;
				}
				else
				{
					s->scale = s->scale_prev;
					stable_flag -= 1;
				}
			}
			else
			{
				s->scale = target / max_mag;
			}
		}
	}
	else
	{
		s->scale = s->scale_prev;
		stable_flag = (stable_flag < 3) ? stable_flag + 1 : stable_flag;
	}

	/* make sure we don't divide by zero */
	if (s->scale == 0)
	{
		s->scale += 1e-6;
	}

	/* reset scale interpolation */
	s->_m = (s->scale - s->scale_prev) / len;
	s->steps = len;
	s->_step = 0;

//	/* scale the data */
//	if (s == s_prev)
//	{
//		for (i = 0; i < len; i++)
//		{
//			data[i] *= s;
//		}
//	}
//	else
//	{
//		m = (s - s_prev) / len;
//		for (i = 0; i < len; i++)
//		{
//			data[i] *= ((i * m) + s_prev);
//		}
//	}
//
//	return s;
}

float _scale_dynamic(dynamic_scaler_t *s, float data, uint8_t skip)
{
	data *= (s->_m * s->_step) + s->scale_prev;

	s->_step += (1 + skip);
	if (s->_step > s->steps)
	{
		s->_step = s->steps;
	}
}

