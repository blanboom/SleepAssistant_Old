/*
 * gui_main.c
 *
 *  Created on: 2015年3月17日
 *      Author: maxz
 */

#include "gui_basic.h"
#include "ili9320.h"
#include "SysTick.h"

#ifndef GUI_GUI_MAIN_C_
#define GUI_GUI_MAIN_C_

void GUI_Main_StartScreen(void) {
	ili9320_Clear(0xc73c);
	GUI_DisplayBMP(85, 20, "/GUI/welcome.bmp");
}

void GUI_Main_NoteScreen(void) {
	ili9320_Clear(White);
	//GUI_DisplayBMP(0, 0, "/GUI/note.bmp");
	GUI_Rectangle(0, 0, 320, 41, 0x001f, 1);
	GUI_Rectangle(0, 233, 320, 240, 0x001f, 1);
	GUI_DisplayBMP_SkipBackground(0, 0, "/GUI/note1.bmp", 0x001f);
	GUI_DisplayBMP_SkipBackground(16, 62, "/GUI/note2.bmp", 0xffff);
	GUI_DisplayBMP_SkipBackground(212, 62, "/GUI/note3.bmp", 0xffff);
	//GUI_Rectangle(0, 233, 320, 240, 0x001f, 1);
}

void GUI_Main_MainScreen(void) {
	ili9320_Clear(Black);
	GUI_DisplayBMP(6,   18,  "/GUI/main1.bmp");
	GUI_DisplayBMP(110, 18,  "/GUI/main2.bmp");
	GUI_DisplayBMP(214, 18,  "/GUI/main3.bmp");
	GUI_DisplayBMP(6,   123, "/GUI/main4.bmp");
	GUI_DisplayBMP(110, 123, "/GUI/main5.bmp");
	GUI_DisplayBMP(214, 123, "/GUI/main6.bmp");
}

#endif /* GUI_GUI_MAIN_C_ */
