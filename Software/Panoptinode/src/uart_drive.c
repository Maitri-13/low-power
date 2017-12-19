/*
 * uart_drive.c
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */

#include "../inc/uart_drive.h"

/* command constants */
uint8_t RESET_MSG[] = {0xAA, 0x08, 0x00, 0x00, 0x00, 0xFF};
uint8_t SYNC_MSG[]  = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
uint8_t ACK_MSG[]   = {0xAA, 0x0E, 0x0D, 0x00, 0x00, 0x00};

uint8_t MSG_ARRAY[3][6] = {{0xAA, 0x01, 0x00, 0x07, 0x07, 0x07},
						  {0xAA, 0x06, 0x08, 0x00, 0x02, 0x00},
						  {0xAA, 0x05, 0x00, 0x00, 0x00, 0x00}};

uint8_t GET_MSG[]  		= {0xAA, 0x04, 0x01, 0x00, 0x00, 0x00};
uint8_t PIC_ACK_MSG[]   = {0xAA, 0x0E, 0x00, 0x00, 0x00, 0x00};
uint8_t END_ACK_MSG[]   = {0xAA, 0x0E, 0x00, 0x00, 0xF0, 0xF0};

/* response constants */
uint8_t SYN_ACK_RSP[]	= {0xAA, 0x0E, 0x0D};
uint8_t SYN_RSP[]	    = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};

/* buffers for images */
uint8_t raw_buffer[PACK_SIZE];
uint8_t dbl_buffer[UART_DBL_LEN];
uint8_t cmd_buffer[UART_CMD_LEN];

/* flags to check if it hits the interrupt */
volatile uint8_t recv_bytes = 0;
extern volatile bool uart_flag;

/* Variable to store final image */
ImageData_t image_data;

/* UART IRQ Handler */
void USART0_RX_IRQHandler(void)
{
	;
}

/* UART Transmit Handler */
void xmitCbk(UARTDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	(void) transferCount;
}

/* UART Receive Handler */
void recvCbk(UARTDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	recv_bytes = transferCount;
}

/* SYNC Camera*/
int drv_syncCam(void)
{
	uint16_t test = 0;
	/* Run till you get sync , if it is stuck here do hardware reset */
	while(1)
	{
		UARTDRV_Receive(UART_handle,dbl_buffer,UART_DBL_LEN, recvCbk);

		/* loop until the ACK is received*/
		while(recv_bytes == 0)
		{
			UARTDRV_TransmitB(UART_handle,SYNC_MSG,UART_CMD_LEN);
			for(test=0;test<8701;test++); //5ms delay
		}

		/* ensure second received message is SYN */
		if ((dbl_buffer[0] != 0xAA ) || (dbl_buffer[2] != 0x0D))
		{
			UARTDRV_TransmitB(UART_handle,SYNC_MSG,UART_CMD_LEN);
		}
		else
		{
			break;
		}
	}
	UARTDRV_TransmitB(UART_handle,ACK_MSG,UART_CMD_LEN);
	return 0;
}

/* RESET Camera */
void drv_resetCam(void)
{
	UARTDRV_TransmitB(UART_handle,RESET_MSG,UART_CMD_LEN);

	/*Delay*/
	uart_flag = true;
	while(uart_flag);
}


/*	UART command sequence to get Picture */
void get_Picture(void)
{
	int num_cmds    	      = 3;
	uint32_t num_packages 	  = 0;
	uint16_t ack_id       	  = 0;
	uint16_t actual_pack_size = 0;
	uint32_t offset			  = 0;
	image_data.image_size    = 0;

	/* Synchronize the camera */
	drv_syncCam();

	for(int i=0; i<num_cmds; i++)
	{
		UARTDRV_TransmitB(UART_handle, MSG_ARRAY[i], UART_CMD_LEN);
		UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
		for(int j=0; j < 8701; j++); // 5ms delay
	}

	/*Delay*/
	uart_flag = true;
	while(uart_flag);

	UARTDRV_TransmitB(UART_handle, GET_MSG, UART_CMD_LEN);
	UARTDRV_ReceiveB(UART_handle, dbl_buffer, UART_DBL_LEN);

	/*Calculate image size*/
	image_data.image_size += (dbl_buffer[11] << 16)
						    + (dbl_buffer[10] << 8)
							+ (dbl_buffer[9]);

	/*Total number of packages to receive*/
	num_packages = image_data.image_size/DATA_SIZE;

	for(ack_id = 0; ack_id< num_packages; ack_id++)
	{

		/* Set ACK */
		PIC_ACK_MSG[4] = ack_id;

		/* get_pic */
		UARTDRV_TransmitB(UART_handle, PIC_ACK_MSG, UART_CMD_LEN);
		UARTDRV_ReceiveB(UART_handle, raw_buffer, PACK_SIZE);

		/* Get the packet size*/
		actual_pack_size = (raw_buffer[3] << 8) + raw_buffer[2];

		/* copy image data into data buffer */
		offset = DATA_SIZE * ack_id;
		for(int i=0;i<actual_pack_size;i++){
			image_data.data_buffer[offset+i] = raw_buffer[i+4];
		}
	}
	/* send the last ACK */
	PIC_ACK_MSG[4] = ack_id;
	UARTDRV_TransmitB(UART_handle, PIC_ACK_MSG, UART_CMD_LEN);

	/* Reset the camera */
	drv_resetCam();
}

