/** 睡眠助手
 * 一个多功能的睡眠辅助设备，包含闹钟、睡眠提醒、睡眠质量统计等
 */

#include <gui_basic.h>
#include <stdio.h>
#include "diag/Trace.h"
#include "Motion.h"
#include "TimeAlarm.h"
#include "interrupt.h"
#include "SysTick.h"
#include "MP3Play.h"
#include "USART1.h"
#include "App_Alarm.h"
#include "gui_touch.h"


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[])
{
	trace_printf("System clock: %uHz\n", SystemCoreClock);
	interruptNVICInit();  // NVIC 初始化
	timeInit();           // RTC 及时间初始化
	//motionInit();         // 动作感应（MPU6050 等）初始化
	systickInit();        // SysTick 初始化
	//mp3Init();
	USART1_Init();
	touchInit();
	GUI_Init();


	//getZeroMotionValue();  // 可在以后启动时增加校准程序
	trace_printf(asctime(&currentTime)); // 现在时间
	GUI_Chinese_Text(85,60,"\xc3\xab",2,Red,White);
	GUI_Chinese_Text(85,90,"\xba\xc3",2,Blue,White);
	GUI_Text(85,120,"X: ",3,Blue,White);
	GUI_Text(85,150,"Y: ",3,Blue,White);

	while (1)
	{
		GUI_Word(150,120,5,getTouchX(),0,White,Red);
		GUI_Word(150,150,5,getTouchY(),0,White,Red);
		//alarmApp();
        //trace_printf("%d\n", detectMove());  // 身体移动
		//delay(400);
	}
}

#pragma GCC diagnostic pop
