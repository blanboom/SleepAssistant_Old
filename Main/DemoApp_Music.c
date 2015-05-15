/*
 * DemoApp_Music.c
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
 */

#include "DemoApp_Music.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "motion.h"
#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"
#include "VS1053.h"
#include "Switches.h"

/* 文件相关变量，用于储存 MP3 播放相关信息 */
FATFS fs;      /* Work area (file system object) for logical drive */
FRESULT res;
UINT br;       /* 读取出的字节数，用于判断是否到达文件尾 */
FIL fsrc;      /* file objects */
FILINFO finfo; /* 文件信息 */
DIR dirs;
BYTE buffer[512];       /* 存放读取出的文件数据 */
uint16_t fileCount = 0;

uint8_t nowPlaying = 1; // 现在正在播放的歌曲序号，1~9
uint8_t playCompleted = 0;

void showMainScreen_Music(void);
void playMusic_Prepare();
void playMusic_Switch();
void playMusic(void);
void playMusic_Stop();

void DemoApp_Music(void) {
	Coordinate* pTouchPosition;
	playMusic_Prepare();
	showMainScreen_Music(); /* 显示欢迎界面 */
	delay(200);
	for(;;) {
		playMusic();
		if(0 == Switches_Read_S1()) break;
		if(0 == Switches_Read_S2() || 1 == playCompleted) playMusic_Switch();
		if(1 == touch_flag) {
			pTouchPosition = GUI_Touch_Read_2046();
			if(pTouchPosition->x < 1032 && pTouchPosition->y < 1032) break;
		}
	}
	playMusic_Stop();
}

// 播放音乐的准备工作
void playMusic_Prepare() {
	VS1053_Init();
	motionInit();
	/* 打开文件 */
	res = f_open(&fsrc, "0:/Music/1.mp3", FA_OPEN_EXISTING | FA_READ);
	br = 1;
	TXDCS_SET(0); // 选择 VS1053 的数据端口
	// trace_printf("准备播放\n");
	res = f_read(&fsrc, buffer, sizeof(buffer), &br); // 读取第一部分数据
}

void playMusic_Switch() {
	f_close(&fsrc);
	nowPlaying++;
	if(nowPlaying > 9) { nowPlaying = 0; }
	switch(nowPlaying) {
	case 1:
		res = f_open(&fsrc, "0:/Music/1.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 2:
		res = f_open(&fsrc, "0:/Music/2.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 3:
		res = f_open(&fsrc, "0:/Music/3.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 4:
		res = f_open(&fsrc, "0:/Music/4.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 5:
		res = f_open(&fsrc, "0:/Music/5.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 6:
		res = f_open(&fsrc, "0:/Music/6.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 7:
		res = f_open(&fsrc, "0:/Music/7.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 8:
		res = f_open(&fsrc, "0:/Music/8.mp3", FA_OPEN_EXISTING | FA_READ); break;
	case 9:
		res = f_open(&fsrc, "0:/Music/9.mp3", FA_OPEN_EXISTING | FA_READ); break;
	default:
		break;
	}
	playCompleted = 0;
	br = 1;
	TXDCS_SET(0); // 选择 VS1053 的数据端口
	// trace_printf("准备播放\n");
	res = f_read(&fsrc, buffer, sizeof(buffer), &br); // 读取第一部分数据
}

// 播放音乐， 需要循环调用
void playMusic(void) {
	uint8_t j = 0;
	if (res == 0 && playCompleted == 0) {
		fileCount = 0; /* 512 字节完重新计数 */
		while (fileCount < 512)  { /* SD 卡读取一个 sector，一个 sector 为 512 字节 */
			if (DREQ != 0)  { /* 等待 DREQ 为高，请求数据输入 */
				for (j = 0; j < 32; j++) {
					VS1053_WriteByte(buffer[fileCount]); /* VS1053 的 FIFO 只有 32 个字节的缓冲 */
					fileCount++;
				}
			}
		}
		res = f_read(&fsrc, buffer, sizeof(buffer), &br);
	}
	if (res || (br == 0)) {
		playCompleted = 1;
	}
}

void showMainScreen_Music(void) {
	ili9320_Clear(0x001f);
	GUI_DisplayBMP_SkipBackground(0, 0, "/GUI/f5/f5_1.bmp", 0x001f);
	GUI_DisplayBMP_SkipBackground(160, 0, "/GUI/f5/f5_2.bmp", 0x001f);
}

// 关闭文件等
void playMusic_Stop() {
	f_close(&fsrc);
	VS1053_DeInit();
	motionDeInit();
}
