/***************************************************************************//**
 * @file
 * @brief 			M A I N . C       F I L E
 * @version 1.0
 *******************************************************************************
 ******************************************************************************/

#include "../includes/main.h"

/***************************************************************************//**
 * Local defines
 ******************************************************************************/

/** Time (in ms) between periodic updates of the measurements. */
#define PERIODIC_UPDATE_MS      1000

volatile bool updateDisplay = true;

/** Timer used for periodic update of the measurements. */
RTCDRV_TimerID_t periodicUpdateTimerId;

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  //EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_STK_DEFAULT;
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_STK_DEFAULT;
  float getAdcVolt;

  /* Chip errata */
  CHIP_Init();

  /* Init HFXO with WSTK radio board specific parameters */
  CMU_HFXOInit(&hfxoInit);

  /* Switch HFCLK to HFXO and disable HFRCO */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Setup ADC */
  AdcSetup();

  /*Setup GPI0*/
  GpioSetup();

  RTCDRV_Init();
  GRAPHICS_Init();

  /* Set up periodic update of the display. */
  RTCDRV_AllocateTimer(&periodicUpdateTimerId);
  RTCDRV_StartTimer(periodicUpdateTimerId, rtcdrvTimerTypePeriodic,
                    PERIODIC_UPDATE_MS, periodicUpdateCallback, NULL);

  updateDisplay = true;

  while (true) {
    if (updateDisplay) {
      updateDisplay = false;
      getAdcVolt = convertADCtoVolt();
      GRAPHICS_DisplayData(getAdcVolt);
    }
    EMU_EnterEM2(false);
  }
}
