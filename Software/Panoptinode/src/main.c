/***************************************************************************//**
 * @file main.c
 * @brief init the main
 * @version 5.3.3
 * @author Chase E Stewart
 *******************************************************************************/


#include <stdint.h>
#include <stdbool.h>

#include "em_device.h"
#include "em_chip.h"

#include "hal-config.h"

#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"

#include "../inc/main.h"
#include "../inc/setup.h"
#include "../inc/adc.h"

int main(void)
{
	double getAdcVolt;

	/* Chip errata */
	CHIP_Init();

	/* init HFPER clock */
	CMU_ClockEnable(cmuClock_HFPER, true);

	/* Initialize all peripherals */
	initPeripherals();

	//BSP_LedClear(1);

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
