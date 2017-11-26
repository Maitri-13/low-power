/***************************************************************************//**
 * @file
 * @brief Configuration of ADC for MIC_OUT
 * @version 1.0
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 ******************************************************************************/
#include "../includes/main.h"

/***************************************************************************//**
 * Local variables
 ******************************************************************************/
/* RTC callback parameters. */
static void (*rtcCallback)(void*) = NULL;
static void * rtcCallbackArg = 0;
volatile uint32_t rtcCallbacks = 0;

/** This flag tracks if we need to update the display. */
extern volatile bool updateDisplay;

/***************************************************************************//**
 * @brief Initialize ADC for MIC readings in single point
 ******************************************************************************/
void AdcSetup(void)
{
  /* Enable ADC clock */
  CMU_ClockEnable(cmuClock_ADC0, true);

  /* Base the ADC configuration on the default setup. */
  ADC_Init_TypeDef       init  = ADC_INIT_DEFAULT;
  ADC_InitSingle_TypeDef sInit = ADC_INITSINGLE_DEFAULT;

  /* Initialize timebases */
  init.timebase = ADC_TimebaseCalc(0);
  init.prescale = ADC_PrescaleCalc(400000, 0);
  ADC_Init(ADC0, &init);

  /* Set input to temperature sensor. Reference must be 1.25V */
  sInit.reference   = adcRef5V;
  sInit.acqTime     = adcAcqTime8; /* Minimum time for temperature sensor */
  sInit.posSel      = adcPosSelAPORT3XCH2; /*PD10 ADC Pin*/
  ADC_InitSingle(ADC0, &sInit);
}

/***************************************************************************//**
 * @brief  Do one ADC conversion
 * @return ADC conversion result
 ******************************************************************************/
static uint32_t AdcRead(void)
{
  ADC_Start(ADC0, adcStartSingle);
  while ( (ADC0->STATUS & ADC_STATUS_SINGLEDV) == 0 ) {
  }
  return ADC_DataSingleGet(ADC0);
}

/***************************************************************************//**************
 * @brief Convert ADC sample values to volts.
 * @detail Calibration from
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 *
 * @return Volts.
 *******************************************************************************************/
double convertADCtoVolt()
{
	uint32_t signalMax = 0;
	uint32_t signalMin = 1024;
	for (int i = 0; i< 500; i++)
	{
		int32_t sample = AdcRead();
		if (sample < 1024)  // toss out spurious readings
		{
			if (sample > signalMax)
			{
				signalMax = sample;  // save just the max levels
			}
			else if (sample < signalMin)
			{
				signalMin = sample;  // save just the min levels
			}
		}
	 }
	 uint32_t peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
	 double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
	 return volts;
}

/***************************************************************************//**
 * @brief   The actual callback for Memory LCD toggling
 * @param[in] id
 *   The id of the RTC timer (not used)
 ******************************************************************************/
void memLcdCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  rtcCallback(rtcCallbackArg);

  rtcCallbacks++;
}

/***************************************************************************//**
 * @brief   Register a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  0 for successful or
 *         -1 if the requested frequency does not match the RTC frequency.
 ******************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  RTCDRV_TimerID_t timerId;
  rtcCallback    = pFunction;
  rtcCallbackArg = argument;

  RTCDRV_AllocateTimer(&timerId);

  RTCDRV_StartTimer(timerId, rtcdrvTimerTypePeriodic, 1000 / frequency,
                    memLcdCallback, NULL);

  return 0;
}

/***************************************************************************//**
 * @brief Callback used to count between measurement updates
 ******************************************************************************/
void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user)
{
  (void) id;
  (void) user;
  updateDisplay = true;
}
