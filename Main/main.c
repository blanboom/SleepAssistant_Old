/** 睡眠助手
 * 一个多功能的睡眠辅助设备，包含闹钟、睡眠提醒、睡眠质量统计等
 * 目前的程序具有基本的演示功能
 */

#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "TimeAlarm.h"
#include "interrupt.h"
#include "SysTick.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "Switches.h"
#include "USART1.h"
#include "DemoApp_SleepTrack.h"
#include "DemoApp_HeartRate.h"
#include "DemoApp_Alarm.h"
#include "DemoApp_Wireless.h"
#include "DemoApp_Music.h"
#include "DemoApp_About.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int main(int argc, char* argv[]) {
	FATFS filesystem;

	/* 初始化 */
	trace_printf("System clock: %uHz\n", SystemCoreClock);
	interruptNVICInit();
	timeInit();
	systickInit();
	GUI_Init();
	USART1_Init();
	Switches_Init();
	f_mount(0, &filesystem);
	trace_printf(asctime(&currentTime));

	/* 屏幕校准，显示主界面 */
	GUI_Main_Start();
	GUI_Main_MenuScreen();

	/* 菜单 */
	for(;;) {
		switch(GUI_Main_MenuScreenCheckTouch()) {
		case 1:
			DemoApp_SleepTrack();
			GUI_Main_MenuScreen();
			break;
		case 2:
			DemoApp_HeartRate();
			GUI_Main_MenuScreen();
			break;
		case 3:
			DemoApp_Alarm();
			GUI_Main_MenuScreen();
			break;
		case 4:
			DemoApp_Wireless();
			GUI_Main_MenuScreen();
			break;
		case 5:
			DemoApp_Music();
			GUI_Main_MenuScreen();
			break;
		case 6:
			DemoApp_About();
			GUI_Main_MenuScreen();
			break;
		default:
			break;
		}
	}
}

#pragma GCC diagnostic pop
