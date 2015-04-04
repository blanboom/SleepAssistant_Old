/** interrput.c - 中断相关程序
 * 包含中断服务程序和中断初始化函数
 */

#include "interrupt.h"
#include "stm32f10x_conf.h"
#include "TimeAlarm.h"
#include "SysTick.h"
#include "USART1.h"
#include "XPT2046.h"

/** void interruptNVICInit(void) - 中断初始化程序
 * 初始化 NVIC，设置中断优先级
 */
void interruptNVICInit(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	// 设置 RTC 中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// 设置 USART1 中断优先级
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	// 设置 EXTI9_5 中断优先级（触摸屏的触摸中断）
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
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

void SysTick_Handler(void)
{
	systickDelayISR();
}

void USART1_IRQHandler(void)
{
	USART1_ISR();
}

void EXTI9_5_IRQHandler(void)
{
	XPT2046_ISR();
}
