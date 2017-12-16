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

/* Setup SPI */
int setupLESENSE(void);

int syncuCam(void);


#endif /* INC_SETUP_H_ */
