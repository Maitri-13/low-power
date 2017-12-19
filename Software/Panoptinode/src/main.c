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
 * @brief  init all periphs, go to sleep, then wake up and do main
 *****************************************************************************/
int main(void)
{
	uint32_t img_size;
	uint8_t device_id;

	/* Chip errata */
	CHIP_Init();

	/* Init HFPER clock */
	CMU_ClockEnable(cmuClock_HFPER,true);

	led_clear();

	/* Initialize all peripherals */
	initPeripherals();

	/* Run your functions */
	while (1)
	{

		ACMP_IntEnable(ACMP0, ACMP_IEN_EDGE);
		EMU_EnterEM2(true);
		//getMemoryDeviceID();
		get_Picture();
		led_clear();
	}
}


