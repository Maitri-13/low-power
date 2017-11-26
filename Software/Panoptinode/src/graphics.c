/***************************************************************************//**
 * @brief Draws the graphics on the display
 * @version 5.3.3
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "graphics.h"
#include "em_types.h"
#include "glib.h"
#include "dmd.h"
#include "display.h"
#include "textdisplay.h"
#include "retargettextdisplay.h"
#include <string.h>
#include <stdio.h>

static GLIB_Context_t glibContext;          /* Global glib context */

/***************************************************************************//**
 * @brief Initializes the graphics stack.
 * @note This function will /hang/ if errors occur (usually
 *       caused by faulty displays.
 ******************************************************************************/
void GRAPHICS_Init(void)
{
  EMSTATUS status;

  /* Initialize the display module. */
  status = DISPLAY_Init();
  if (DISPLAY_EMSTATUS_OK != status) {
    while (1)
      ;
  }

  /* Initialize the DMD module for the DISPLAY device driver. */
  status = DMD_init(0);
  if (DMD_OK != status) {
    while (1)
      ;
  }

  status = GLIB_contextInit(&glibContext);
  if (GLIB_OK != status) {
    while (1)
      ;
  }

  glibContext.backgroundColor = Black;
  glibContext.foregroundColor = White;

  /* Use Narrow font */
  GLIB_setFont(&glibContext, (GLIB_Font_t *)&GLIB_FontNarrow6x8);
}

/***************************************************************************//**
 * @brief This function prints the temperature on the display
 * @param temp
 *        The temperature in celsius
 ******************************************************************************/
void GRAPHICS_DisplayData(float val)
{
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "%f V", val);
  GLIB_clear(&glibContext);
  GLIB_drawString(&glibContext, "ADC val: ", 32, 5, 5, 0);
  GLIB_drawString(&glibContext, buffer, 32, 5, 15, 0);
  if (val == -100.0) {
    GLIB_drawString(&glibContext, "ADC is not calibrated.", 32, 5, 65, 0);
  }
  DMD_updateDisplay();
}
