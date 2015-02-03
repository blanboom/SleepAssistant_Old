/*
 * RF-BM-S02.h
 *
 *  Created on: 2014年11月23日
 *      Author: maxz
 */

#ifndef USART1_H_
#define USART1_H_
#include "stm32f10x_conf.h"

void USART1_Init(void);
int USART1_Printf(const char* format, ...);
void USART1_ISR(void);

extern uint8_t USART1_ReadBuffer[1];

#endif /* RF_BM_S02_RF_BM_S02_H_ */
