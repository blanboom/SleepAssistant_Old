/*
 * gui_main.c
 *
 *  Created on: 2015年3月17日
 *      Author: maxz
 */

#include "gui_basic.h"
#include "ili9320.h"
#include "SysTick.h"
#include "gui_main.h"

#ifndef GUI_GUI_MAIN_C_
#define GUI_GUI_MAIN_C_

uint8_t graphX, graphY, graphWidth, graphHeight, graphRight, graphNowX;
uint16_t graphBG;

// x, y 为图标左下角坐标
void GUI_Main_DrawGraph_Prepare(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t background) {
	graphX      = x;
	graphY      = y;
	graphNowX   = x;
	graphRight  = x + width;
	graphWidth  = width;
	graphHeight = height;
	graphBG     = background;
	GUI_Rectangle(x, y - height, x + width, y, background, background);
}

void GUI_Main_DrawGraph(uint8_t data, uint16_t color) {
	static previousData;
	GUI_Line(graphNowX+2, graphY, graphNowX+2, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX+3, graphY, graphNowX+3, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX+4, graphY, graphNowX+4, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX, graphY+previousData, graphNowX+1, graphY+data, color);
	if(graphNowX >= graphRight) {
		graphNowX = graphX;
	}
	previousData = data;
	graphNowX++;
}

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

void GUI_Main_F1_NoteScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/nf1.bmp");
}

void GUI_Main_F1_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/f1.bmp");
}

void GUI_Main_F2_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f2/f2.bmp");
}

void GUI_Main_F3_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f3/f3.bmp");
}

void GUI_Main_F4_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f4/f4.bmp");
}

void GUI_Main_F5_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f5/f5.bmp");
}

void GUI_Main_F6_MainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f6/f6.bmp");
}

#endif /* GUI_GUI_MAIN_C_ */
