/** SysTick.c - SysTick 相关函数
 */

#include "systick.h"

static volatile uint32_t TimingDelay;
volatile uint32_t sysTime = 0;

/**
  * @brief  启动 SysTick
  * @param  None
  * @retval None
  */
void systickInit( void )
{
	  /*!< SystemFrequency / 1000    1ms中断一次
           SystemFrequency / 100000	 10us中断一次
           SystemFrequency / 1000000 1us中断一次
	     */
	if(SysTick_Config(SystemCoreClock / 1000))
	{
		/* Capture Error */
		while(1);
	}
}

/**
  * @brief  延时程序
  * @param  nTime 延时的时间，单位为 SysTick 的周期
  * @retval None
  */
void delay(__IO uint32_t nTime)
{
	TimingDelay = nTime;
	while(TimingDelay != 0);
}

/**
  * @brief  获取节拍程序
  * @note   在 SysTick 中断函数 SysTick_Handler()调用
  * @param  None
  * @retval None
  */
void systickDelayISR(void)
{
	sysTime++;
	if(TimingDelay != 0x00)
	{
		TimingDelay--;
	}
}
