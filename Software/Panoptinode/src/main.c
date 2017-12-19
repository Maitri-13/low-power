/***************************************************************************//**
 * @file main.c
 * @brief init the main
 * @version 5.3.3
 * @author Chase E Stewart
 *******************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "hal-config.h"

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_acmp.h"
#include "bsp.h"

#include "../inc/main.h"


/**************************************************************************//**
 * @brief  delay for some part of time
 *****************************************************************************/
void delay(int number_of_sec)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_sec;
    int intermediate = 0;

    // Stroing start time
    clock_t start_time = clock();

    // looping till required time is not acheived
    while (intermediate < start_time + milli_seconds)
    {
    	intermediate = clock();
    }

    clock_t end_time = clock();
}



/**************************************************************************//**
 * @brief  init all periphs, go to sleep, then wake up and do main
 *****************************************************************************/
int main(void)
{
	uint32_t img_size;
	uint8_t device_id;

	/* Chip errata */
	CHIP_Init();

	/* init HFPER clock */
	CMU_ClockEnable(cmuClock_HFPER, true);

	led_clear();

	/* Initialize all peripherals */
	initPeripherals();

	while (1)
	{

		ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
		EMU_EnterEM2(true);
		//getMemoryDeviceID();
		getWGMHello();
		led_clear();
	}
}


