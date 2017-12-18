/*
 * led.c
 *
 *  Created on: Dec 17, 2017
 *      Author: cstewart
 */


#include "em_gpio.h"
#include "hal-config.h"
#include "../inc/main.h"


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
	if ((GPIO_PinOutGet(LED_PORT, LED_PIN)) == 0)
	{
		GPIO_PinOutSet(LED_PORT, LED_PIN);
	}
	else
	{
		GPIO_PinOutClear(LED_PORT, LED_PIN);
	}
}
