/*
 * DemoApp_Wireless.c
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
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
	showMainScreen_Wireless();
	delay(500);
	for(;;) {
		if(GUI_Touch_Read_2046() != 0) return;
	}
}

void showMainScreen_Wireless(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f4/f4.bmp");
}
