/*
 * adc.c
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#include "em_adc.h"

#include "../inc/adc.h"

/***************************************************************************//**
 * @brief  Do one ADC conversion
 * @return ADC conversion result
 ******************************************************************************/
static uint32_t AdcRead(void)
{
  ADC_Start(ADC0, adcStartSingle);
  while ( (ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0 ) {
  }
  return ADC_DataSingleGet(ADC0);
}

/***************************************************************************//**************
 * @brief Convert ADC sample values to volts.
 * @detail Calibration from
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 *
 * @return Volts.
 *******************************************************************************************/
double convertADCtoVolt()
{
	uint32_t signalMax = 0;
	uint32_t signalMin = 1024;
	for (int i = 0; i< 500; i++)
	{
		int32_t sample = AdcRead();
		if (sample < 1024)  // toss out spurious readings
		{
			if (sample > signalMax)
			{
				signalMax = sample;  // save just the max levels
			}
			else if (sample < signalMin)
			{
				signalMin = sample;  // save just the min levels
			}
		}
	 }
	 uint32_t peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
	 double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
	 return volts;
}

