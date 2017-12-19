/*
 * led.c
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */


#include "em_gpio.h"
#include "hal-config.h"
#include "../inc/main.h"

extern int led_count;
/* set the led */
void led_set(void)
{
	GPIO_PinOutSet(LED_PORT, LED_PIN);
}

/* clear the led */
void led_clear(void)
{
	GPIO_PinOutClear(LED_PORT, LED_PIN);
}

/* toggle the led */
void led_toggle(void)
{
	if (led_count%2)
	{
		led_set();
	}
	else
	{
		led_clear();
	}
}
