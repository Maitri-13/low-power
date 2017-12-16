/*
 * setup.c
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#include "em_adc.h"
#include "em_cmu.h"
#include "hal-config.h"


#include "../inc/setup.h"


/* Init all peripherals*/
int	initPeripherals(void)
{
	setupADC();

	/* Setup UART */
	setupUART();

	/* Setup SPI */
	setupSPI();

	/* Setup SPI */
	setupGPIO();

	/* Setup SPI */
	setupLESENSE();

	return 0;
}

/* Setup ADC */
int setupADC(void)
{
	/* Enable ADC clock */
	CMU_ClockEnable(cmuClock_ADC0, true);

	/* Base the ADC configuration on the default setup. */
	ADC_Init_TypeDef       init  = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;

	/* Initialize timebases */
	init.timebase = ADC_TimebaseCalc(0);
	init.prescale = ADC_PrescaleCalc(400000, 0);
	ADC_Init(ADC0, &init);

	/* Set input to temperature sensor. Reference must be 1.25V */
	sInit.reference   = adcRef5V;
	sInit.acqTime     = adcAcqTime8; /* Minimum time for temperature sensor */
	sInit.posSel      = PORTIO_ADC0_POS_LOC; /*PD10 ADC Pin*/
	ADC_InitSingle(ADC0, &sInit);

	return 0;
}

/* Setup UART */
int setupUART(void)
{
	return 0;
}

/* Setup SPI */
int setupSPI(void)
{
	return 0;
}

/* Setup SPI */
int setupGPIO(void)
{
	/* setup GPIO for ADC */
	GPIO_PinModeSet(PORTIO_ADC0_POS_PORT, PORTIO_ADC0_POS_PIN, gpioModeInput, 0);

	/* Initialize LED driver */
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(LED_PORT, LED_PIN, gpioModePushPull, 0);

	return 0;
}

/* Setup SPI */
int setupLESENSE(void)
{
	return 0;
}
