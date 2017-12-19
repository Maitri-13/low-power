/*
 * setup.h
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#ifndef INC_SETUP_H_
#define INC_SETUP_H_

#include "../inc/spi.h"
#include "../inc/led.h"

#include "uartdrv.h"
#include "spidrv.h"

#define MY_UART                                     \
{                                                   \
  USART0,                                           \
  57600,                                            \
  UART_TX_LOC,					                    \
  UART_RX_LOC,                  					\
  usartStopbits1,                                   \
  usartNoParity,                                    \
  usartOVS16,                                       \
  false,                                            \
  uartdrvFlowControlNone, 	                        \
  gpioPortA,                                        \
  4,                                                \
  gpioPortA,                                        \
  5,                                                \
  (UARTDRV_Buffer_FifoQueue_t *)&rxBufferQueue,     \
  (UARTDRV_Buffer_FifoQueue_t *)&txBufferQueue,     \
}

UARTDRV_HandleData_t UART_handleData;
UARTDRV_Handle_t UART_handle;


SPIDRV_HandleData_t SPI_handleData;
SPIDRV_Handle_t SPI_handle;


/* Init all peripherals*/
int	initPeripherals(void);

/* Setup ACMP */
int setupACMP(void);

/* Setup ADC */
int setupADC(void);

/* Setup UART */
int setupUART(void);

/* Setup UART */
int setupUART2(void);

/* Setup SPI */
int setupSPI(void);

/* Setup SPI */
int setupSPI2(void);

/* Setup GPIO */
int setupLED(void);

/* IRQHandler for ACMP */
void ACMP0_IRQHandler(void);

int setupTimer(void);



#endif /* INC_SETUP_H_ */
