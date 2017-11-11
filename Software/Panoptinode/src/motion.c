/***************************************************************************//**
 * @file
 * @brief Configuration of GPIO LESENSE for Motion Sensor
 * @version 1.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 ******************************************************************************/
#include "rtcdriver.h"
#include "graphics.h"
#include "bspconfig.h"
#include "../includes/main.h"

/***************************************************************************//**
 * @brief Setup GPIO input for Motion Detection
 ******************************************************************************/
void GpioSetup(void)
{
  /* Enable GPIO clock. */
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PD8 as input and enable interrupt. */
  GPIO_PinModeSet(gpioPortD, 8, gpioModeInput, 1);
  GPIO_IntConfig(gpioPortD, 8, true, false, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/***************************************************************************//**
 * @brief Setup GPIO interrupt
 ******************************************************************************/
void GPIO_IRQ(void)
{
  /* Get and clear all pending GPIO interrupts */
  uint32_t interruptMask = GPIO_IntGet();
  GPIO_IntClear(interruptMask);

  /* Act on interrupts */
  if (interruptMask & (1 << 8)) { /*8==> Pin 8 of port D*/
    /* Display on GLCD */
	GRAPHICS_DisplayData(interruptMask);
  }
}

/***************************************************************************//**
 * @brief GPIO Interrupt handler for even pins
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IRQ();
}
