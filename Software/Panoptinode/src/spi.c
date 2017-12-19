/*
 * spi.c
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */

#include "hal-config.h"
#include "em_gpio.h"
#include "spidrv.h"

#include "../inc/setup.h"
#include "../inc/spi.h"

#define DEVICE_ID_LEN  4

/* MEM CHIP DEVICE ID*/
uint8_t  DEVICE_ID[] = {0x90, 0x00, 0x00, 0x01};


uint8_t  WGM_HELLO[] = {0x00, 0x00, 0x00, 0x01};
uint8_t  WGM_BASIC[] = {0x08, 0x00, 0x01, 0x02};
uint8_t  ADVERTISE[] = {0x00, 0x02, 0x06, 0x01, 0x02, 0x02};



int mrecv_bytes = 0;
int mxmit_bytes = 0;
uint8_t input_buffer[1024];



void USART1_RX_IRQHandler(void)
{
	USART_IntClear(USART1, USART_IF_TXC);
	USART_IntClear(USART1, USART_IF_RXDATAV);
}


void mxmitCbk( SPIDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, int transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	if ( transferStatus == ECODE_EMDRV_SPIDRV_OK )
	{
	// Success !
	}
	mxmit_bytes = transferCount;
}

void mrecvCbk(SPIDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, int transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	mrecv_bytes = transferCount;
}


int getMemoryDeviceID(void)
{
	GPIO_PinOutClear(MEM_CS_PORT, MEM_CS_PIN);
	SPIDRV_MTransmitB(SPI_handle, DEVICE_ID, 4);
	SPIDRV_MReceiveB(SPI_handle, input_buffer, 2);
	GPIO_PinOutSet(MEM_CS_PORT, MEM_CS_PIN);
	return input_buffer[0];
}



int getWGMHello(void)
{
	GPIO_PinOutClear(SPI_CS_PORT, SPI_CS_PIN);
	SPIDRV_MTransmitB(SPI_handle, WGM_HELLO, 4);
	SPIDRV_MReceiveB(SPI_handle, input_buffer, 4);
	GPIO_PinOutSet(SPI_CS_PORT, SPI_CS_PIN);
	return input_buffer[0];
}



