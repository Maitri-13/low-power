/*
 * setup.c
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#include "em_adc.h"
#include "em_cmu.h"
#include "em_lesense.h"
#include "em_usart.h"
#include "em_acmp.h"
#include "hal-config.h"

#include "../inc/setup.h"
#include "../inc/main.h"

//#define DORESET

/* Init all peripherals*/
int	initPeripherals(void)
{
	setupADC();

	/* Setup UART */
	setupUART();

	/* Setup SPI */
	setupSPI();

	/* Setup GPIO */
	setupGPIO();

	/* Setup ACMP */
	setupACMP();

	//UART_SM();

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
	/* Configure GPIO pins */
	CMU_ClockEnable(cmuClock_GPIO, true);
	/* To avoid false start, configure output as high */
	GPIO_PinModeSet(UART_TX_PORT, UART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_RX_PORT, UART_RX_PIN, gpioModeInput, 0);

	USART_TypeDef           *usart = USART0;
	USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;

	/* Initialize uart clock */
	CMU_ClockEnable(cmuClock_USART0, true);

	init.enable = usartDisable;

	USART_InitAsync(usart, &init);

    /* Enable pins at correct UART/USART location. */
    usart->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
    usart->ROUTELOC0 = (usart->ROUTELOC0
					  & ~(_USART_ROUTELOC0_TXLOC_MASK
						  | _USART_ROUTELOC0_RXLOC_MASK) )
					 | (UART_TX_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)
					 | (UART_RX_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);

    /* Clear previous RX interrupts */
    USART_IntClear(USART0, USART_IF_RXDATAV);
    NVIC_ClearPendingIRQ(USART0_RX_IRQn);

    /* Enable RX interrupts */
    USART_IntEnable(USART0, USART_IF_RXDATAV);
    NVIC_EnableIRQ(USART0_RX_IRQn);

    /* Finally enable it */
    USART_Enable(usart, usartEnable);

	return 0;
}

/* Setup SPI */
int setupSPI(void)
{
	return 0;
}

/* Setup GPIO */
int setupGPIO(void)
{
	/* setup GPIO for ADC */
	GPIO_PinModeSet(PORTIO_ADC0_POS_PORT, PORTIO_ADC0_POS_PIN, gpioModeInput, 0);

	/* Initialize LED driver */
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(LED_PORT, LED_PIN, gpioModePushPull, 0);

	return 0;
}

void ACMP0_IRQHandler(void)
{
	ACMP_IntClear(ACMP0,ACMP_IFC_EDGE);
	LedToggle();
}

/**************************************************************************//**
 * @brief  Sets up the ACMP
 *****************************************************************************/
int setupACMP(void)
{
	CMU_ClockEnable(cmuClock_ACMP0, true);

	/* ACMP configuration constant table. */
	static ACMP_Init_TypeDef acmpInit = ACMP_INIT_DEFAULT;
	acmpInit.interruptOnFallingEdge = true;
	acmpInit.interruptOnRisingEdge = false;
	acmpInit.hysteresisLevel_0 = acmpHysteresisLevel13;
	acmpInit.enable = false;

	/* ACMP configuration constant table. */
	static ACMP_VBConfig_TypeDef vbConf = ACMP_VBCONFIG_DEFAULT;
	vbConf.div0 = 10;
	vbConf.div1 = 10;

	/* Initialize ACMP */
	ACMP_Init(ACMP0, &acmpInit);

	ACMP_VBSetup(ACMP0, &vbConf);

	ACMP_ChannelSet(ACMP0, acmpInputVBDIV, acmpInputAPORT4YCH0);

	/* Disable ACMP0 out to a pin. */
	ACMP_GPIOSetup(ACMP0, 0, false, false);

	ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
	ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);
	/* Enable interrupt in NVIC. */
	NVIC_EnableIRQ(ACMP0_IRQn);

	ACMP_Enable(ACMP0);
	return 0;
}
