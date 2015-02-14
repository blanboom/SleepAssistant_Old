/** SysTick.h - SysTick 相关函数
 */

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void systickInit(void);
void delay(__IO u32 nTime);
void systickDelayISR(void);

#endif /* __SYSTICK_H */
