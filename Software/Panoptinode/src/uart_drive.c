/*
 * uart_drive.c
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */

#include "../inc/uart_drive.h"


/* command consts */
uint8_t RESET_MSG[] = {0xAA, 0x08, 0x00, 0x00, 0x00, 0xFF};
uint8_t SYNC_MSG[]  = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
uint8_t ACK_MSG[]   = {0xAA, 0x0E, 0x0D, 0x00, 0x00, 0x00};


//uint8_t INIT_MSG[]  = {0xAA, 0x01, 0x00, 0x07, 0x07, 0x07};
//uint8_t PACK_MSG[]  = {0xAA, 0x06, 0x08, 0x00, 0x00, 0x00};
//uint8_t SNAP_MSG[]  = {0xAA, 0x05, 0x00, 0x00, 0x00, 0x00};
//uint8_t GET_MSG[]   = {0xAA, 0x04, 0x01, 0x00, 0x00, 0x00};

uint8_t MSG_ARRAY[3][6] = {{0xAA, 0x01, 0x00, 0x07, 0x07, 0x07},
						  {0xAA, 0x06, 0x08, 0x00, 0x00, 0x00},
						  {0xAA, 0x05, 0x00, 0x00, 0x00, 0x00}};
uint8_t GET_MSG[]  		= {0xAA, 0x04, 0x01, 0x00, 0x00, 0x00};
uint8_t PIC_ACK_MSG[]   = {0xAA, 0x0E, 0x00, 0x00, 0x00, 0x00};

/* response consts */
uint8_t SYN_ACK_RSP[]	= {0xAA, 0x0E, 0x0D};
uint8_t SYN_RSP[]	    = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};

/* buffers for images */
uint8_t raw_buffer[PACK_SIZE];
uint8_t dbl_buffer[UART_DBL_LEN];
uint8_t cmd_buffer[UART_CMD_LEN];
volatile uint8_t recv_bytes = 0;

void USART0_RX_IRQHandler(void)
{
	;
}



void xmitCbk( UARTDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	(void) transferCount;
}

void recvCbk(UARTDRV_Handle_t UART_handle, Ecode_t transferStatus, uint8_t *data, UARTDRV_Count_t transferCount)
{
	(void) UART_handle;
	(void) transferStatus;
	(void) data;
	recv_bytes = transferCount;
}



int drv_syncCam(void)
{
	uint8_t count  = 0;
	uint16_t test = 0;

	UARTDRV_Receive(UART_handle,dbl_buffer,UART_DBL_LEN, recvCbk);

	/* loop until the ACK msg is received*/
	while(recv_bytes == 0)
	{
 		UARTDRV_TransmitB(UART_handle,SYNC_MSG,UART_CMD_LEN);
		count++;
		for(test=0;test<8701;test++);
	}

	/* ensure second received message is SYN */
	if ((dbl_buffer[0] != 0xAA ) || (dbl_buffer[2] != 0x0D))
	{
		while(1);
	}

	UARTDRV_TransmitB(UART_handle,ACK_MSG,UART_CMD_LEN);
	return 0;
}

void drv_resetCam(void)
{
	UARTDRV_TransmitB(UART_handle,RESET_MSG,UART_CMD_LEN);
}


