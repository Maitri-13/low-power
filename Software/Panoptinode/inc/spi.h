/*
 * spi.h
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "spidrv.h"

extern SPIDRV_HandleData_t SPI_handleData;
extern SPIDRV_Handle_t SPI_handle;

extern volatile uint8_t wait_to_recv;

void mxmitCbk( SPIDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, int transferCount);
void mrecvCbk( SPIDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, int transferCount);

int getMemoryDeviceID(void);

int getWGMHello(void);


#endif /* INC_SPI_H_ */
