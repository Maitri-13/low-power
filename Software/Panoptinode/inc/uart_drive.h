/*
 * uart_drive.h
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */

#ifndef INC_UART_DRIVE_H_
#define INC_UART_DRIVE_H_

#include "../inc/led.h"
#include "../inc/main.h"

#include <stdlib.h>
#include "uartdrv.h"
#include "string.h"
#include "em_usart.h"

/* length of command messages */
#define UART_CMD_LEN 	6
#define UART_DBL_LEN 	12
#define PACK_SIZE 		512
#define DATA_SIZE 		(PACK_SIZE - UART_CMD_LEN)
#define MAX_IMAGE_SIZE  20240

extern UARTDRV_HandleData_t UART_handleData;
extern UARTDRV_Handle_t UART_handle;


void recvCbk(UARTDRV_Handle_t, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount);

void xmitCbk(UARTDRV_Handle_t, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount);

int drv_syncCam(void);

void drv_resetCam(void);

uint32_t drv_UART_SM(void);

uint8_t *getImage(uint32_t image_size);

uint8_t *all_in_one(void);

#endif /* INC_UART_DRIVE_H_ */
