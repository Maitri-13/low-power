/***************************************************************************//**
 * @file
 * @brief
 * @version 1.0
 *******************************************************************************/

#ifndef INCLUDES_MIC_H_
#define INCLUDES_MIC_H_

void periodicUpdateCallback(RTCDRV_TimerID_t id, void *user);
void memLcdCallback(RTCDRV_TimerID_t id, void *user);
void AdcSetup(void);
double convertADCtoVolt();

#endif /* INCLUDES_MIC_H_ */
