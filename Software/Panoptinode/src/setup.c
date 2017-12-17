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

#include <time.h>

//#define DORESET

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

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

	setupACMP();

	/* Setup SPI */
	//setupLESENSE();

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

/*UART VARIABLES*/
#define CMDBUFSIZE    6                          /**< Buffer size for RX */
#define IMGBUFSIZE    512
static volatile int     rxReadIndex  = 0;       /**< Index in buffer to be read */
static volatile int     rxWriteIndex = 0;       /**< Index in buffer to be written to */
static volatile int     imgIndex = 0;       	/**< Index in image buffer to be written to */
static volatile int     rxCount      = 0;       /**< Keeps track of how much data which are stored in the buffer */
static volatile uint8_t is_received_data = 0;
static volatile uint8_t is_image_data = 0;
static uint8_t cmdBuffer[CMDBUFSIZE];    		  /**< Buffer to store cmd data */
static volatile uint8_t rxBuffer[CMDBUFSIZE];     /**< Buffer to store received data */
static uint8_t verifyBuffer[CMDBUFSIZE]; 		  /**< Buffer to verify received data */
static volatile uint8_t imgBuffer[IMGBUFSIZE];    /**< Buffer to store image data */

/**************************************************************************//**
 * @brief UART/LEUART IRQ Handler
 *****************************************************************************/
