/*
 * interrupt.c
 *
 * 中断相关函数
 */

#include "interrupt.h"
#include "stm32f10x_conf.h"
#include "TimeAlarm.h"

void interruptNVICInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// 设置 RTC 中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void RTC_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{
		timeAlarmUpdate();
		RTC_ClearITPendingBit(RTC_IT_SEC);
		RTC_WaitForLastTask();
	}
}
