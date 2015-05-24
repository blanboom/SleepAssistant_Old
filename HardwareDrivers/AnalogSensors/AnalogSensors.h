/*
 * 心率与光照传感器的读取程序
 */

#ifndef ANALOGSENSORS_ANALOGSENSORS_H_
#define ANALOGSENSORS_ANALOGSENSORS_H_

#include "stm32f10x_conf.h"

extern volatile uint16_t AnalogSensors_ADCValue; // 以 DMA 方式将 ADC 的值放在此变量里

void AnalogSensors_Light_Init(void);
void AnalogSensors_HeartRate_Init(void);
void AnalogSensors_Light_DeInit(void);
void AnalogSensors_HeartRate_DeInit(void);

#endif /* ANALOGSENSORS_ANALOGSENSORS_H_ */
