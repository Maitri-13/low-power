/*
 * adc.h
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_


/***************************************************************************//**
 * @brief  Do one ADC conversion
 * @return ADC conversion result
 ******************************************************************************/
static uint32_t AdcRead(void);

/***************************************************************************//**************
 * @brief Convert ADC sample values to volts.
 * @detail Calibration from
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 *
 * @return Volts.
 *******************************************************************************************/
double convertADCtoVolt();





#endif /* INC_ADC_H_ */
