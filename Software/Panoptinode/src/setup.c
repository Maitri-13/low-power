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

#include "hal-config.h"

#include "../inc/setup.h"

volatile uint8_t is_synced = 0;

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

	syncuCam();

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

/*UART VARIABLES*/
#define RXBUFSIZE    8                          /**< Buffer size for RX */
static volatile int     rxReadIndex  = 0;       /**< Index in buffer to be read */
static volatile int     rxWriteIndex = 0;       /**< Index in buffer to be written to */
static volatile int     rxCount      = 0;       /**< Keeps track of how much data which are stored in the buffer */
static volatile uint8_t rxBuffer[RXBUFSIZE];    /**< Buffer to store data */

/**************************************************************************//**
 * @brief UART/LEUART IRQ Handler
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  if (USART0->STATUS & USART_STATUS_RXDATAV) {


	is_synced = 1;
	/* Store Data */
    rxBuffer[rxWriteIndex] = USART_Rx(USART0);
    rxWriteIndex++;
    rxCount++;
    if (rxWriteIndex == RXBUFSIZE) {
      rxWriteIndex = 0;
    }
    /* Check for overflow - flush buffer */
    if (rxCount > RXBUFSIZE) {
      rxWriteIndex = 0;
      rxCount      = 0;
      rxReadIndex  = 0;
    }
  }
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

/**************************************************************************//**
 * @brief Receive a byte from USART/LEUART and put into global buffer
 * @return -1 on failure, or positive character integer on sucesss
 *****************************************************************************/
int UART_ReadChar(void)
{
  int c = -1;

  if (rxCount > 0) {
    c = rxBuffer[rxReadIndex];
    rxReadIndex++;
    if (rxReadIndex == RXBUFSIZE) {
      rxReadIndex = 0;
    }
    rxCount--;
  }

  return c;
}

/**************************************************************************//**
 * @brief Transmit single byte to USART/LEUART
 * @param c Character to transmit
 * @return Transmitted character
 *****************************************************************************/
int UART_WriteChar(char c)
{
  USART_Tx(USART0, c);

  return c;
}

int syncuCam(void)
{
	uint16_t counter = 0;

	while( is_synced == 0){
		// Send Sync command
		USART_Tx(USART0, 0xAA);
		USART_Tx(USART0, 0x0D);
		USART_Tx(USART0, 0);
		USART_Tx(USART0, 0);
		USART_Tx(USART0, 0);
		USART_Tx(USART0, 0);
		counter ++;
	}

	for (int i= 0; i<65500; i++)
	{
		for(int j=0;j<65500;j++);
	}

	USART_Tx(USART0, 0xAA);
	USART_Tx(USART0, 0x0E);
	USART_Tx(USART0, 0x0D);
	USART_Tx(USART0, 0);
	USART_Tx(USART0, 0);
	USART_Tx(USART0, 0);


	//(AA 01 00 07 07 07

	// (AA 06 08 00 02 00)

	// (AA 05 00 00 00 00)

	// (AA 04 01 00 00 00)


	//BSP_LedSet(1);
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
