/*
 * DemoApp_Alarm.c
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
 */

#include "DemoApp_Alarm.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "Switches.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include "vs1053.h"
#include "sdio_sdcard.h"
#include "RTC_Time.h"
#include "Motion.h"

#define TRUE 1
#define FALSE 0

#define TOUCH_POSITION_NO    0
#define TOUCH_POSITION_BACK  1
#define TOUCH_POSITION_MODE1 2
#define TOUCH_POSITION_MODE2 3

/* 文件相关变量，用于储存 MP3 播放相关信息 */
FATFS fs;      /* Work area (file system object) for logical drive */
FRESULT res;
UINT br;       /* 读取出的字节数，用于判断是否到达文件尾 */
FIL fsrc;      /* file objects */
FILINFO finfo; /* 文件信息 */
DIR dirs;
BYTE buffer[512];       /* 存放读取出的文件数据 */
uint16_t count = 0;

uint16_t movedTotal = 0;
uint16_t moveDetectTotal = 0;
uint8_t touchState;

time_t currentUNIXTime;
Coordinate* pTouchPosition;

void showMainScreen_Alarm(void);
void showPlayingScreen_Alarm(void);
void showWaitingScreen_Alarm(void);
void playAlarm_Prepare(TCHAR*);
void playAlarm(void);
void playAlarm_Stop();
uint8_t checkTouchPosition(void);

void DemoApp_Alarm(void) {
	/* 显示欢迎界面 */
	showMainScreen_Alarm();
	movedTotal = 0;
	moveDetectTotal = 0;
	delay(200);
	for(;;) {
		touchState = checkTouchPosition();
		if(touchState == TOUCH_POSITION_BACK) return;
		if(touchState == TOUCH_POSITION_MODE1)
		{
			playAlarm_Prepare("5_Bird.mp3");
			break;
		}
		if(touchState == TOUCH_POSITION_MODE2)
		{
			showWaitingScreen_Alarm();
			motionInit();
			for(;;) { // 若身体有运动，说明睡眠比较浅，则打开闹钟
				moveDetectTotal++;
				if(detectMove()) movedTotal++;
				delay(10);
				if(moveDetectTotal > 1000) {
					if(moveDetectTotal / movedTotal <= 2) break;
				}
			}
			playAlarm_Prepare("5_Bird.mp3");
			break;
		}
	}
	showPlayingScreen_Alarm();
	for(;;) {
		playAlarm();
		if(0 == Switches_Read_S1()) break;
		if(0 == Switches_Read_Reed()) break;
	}

	playAlarm_Stop();
}

void showMainScreen_Alarm(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f3/f3_1.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f3/f3_2.bmp");
}

void showPlayingScreen_Alarm(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f3/f3_x21.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f3/f3_x22.bmp");
}

void showWaitingScreen_Alarm(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f3/f3_x11.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f3/f3_x12.bmp");
}

uint8_t checkTouchPosition(void) {
	uint16_t x, y;
	pTouchPosition = GUI_Touch_Read_2046();
	if(pTouchPosition == 0) { return TOUCH_POSITION_NO; }
	x = pTouchPosition->x; y = pTouchPosition->y;
	//trace_printf("%d, %d\n", x, y);
	if(x < 1032 && y < 1032) { return TOUCH_POSITION_BACK; }
	if(x > 1829 && x < 2705 && y > 1161 && y < 1790) { return TOUCH_POSITION_MODE1; }
	if(x > 1827 && x < 2794 && y > 2445 && y < 3074) { return TOUCH_POSITION_MODE2; }
	return TOUCH_POSITION_NO;
}

// 播放闹钟的准备工作
void playAlarm_Prepare(TCHAR* song) {
	VS1053_Init();
	Switches_LED1_ON();
	Switches_LED2_ON();
	/* 生成完整文件路径 */
	TCHAR songPath[20];
	strcpy(songPath, "0:/Alarm/");
	strcat(songPath, song);
	/* 打开文件 */
	res = f_open(&fsrc, songPath, FA_OPEN_EXISTING | FA_READ);
	br = 1;
	TXDCS_SET(0); // 选择 VS1053 的数据端口
	// trace_printf("准备播放\n");
	res = f_read(&fsrc, buffer, sizeof(buffer), &br); // 读取第一部分数据
}

// 播放闹钟， 需要循环调用
void playAlarm(void) {
	uint8_t j = 0;
	static uint8_t tmp = 0; // tmp = 1 时播放完毕
	time(&currentUNIXTime);
	if (res == 0 && tmp == 0) {
		count = 0; /* 512 字节完重新计数 */
		while (count < 512)  { /* SD 卡读取一个 sector，一个 sector 为 512 字节 */
			if (DREQ != 0)  { /* 等待 DREQ 为高，请求数据输入 */
				for (j = 0; j < 32; j++) {
					VS1053_WriteByte(buffer[count]); /* VS1053 的 FIFO 只有 32 个字节的缓冲 */
					count++;
				}
			}
		}
		if(currentUNIXTime % 2 == 0) {
			Switches_LED1_ON();
			Switches_LED2_OFF();
		} else {
			Switches_LED1_OFF();
			Switches_LED2_ON();
		}
		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
	}
	if (res || (br == 0)) {
		tmp = 1;
		if(currentUNIXTime % 2 == 0) {
			Switches_BUZZER_ON();  // 打开蜂鸣器
			Switches_LED1_ON();
			Switches_LED2_ON();
		} else {
			Switches_BUZZER_OFF();
			Switches_LED1_OFF();
			Switches_LED2_OFF();
		}
	}
}

// 关闭文件、蜂鸣器等
void playAlarm_Stop() {
	f_close(&fsrc);
	Switches_LED1_OFF();
	Switches_LED2_OFF();
	Switches_BUZZER_OFF();
	VS1053_DeInit();
	if(touchState == TOUCH_POSITION_MODE2) motionDeInit();
}
