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
#include "em_timer.h"
#include "hal-config.h"

#include "spidrv.h"
#include "uartdrv.h"

#include "../inc/setup.h"

DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_RX_BUFS, rxBufferQueue);
DEFINE_BUF_QUEUE(EMDRV_UARTDRV_MAX_CONCURRENT_TX_BUFS, txBufferQueue);

/* 13671 Hz -> 14Mhz (clock frequency) / 1024 (prescaler)
  Setting TOP to 27342 results in an overflow each 2 seconds */
#define TOP 27342
volatile int led_count = 0;
volatile bool uart_flag = true;


/**************************************************************************//**
 * @brief handle ACMP wakeups
 *****************************************************************************/
void ACMP0_IRQHandler(void)
{
	ACMP_IntClear(ACMP0,ACMP_IFC_EDGE);
	ACMP_IntClear(ACMP0,ACMP_IFC_WARMUP);
	ACMP_IntDisable(ACMP0, ACMP_IEN_EDGE);
	ACMP_IntDisable(ACMP0, ACMP_IEN_WARMUP);
	led_set();
}

/**************************************************************************//**
 * @brief  INIT Peripherals one-by-one
 *****************************************************************************/
int	initPeripherals(void)
{
	/* Setup GPIO */
	setupLED();

	/* Setup the microphone ADC */
	//setupADC();

	/* Setup UART */
	setupUART2();

	/* Setup SPI */
	setupSPI2();

	setupTimer();

	/* Setup ACMP */
	setupACMP();

	/* Setup GPIO*/
	setupGPIO();

	return 0;
}

/**************************************************************************//**
 * @brief TIMER0_IRQHandler
 * Interrupt Service Routine TIMER0 Interrupt Line
 *****************************************************************************/
void TIMER0_IRQHandler(void)
{
  /* Clear flag for TIMER0 overflow interrupt */
  TIMER_IntClear(TIMER0, TIMER_IF_OF);

  /* Toggle LED ON/OFF */
  led_count++;
  if(led_count==10)	led_count = 0;

  uart_flag = false;
}

/**************************************************************************//**
 * @brief  Sets up the Timer
 *****************************************************************************/
int setupTimer(void)
{
	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	/* Select TIMER0 parameters */
	  TIMER_Init_TypeDef timerInit =
	  {
		.enable     = true,
		.debugRun   = true,
		.prescale   = timerPrescale1024,
		.clkSel     = timerClkSelHFPerClk,
		.fallAction = timerInputActionNone,
		.riseAction = timerInputActionNone,
		.mode       = timerModeUp,
		.dmaClrAct  = false,
		.quadModeX4 = false,
		.oneShot    = false,
		.sync       = false,
	  };

	  /* Enable overflow interrupt */
	  TIMER_IntEnable(TIMER0, TIMER_IF_OF);

	  /* Enable TIMER0 interrupt vector in NVIC */
	  NVIC_EnableIRQ(TIMER0_IRQn);

	  /* Set TIMER Top value */
	  TIMER_TopSet(TIMER0, TOP);

	  /* Configure TIMER */
	  TIMER_Init(TIMER0, &timerInit);

	  return 0;
}

/**************************************************************************//**
 * @brief  Sets up the ADC
 *****************************************************************************/
