/*
 * ts_utils.c
 *
 *  Created on: Sep 30, 2024
 *      Author: Matt
 */

#include "ts_utils_simple.h"
#include "math.h"

void scale_init(dynamic_scaler_t *s, float scale)
{
	s->scale = scale;
	s->peak = 0;
	s->cnt = 0;
}

float scale_dynamic(dynamic_scaler_t *s, float data)
{
	float scaled;

	// scale the input data
	scaled = data * s->scale;

	// find the peak magnitude in the time series
	if (fabs(scaled) > s->peak)
	{
		s->peak = fabs(scaled);
	}

	// if data has yet to stabilize,
	// wait for it to do so
	if (s->peak > 255)
	{
		s->peak = 0;
		s->cnt = 0;
		scaled = 0;
	}
	// if the current peak is larger than desired,
	// adjust scaler and re-scale the data
	else if (s->peak > 127)
	{
		s->scale /= 1.3;
		s->peak = 0;
		s->cnt = 0;

		scaled = data * s->scale;
	}
	// if the current peak is smaller than desired,
	// adjust scaler and re-scale the data
	else if (s->cnt > 28)
	{
		if (s->peak < 69)
		{
			s->scale *= ((s->peak * -0.3) + 20.8);
			scaled = data * s->scale;
		}

		s->peak = 0;
		s->cnt = 0;
	}
	else
	{
		s->cnt++;
	}

	return scaled;
}

void hb_detect_init(hb_detector_t *hb, uint8_t rolling_beats, uint16_t sps)
{
	uint8_t i;

	hb->data_prev = 0;
	hb->data_pprev = 0;
	hb->bpm = 0;
	hb->cnt = 0;
	hb->rate = 0;

	for (i = 0; i < 10; i++)
	{
		hb->rates[i] = 0;
	}

	hb->i = 0;

	if (rolling_beats > 10)
	{
		rolling_beats = 10;
	}

	hb->max_beats = rolling_beats;
	hb->beats = 0;
	hb->sps = sps;
}

uint8_t hb_detect_step(hb_detector_t *hb, float data, int16_t width)
{
	float bpm;

	hb->cnt++;

	/* if there is a negative zero crossing... */
	if ((data <= 0) && (hb->data_prev > 0) && (hb->data_pprev > 0))
	{
		if ((hb->cnt > width) || (hb->beats == 0))
		{
			hb->rate -= hb->rates[hb->i];
			hb->rate += hb->cnt;
			hb->rates[hb->i] = hb->cnt;

			hb->i = (hb->i + 1) % hb->max_beats;
			hb->beats++;
			if (hb->beats > hb->max_beats)
			{
				hb->beats = hb->max_beats;
			}

			hb->cnt = 0;

			/* re-calculate HR */
			bpm = ((float)(hb->sps) / ((float)(hb->rate) / (float)(hb->beats)));
			bpm *= 60.0;
			if (bpm > 199)
			{
				bpm = 199;
			}
			else if (bpm < 30)
			{
				bpm = 30;
			}
			hb->bpm = (int)(bpm + 0.5);
		}
	}

	hb->data_pprev = hb->data_prev;
	hb->data_prev = data;

	return hb->bpm;
}

