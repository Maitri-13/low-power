/*
 * main.h
 *
 *  Created on: Dec 15, 2017
 *      Author: cstewart
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "../inc/setup.h"
#include "../inc/adc.h"
#include "../inc/uart_drive.h"
#include "../inc/led.h"


/* state vars */
uint16_t major_state;
uint16_t minor_state;


typedef enum
{
	WAKEUP = 0,
	IMAG_START,
	IMAG_COMPLETE,
	NOTIF_SEND,
	AUD_BEGIN,
	AUD_COMPLETE,
	SHDWN
} major_state_enum;


/* main function */
int main(void);

/* delay for a certain length of time */
void delay(int number_of_sec);



#endif /* INC_MAIN_H_ */
