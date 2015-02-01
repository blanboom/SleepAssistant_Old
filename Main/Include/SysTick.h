/**
  ******************************************************************************
  * @file    SysTick.h
  * @author  Blanboom
  * @date    2015 - 2 - 1
  * @brief   SysTick 中断函数库的头文件
  ******************************************************************************
  */

#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void systickInit(void);
void delay(__IO u32 nTime);
void systickDelayISR(void);

#endif /* __SYSTICK_H */