int setupADC(void)
{
	/* Enable ADC clock */
	CMU_ClockEnable(cmuClock_ADC0, true);

	/* setup GPIO for ADC */
	GPIO_PinModeSet(PORTIO_ADC0_POS_PORT, PORTIO_ADC0_POS_PIN, gpioModeInput, 0);

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

/**************************************************************************//**
 * @brief  Sets up the UART using UARTDRV
 *****************************************************************************/
int setupUART2(void)
{
	/* Initialize USART0 clock */
	CMU_ClockEnable(cmuClock_USART0, true);

	UART_handle = &UART_handleData;

	/* setup the UART type*/
	UARTDRV_InitUart_t initData = MY_UART; /* check setup.h */
	UARTDRV_InitUart(UART_handle, &initData);

	GPIO_PinModeSet(UCAM_RST_PORT, UCAM_RST_PIN, gpioModePushPull, 0);

	return 0;
}

/**************************************************************************//**
 * @brief  Sets up the UART
 *****************************************************************************/
int setupUART(void)
{
	/* Initialize USART0 clock */
	CMU_ClockEnable(cmuClock_USART0, true);

	/* To avoid false start, configure output as high */
	GPIO_PinModeSet(UART_TX_PORT, UART_TX_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(UART_RX_PORT, UART_RX_PIN, gpioModeInput, 0);

	/* use default init typeDef */
	USART_InitAsync_TypeDef init   = USART_INITASYNC_DEFAULT;
	init.enable = usartDisable;

	/* use the init */
	USART_InitAsync(USART0, &init);

    /* Enable pins at correct UART/USART location. */
    USART0->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
    USART0->ROUTELOC0 = (USART0->ROUTELOC0
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
    USART_Enable(USART0, usartEnable);

	return 0;
}


/**************************************************************************//**
 * @brief  Sets up the SPI using SPIDRV
 *****************************************************************************/

int setupSPI2(void)
{

	SPI_handle = &SPI_handleData;

	///* Initialize uart clock /
	CMU_ClockEnable(cmuClock_USART1, true);

	//* Initialize the SPI /
	SPIDRV_Init_t initSPI   = SPIDRV_MASTER_USART1;
	initSPI.portLocationClk = SPI_CLK_LOC;
	initSPI.portLocationCs  = SPI_CS_LOC;
	initSPI.portLocationTx  = SPI_MOSI_LOC;
	initSPI.portLocationRx  = SPI_MISO_LOC;
	initSPI.clockMode		= spidrvClockMode0;
	initSPI.csControl       = spidrvCsControlApplication;
	initSPI.bitOrder		= spidrvBitOrderMsbFirst;

	SPIDRV_Init(SPI_handle,&initSPI);

	//* Enable RX interrupts /
    USART_IntEnable(USART1, USART_IF_RXDATAV);
    NVIC_EnableIRQ(USART1_RX_IRQn);



	GPIO_PinModeSet(MEM_HOLD_PORT, MEM_HOLD_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(MEM_WP_PORT, MEM_WP_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(MEM_CS_PORT, MEM_CS_PIN, gpioModePushPull, 1);
	GPIO_PinModeSet(SPI_CS_PORT, SPI_CS_PIN, gpioModePushPull, 1);

	/* setup SPI READY interrupt*/
	GPIO_PinModeSet(gpioPortA, 4, gpioModeInputPull, 1);



    return 0;
}

/**************************************************************************//**
 * @brief  Sets up the SPI using USART
 *****************************************************************************/
int setupSPI(void)
{
	/* Initialize uart clock */
	CMU_ClockEnable(cmuClock_USART1, true);

	USART_InitSync_TypeDef initSync = USART_INITSYNC_DEFAULT;
	initSync.master = true;

	initSync.clockMode = usartClockMode1;
	USART_InitSync(USART1, &initSync);

	/* Enable I/O and set location */
	USART1->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN | USART_ROUTEPEN_CSPEN | USART_ROUTEPEN_CLKPEN ;
	USART1->ROUTELOC0 = (USART0->ROUTELOC0 &
		~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK ))
			| (SPI_MOSI_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)
			| (SPI_MISO_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);
	/* */
	USART1->ROUTELOC0 = (USART1->ROUTELOC0 &
		~(_USART_ROUTELOC0_CSLOC_MASK | _USART_ROUTELOC0_CLKLOC_MASK ))
			| (SPI_CS_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)
			| (SPI_CLK_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);
	GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_TX_PORT(SPI_MOSI_LOC), AF_USART0_TX_PIN(SPI_MOSI_LOC), gpioModePushPull, 0);
	GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_RX_PORT(SPI_MISO_LOC), AF_USART0_RX_PIN(SPI_MISO_LOC), gpioModeInput, 0);
	GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_CS_PORT(SPI_CS_LOC), AF_USART0_CS_PIN(SPI_CS_LOC), gpioModePushPull, 0);
	GPIO_PinModeSet((GPIO_Port_TypeDef)AF_USART0_CLK_PORT(SPI_CLK_LOC), AF_USART0_CLK_PIN(SPI_CLK_LOC), gpioModePushPull, 0);

	return 0;
}


/**************************************************************************//**
 * @brief  Sets up the LED
 *****************************************************************************/
int setupLED(void)
{
	/* Initialize LED driver */
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(LED_PORT, LED_PIN, gpioModePushPull, 0);

	return 0;
}


/**************************************************************************//**
 * @brief  Sets up the ACMP
 *****************************************************************************/
int setupACMP(void)
{
	/* enable ACMP clock */
	CMU_ClockEnable(cmuClock_ACMP0, true);

	/* ACMP configuration constant table. */
	static ACMP_Init_TypeDef acmpInit = ACMP_INIT_DEFAULT;
	acmpInit.interruptOnFallingEdge = true;
	acmpInit.interruptOnRisingEdge = false;
	acmpInit.hysteresisLevel_0 = acmpHysteresisLevel13;
	acmpInit.enable = false;

	/* Initialize ACMP */
	ACMP_Init(ACMP0, &acmpInit);

	/* Set comparator value and set comparator inputs */
	static ACMP_VBConfig_TypeDef vbConf = ACMP_VBCONFIG_DEFAULT;
	vbConf.div0 = 10;
	vbConf.div1 = 10;
	ACMP_VBSetup(ACMP0, &vbConf);
	ACMP_ChannelSet(ACMP0, acmpInputVBDIV, acmpInputAPORT4YCH0);

	/* Disable ACMP0 out to a pin. */
	ACMP_GPIOSetup(ACMP0, 0, false, false);

	/* Enable interrupt in NVIC. */
	ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
	ACMP_IntClear(ACMP0, ACMP_IFC_EDGE);
	NVIC_EnableIRQ(ACMP0_IRQn);

	/* enable and we're done here*/
	ACMP_Enable(ACMP0);
	return 0;
}

int setupGPIO(void)
{

	GPIO_PinModeSet(gpioPortA, 4, gpioModeInputPullFilter, 0);

	/* Set falling edge interrupt for A4 */
	GPIO_IntConfig(gpioPortA, 4, true, true, true);

	/* Enable interrupt in core for even and odd gpio interrupts */
	NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

}
