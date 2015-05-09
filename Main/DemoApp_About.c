/*
 * DemoApp_About.c
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
 */

#include "DemoApp_About.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"

void showMainScreen_About(void);

void DemoApp_About(void) {
	showMainScreen_About();
	delay(500);
	for(;;) {
		if(GUI_Touch_Read_2046() != 0) return;
	}
}

void showMainScreen_About(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f6/f6.bmp");
}


