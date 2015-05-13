/*
 * AnalogSensors.h
 *
 *  Created on: 2015年5月12日
 *      Author: maxz
 */

#ifndef ANALOGSENSORS_ANALOGSENSORS_H_
#define ANALOGSENSORS_ANALOGSENSORS_H_

#include "stm32f10x_conf.h"

extern volatile uint16_t AnalogSensors_ADCValue;

void AnalogSensors_Light_Init(void);
void AnalogSensors_HeartRate_Init(void);

#endif /* ANALOGSENSORS_ANALOGSENSORS_H_ */