uint8_t *all_in_one(void)
{
	int num_cmds    	      = 3;
	uint32_t image_size   	  = 0;
	uint32_t num_packages 	  = 0;
	uint16_t ack_id       	  = 0;
	uint16_t actual_pack_size = 0;
	uint32_t offset			  = 0;

	for(int i=0; i<num_cmds; i++)
	{
		UARTDRV_TransmitB(UART_handle, MSG_ARRAY[i], UART_CMD_LEN);
		UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
		for(int j=0; j < 8701; j++);

	}

	for(uint16_t j=0; j < 65535; j++)
	{
		for(uint8_t k; k<30;k++);
	}

	UARTDRV_TransmitB(UART_handle, GET_MSG, UART_CMD_LEN);
	UARTDRV_Receive(UART_handle, dbl_buffer, UART_DBL_LEN, recvCbk);

	for(uint16_t j=0; j < 65535; j++)
	{
		for(uint8_t k; k<30;k++);
	}

	image_size |= (dbl_buffer[9] << 16);
	image_size |= (dbl_buffer[10] << 8);
	image_size |= dbl_buffer[11];

	num_packages = image_size/DATA_SIZE;
	//uint8_t data_buffer[image_size];

	for(ack_id = 0; ack_id< num_packages; ack_id++){


		/* Set ACK msg*/
		PIC_ACK_MSG[4]=*((uint8_t*)&(ack_id)+1);
		PIC_ACK_MSG[5]=*((uint8_t*)&(ack_id)+0);

		/* get_pic */
		UARTDRV_TransmitB(UART_handle, PIC_ACK_MSG, UART_CMD_LEN);
		UARTDRV_ReceiveB(UART_handle, raw_buffer, PACK_SIZE);


		actual_pack_size = (raw_buffer[2] << 8) + raw_buffer[3];

		/* copy image data into data buffer */
		offset = DATA_SIZE * ack_id;
		//for(int i=0;i<actual_pack_size;i++){
		//	data_buffer[offset+i] = raw_buffer[i+4];
		//}
	}
	return raw_buffer;
}


//uint32_t drv_UART_SM(void)
//{
//	int test = 0;
//	uint16_t num_packages = 0;
//	uint32_t image_size = 0;
//	uint16_t ack_id = 0;
//	uint16_t actual_pack_size = 0;
//	uint32_t offset = 0;
//	/* TODO wait for data */
//
//	/* init */
//	UARTDRV_TransmitB(UART_handle, INIT_MSG, UART_CMD_LEN);
//	UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
//
//	for(test=0;test<8701;test++);
//
//	/* package_size */
//	//UARTDRV_TransmitB(UART_handle, PACK_MSG, UART_CMD_LEN);
//	//UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
//
//	//for(test=0;test<8701;test++);
//
//	/* snapshot */
//	UARTDRV_TransmitB(UART_handle, SNAP_MSG, UART_CMD_LEN);
//	UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
//
//	for(test=0;test<8701;test++);
//
//	return 0;
//}
//
//
//uint8_t *getImage(uint32_t image_size){
//	uint16_t num_packages;
//	uint16_t ack_id;
//	uint16_t actual_pack_size;
//	uint32_t offset;
//
//	//UARTDRV_TransmitB(UART_handle, GET_MSG, UART_CMD_LEN);
//	//UARTDRV_ReceiveB(UART_handle, dbl_buffer, UART_DBL_LEN);
//
//	image_size |= (dbl_buffer[9] << 16);
//	image_size |= (dbl_buffer[10] << 8);
//	image_size |= dbl_buffer[11];
//
//	// calculate number of packages
//	num_packages = image_size/DATA_SIZE;
//	uint8_t data_buffer[image_size];
//
//	num_packages = 1000;
//
//	for(ack_id = 0; ack_id< num_packages; ack_id++){
//
//
//		/* Set ACK msg*/
//		PIC_ACK_MSG[4]=*((uint8_t*)&(ack_id)+1);
//		PIC_ACK_MSG[5]=*((uint8_t*)&(ack_id)+0);
//
//		/* get_pic */
//		UARTDRV_TransmitB(UART_handle, PACK_MSG, UART_CMD_LEN);
//		UARTDRV_ReceiveB(UART_handle, cmd_buffer, UART_CMD_LEN);
//
//
//		actual_pack_size = (raw_buffer[2] << 8) + raw_buffer[3];
//
//		/* copy image data into data buffer */
//		offset = DATA_SIZE * ack_id;
//		for(int i=0;i<actual_pack_size;i++){
//			data_buffer[offset+i] = raw_buffer[i+4];
//		}
//	}
//
//
//	return data_buffer;
//}
