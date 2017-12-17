/***************************************************************************//**
 * @file main.c
 * @brief init the main
 * @version 5.3.3
 * @author Chase E Stewart
 *******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "em_device.h"
#include "em_chip.h"

#include "hal-config.h"

#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"

#include "../inc/main.h"
#include "../inc/setup.h"
#include "../inc/adc.h"
#include "../inc/uart.h"

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (clock() < start_time + milli_seconds);
}

void LedToggle()
{
	GPIO_PinOutSet(gpioPortD, 14);
	delay(1);
	GPIO_PinOutClear(gpioPortD, 14);
}

int main(void)
{
	double getAdcVolt;

	/* Chip errata */
	CHIP_Init();

	/* init HFPER clock */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Initialize all peripherals */
	initPeripherals();

	/* set LED when ADC noise exceeds threshold */
	while (1) {
		/*
		getAdcVolt = convertADCtoVolt();
		if (getAdcVolt > 0.018)
		{
			GPIO_PinOutSet(gpioPortD, 14);
		}
		else
		{
			GPIO_PinOutClear(gpioPortD, 14);
		}*/
	}
}
