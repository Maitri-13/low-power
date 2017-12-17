/*
 * setup.h
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#ifndef INC_SETUP_H_
#define INC_SETUP_H_


/* Init all peripherals*/
int	initPeripherals(void);

int setupACMP(void);


/* Setup ADC */
int setupADC(void);

/* Setup UART */
int setupUART(void);

/* Setup SPI */
int setupSPI(void);

/* Setup SPI */
int setupGPIO(void);

/* Setup SPI */
int setupLESENSE(void);

int syncCam(void);

void UART_SM(void);

void ACMP0_IRQHandler(void);


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


#endif /* INC_SETUP_H_ */