void USART0_RX_IRQHandler(void)
{
  if (USART0->STATUS & USART_STATUS_RXDATAV) {
	is_received_data = 1;
	if (is_image_data == 0){
		/* Store Data */
		rxBuffer[rxWriteIndex] = USART_Rx(USART0);
		rxWriteIndex++;
		rxCount++;
		if (rxWriteIndex == CMDBUFSIZE) {
		  rxWriteIndex = 0;
		}
		/* Check for overflow - flush buffer */
		if (rxCount > CMDBUFSIZE) {
		  rxWriteIndex = 0;
		  rxCount      = 0;
		  rxReadIndex  = 0;
		}
	}
	else{
		imgBuffer[imgIndex] = USART_Rx(USART0);
		imgIndex++;
		if (imgIndex == IMGBUFSIZE)
			imgIndex = 0;
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
    if (rxReadIndex == CMDBUFSIZE) {
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
static inline void UART_WriteChar(char c)
{
  USART_Tx(USART0, c);
}

int UART_WriteBuff(uint8_t *pCmdBuffer,int len)
{
	for(int i = 0; i < len; i++)
		UART_WriteChar(pCmdBuffer[i]);
	return 0;
}

int UART_VerifyBuff(uint8_t *pverifyBuff,int len)
{
	//wait till you receive the data
	while(!is_received_data) UART_WriteBuff(cmdBuffer,CMDBUFSIZE);
	is_received_data = 0;
	for(int i = 0; i < len; i++)
	{
		// unexpected data
		if (pverifyBuff[i] != rxBuffer[i])
			return 1;
	}
	return 0;
}

void resetCam(void)
{
	cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x08;
	cmdBuffer[2] = 0x00;	cmdBuffer[3] = 0x00;
	cmdBuffer[4] = 0x00;	cmdBuffer[5] = 0xFF;
	UART_WriteBuff(cmdBuffer,CMDBUFSIZE);
	delay(5);

	// expected response
	verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x08;

	// keep sending it until correct data is received
	while(UART_VerifyBuff(verifyBuffer,2)){
		UART_WriteBuff(cmdBuffer,CMDBUFSIZE);
	}
}

int syncCam(void)
{
	uint16_t counter = 0;
	// Send Sync command
	cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x0D;
	cmdBuffer[2] = 0x00;	cmdBuffer[3] = 0x00;
	cmdBuffer[4] = 0x00;	cmdBuffer[5] = 0x00;
	UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

	// expected response
	verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0D;

	// keep sending it until correct data is received
	while(UART_VerifyBuff(verifyBuffer,2)){
		UART_WriteBuff(cmdBuffer,CMDBUFSIZE);
		counter ++;
	}

	// wait for some time
	for (int i= 0; i<65500; i++);

	cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x0E;
	cmdBuffer[2] = 0x0D;	cmdBuffer[3] = 0x00;
	cmdBuffer[4] = 0x00;	cmdBuffer[5] = 0x00;
	UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

	return 0;
}

void UART_SM(void)
{
	uint32_t imgSize = 0;
#ifdef DORESET
	UART_STATE state = RESET;
#else
	UART_STATE state = SYNC;
#endif
	while(state != EXIT)
	{
		switch(state)
		{
			case RESET:
				resetCam();
				state = SYNC;
				break;
			case SYNC:
				syncCam();
				state = INIT_JPEG;
				break;
			case INIT_JPEG:
				// Send command
				cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x01;
				cmdBuffer[2] = 0x00;	cmdBuffer[3] = 0x07;
				cmdBuffer[4] = 0x07;	cmdBuffer[5] = 0x07;
				UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

				// expected response
				verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0E;
				verifyBuffer[2] = 0x01;
				while(UART_VerifyBuff(verifyBuffer,3));

				state = SET_PKG_SIZE;
				break;
			case SET_PKG_SIZE:
				// Send command
				cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x06;
				cmdBuffer[2] = 0x08;	cmdBuffer[3] = 0x00;
				cmdBuffer[4] = 0x02;	cmdBuffer[5] = 0x00;
				UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

				// expected response
				verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0E;
				verifyBuffer[2] = 0x06;
				while(UART_VerifyBuff(verifyBuffer,3));

				state = SNAPSHOT;
				break;
			case SNAPSHOT:
				// Send command
				cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x05;
				cmdBuffer[2] = 0x00;	cmdBuffer[3] = 0x00;
				cmdBuffer[4] = 0x00;	cmdBuffer[5] = 0x00;
				UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

				// expected response
				verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0E;
				verifyBuffer[2] = 0x05;
				while(UART_VerifyBuff(verifyBuffer,3));

				state = GET_PICTURE_CMD;
				break;
			case GET_PICTURE_CMD:
				// Send command
				cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x04;
				cmdBuffer[2] = 0x01;	cmdBuffer[3] = 0x00;
				cmdBuffer[4] = 0x00;	cmdBuffer[5] = 0x00;
				UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

				// expected response
				verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0E;
				verifyBuffer[2] = 0x04;

				/* @FIXME: Everytime returns AA 0E 00 XX XX
				 * returning ACK correctly
				 * that means the command ID 00 is wrong
				 * ACK counter == XX
				 * */
				while(UART_VerifyBuff(verifyBuffer,3));

				state = GET_PICTURE;
				break;
			case GET_PICTURE:
				// expected data response
				verifyBuffer[0] = 0xAA;	verifyBuffer[1] = 0x0A;
				verifyBuffer[2] = 0x01;
				// wait till you get 1st data
				while(UART_VerifyBuff(verifyBuffer,3));

				// extract size from the response
				imgSize = rxBuffer[3] + rxBuffer[4] + rxBuffer[5];

				// calculate number of packages
				int numPackages  = imgSize/(IMGBUFSIZE - CMDBUFSIZE);

				// initialize data buffer
				uint8_t dataBuffer[imgSize];
				uint32_t dataBufferIndex = 0;

				// initial package id 0x00, 0x00
				uint8_t pck_1st_byte = 0x00;
				uint8_t pck_2nd_byte = 0x00;

				// set image data flag
				is_image_data = 1;
				while(numPackages--)
				{
					// Send ACKs
					cmdBuffer[0] = 0xAA;			cmdBuffer[1] = 0x0E;
					cmdBuffer[2] = 0x00;			cmdBuffer[3] = 0x00;
					cmdBuffer[4] = pck_1st_byte;	cmdBuffer[5] = pck_2nd_byte;
					UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

					//wait till you receive the data
					while(!is_received_data);
					is_received_data = 0;

					// Set data for next ACK
					pck_1st_byte = imgBuffer[0];
					pck_1st_byte = imgBuffer[1];

					// copy image data into data buffer
					for(int i=0;i<(imgBuffer[2]+imgBuffer[3]);i++){
						dataBuffer[i+dataBufferIndex] = imgBuffer[i+4];
						dataBufferIndex++;
					}
				}

				//send end packet
				cmdBuffer[0] = 0xAA;	cmdBuffer[1] = 0x0E;
				cmdBuffer[2] = 0x00;	cmdBuffer[3] = 0x00;
				cmdBuffer[4] = 0xF0;	cmdBuffer[5] = 0xF0;
				UART_WriteBuff(cmdBuffer,CMDBUFSIZE);

				state = EXIT;
				break;
			/*
			 default:
				// Something really bad happened
				state = EXIT;
				break;
			*/
		}
	}
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
	acmpInit.interruptOnRisingEdge = true;
	acmpInit.hysteresisLevel_0 = acmpHysteresisLevel13;
	acmpInit.enable = false;

	/* ACMP configuration constant table. */
	static ACMP_VBConfig_TypeDef vbConf = ACMP_VBCONFIG_DEFAULT;
	vbConf.div0 = 4;
	vbConf.div1 = 4;

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
