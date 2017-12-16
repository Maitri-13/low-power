/*
 * setup.c
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#include "em_adc.h"
#include "em_cmu.h"
#include "em_lesense.h"
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

void LedToggle()
{
	GPIO_PinOutSet(gpioPortD, 14);
	Delay(1000);
	GPIO_PinOutClear(gpioPortD, 14);
	Delay(1000);
}

void LESENSE_IRQHandler(void)
{
  /* Clear interrupt flag */
  LESENSE_IntClear(1<<PORTIO_LESENSE_CH0_PIN);
  LedToggle();
}

/* Setup SPI */
int setupLESENSE(void)
{
	/*Enable lesense clock*/
	CMU_ClockEnable(cmuClock_LESENSE, true);

	/*Setting default configuration*/
	LESENSE_Init_TypeDef init = LESENSE_INIT_DEFAULT;

	/*Changing the necessary variables from the default config*/
	/*Disabling the decoder*/
	init.decCtrl.hystIRQ = false;
	init.decCtrl.hystPRS0 = false;
	init.decCtrl.hystPRS1 = false;
	init.decCtrl.hystPRS2 = false;

	/*enabling the prs count*/
	init.decCtrl.prsCount = true;

	LESENSE_Init(&init,true);

	/* Channel configuration */
	/* Only one channel is configured for the motion sense application. */
	LESENSE_ChDesc_TypeDef initLesenseCh = LESENSE_CH_CONF_DEFAULT;

	/*Changing the necessary variables from the default config*/
	initLesenseCh.enaScanCh = true;
	initLesenseCh.enaInt = true;
	initLesenseCh.intMode = lesenseSetIntNegEdge;
	/*Set threshold*/
	initLesenseCh.acmpThres = 0x10;

	/* Configure LESENSE channel */
	GPIO_PinModeSet(PORTIO_LESENSE_CH0_PORT, PORTIO_LESENSE_CH0_PIN, gpioModePushPull, 0);
	LESENSE_ChannelConfig(&initLesenseCh, PORTIO_LESENSE_CH0_PIN);

	/* Enable interrupt in NVIC. */
	NVIC_EnableIRQ(LESENSE_IRQn);

	/* Start scan. */
	LESENSE_ScanStart();
	return 0;
}
