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
#include "gui_touch.h"

#ifndef GUI_GUI_MAIN_C_
#define GUI_GUI_MAIN_C_

uint8_t graphX, graphY, graphWidth, graphHeight, graphRight, graphNowX;
uint16_t graphBG;

// x, y 为图标左上角坐标
// 绘制折线图
void GUI_Main_DrawGraph_Prepare(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t background) {
	graphX      = x;
	graphY      = y;
	graphNowX   = x;
	graphRight  = x + width;
	graphWidth  = width;
	graphHeight = height;
	graphBG     = background;
	GUI_Rectangle(x, y, x + width, y + height, background, background);
}

void GUI_Main_DrawGraph(uint8_t data, uint16_t color) {
	static int previousData;
	data = graphHeight - data;
	GUI_Line(graphNowX+2, graphY, graphNowX+2, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX+3, graphY, graphNowX+3, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX+4, graphY, graphNowX+4, graphY+graphHeight, graphBG);
	GUI_Line(graphNowX, graphY+previousData, graphNowX+1, graphY+data, color);
	if(graphNowX >= graphRight) {
		graphNowX = graphX;
		GUI_Line(graphNowX+0, graphY, graphNowX+0, graphY+graphHeight, graphBG);
		GUI_Line(graphNowX+1, graphY, graphNowX+1, graphY+graphHeight, graphBG);
		GUI_Line(graphNowX+2, graphY, graphNowX+2, graphY+graphHeight, graphBG);
	}
	previousData = data;
	graphNowX++;
}

void GUI_Main_Start(void) {
	GUI_Touch_Init();
	while(GUI_Touch_Calibrate() != 0);
	/********* Start *********/
	ili9320_Clear(0xc73c);
	GUI_DisplayBMP(85, 20, "/GUI/welcome.bmp");
	while (GUI_Touch_Read_2046() == 0);
	/********* Note *********/
	ili9320_Clear(White);
	//GUI_DisplayBMP(0, 0, "/GUI/note.bmp");
	GUI_Rectangle(0, 0, 320, 41, 0x001f, 1);
	GUI_Rectangle(0, 233, 320, 240, 0x001f, 1);
	GUI_DisplayBMP_SkipBackground(0, 0, "/GUI/note1.bmp", 0x001f);
	GUI_DisplayBMP_SkipBackground(16, 62, "/GUI/note2.bmp", 0xffff);
	GUI_DisplayBMP_SkipBackground(212, 62, "/GUI/note3.bmp", 0xffff);
	//GUI_Rectangle(0, 233, 320, 240, 0x001f, 1);
	while (GUI_Touch_Read_2046() == 0);
}

void GUI_Main_MenuScreen(void) {
	ili9320_Clear(Black);
	GUI_DisplayBMP(6,   18,  "/GUI/main1.bmp");
	GUI_DisplayBMP(110, 18,  "/GUI/main2.bmp");
	GUI_DisplayBMP(214, 18,  "/GUI/main3.bmp");
	GUI_DisplayBMP(6,   123, "/GUI/main4.bmp");
	GUI_DisplayBMP(110, 123, "/GUI/main5.bmp");
	GUI_DisplayBMP(214, 123, "/GUI/main6.bmp");
}

uint8_t GUI_Main_MenuScreenCheckTouch(void) {
	Coordinate* pTouchPosition;
	uint16_t x, y;
	pTouchPosition = GUI_Touch_Read_2046();
	if(pTouchPosition == 0) return 0;
	x = pTouchPosition -> x;
	y = pTouchPosition -> y;
	if(y > 540  && y < 1324) {
		if(x > 760  && x < 1790) return 1;
		if(x > 2300 && x < 3360) return 4;
	}
	if(y > 1700 && y < 2530) {
		if(x > 760  && x < 1790) return 2;
		if(x > 2300 && x < 3360) return 5;
	}
	if(y > 2840 && y < 3689) {
		if(x > 760  && x < 1790) return 3;
		if(x > 2300 && x < 3360) return 6;
	}
	return 0;
}

#endif /* GUI_GUI_MAIN_C_ */
