/** USART1.h - USART1 相关程序
 */

#ifndef USART1_H_
#define USART1_H_

#include "stm32f10x_conf.h"

#define USART1_BUFFER_LENTH 20

void USART1_Init(void);
int USART1_Printf(const char* format, ...);
void USART1_ISR(void);

extern uint8_t USART1_ReadBuffer[USART1_BUFFER_LENTH];
extern volatile uint8_t USART1_ReadBufferPointer;

#endif /* RF_BM_S02_RF_BM_S02_H_ */
