/*
 * uart.h
 *
 *  Created on: Dec 17, 2017
 *      Author: ashah
 */

#ifndef INC_UART_H_
#define INC_UART_H_

typedef enum _UART_STATE{
	RESET = 0,
	SYNC,
	INIT_JPEG,
	SET_PKG_SIZE,
	SNAPSHOT,
	GET_PICTURE_CMD,
	GET_PICTURE,
	EXIT
}UART_STATE;

int UART_ReadChar(void);
static inline void UART_WriteChar(char c);
int UART_WriteBuff(uint8_t *pCmdBuffer,int len);
int UART_VerifyBuff(uint8_t *pverifyBuff,int len);
int syncCam(void);
void resetCam(void);
void UART_SM(void);

#endif /* INC_UART_H_ */
