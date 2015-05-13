#include "DemoApp_SleepTrack.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "xpt2046.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"
#include "AnalogSensors.h"
#include "DHT11.h"

uint16_t sleepBadTotal = 0;
uint16_t sleepTotal    = 0;

uint8_t dhtValue[5];

void showNoteScreen_SleepTrack(void);
void showMainScreen_SleepTrack(void);
void sleepTrack(void);

void DemoApp_SleepTrack(void) {
	showNoteScreen_SleepTrack();
	AnalogSensors_Light_Init();
	motionInit();
	delay(200);
	while (GUI_Touch_Read_2046() == 0);
	showMainScreen_SleepTrack();
	delay(500);
	touch_flag = 0;
	for(;;) {
		sleepTrack();
		if(1 == touch_flag) { touch_flag = 0; break; }
	}
	AnalogSensors_Light_DeInit();
	motionDeInit();
}

void showNoteScreen_SleepTrack(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/nf1_1.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f1/nf1_2.bmp");
}

void showMainScreen_SleepTrack(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/f1_1.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f1/f1_2.bmp");
	GUI_Main_DrawGraph_Prepare(0, 45, 220, 180, White);
	GUI_Text(277, 77, "%", 1, Black, White);
	sleepBadTotal = 0;
	sleepTotal = 0;
}

void sleepTrack(void) {
	static uint8_t sleepBadValue;
	static uint8_t counter;
	if(counter >= 180) {
		/******* 睡眠质量部分 *******/
		GUI_Main_DrawGraph(sleepBadValue, Blue);
		if(sleepBadValue >= 2) { sleepBadTotal++; }
		sleepTotal++;
		GUI_Word(233, 77, 4, 9999 - sleepBadTotal*10000/sleepTotal, 2, Black, White);
		sleepBadValue = 0;
		counter       = 0;
		/******* 光照部分 *******/
		GUI_Word(233, 119, 4, AnalogSensors_ADCValue, 0, Black, White);
		/******* 温湿度部分 *******/
		DHT11_RawRead(dhtValue);
		GUI_Word(233, 161, 4, 100*DHT11_Temperature(dhtValue), 2, Black, White);
		GUI_Word(233, 203, 4, 100*DHT11_Humidity(dhtValue), 2, Black, White);
	}
	sleepBadValue += detectMove();
	counter++;
}
