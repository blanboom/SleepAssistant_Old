/*
 * 演示程序：无线连接
 */

#include "DemoApp_Wireless.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"

void showMainScreen_Wireless(void);

void DemoApp_Wireless(void) {
	Coordinate* pTouchPosition;
	showMainScreen_Wireless();
	delay(500);
	for(;;) {
		if(1 == touch_flag) {
			pTouchPosition = GUI_Touch_Read_2046();
			if(pTouchPosition->x < 1032 && pTouchPosition->y < 1032) break;
		}
	}
}

void showMainScreen_Wireless(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f4/f4_1.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f4/f4_2.bmp");
}
