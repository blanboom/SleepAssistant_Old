#include "DemoApp_SleepTrack.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "ili9320.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"


uint16_t sleepBadTotal = 0;
uint16_t sleepTotal    = 0;

void showNoteScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/nf1.bmp");
}

void showMainScreen(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f1/f1.bmp");
	GUI_Main_DrawGraph_Prepare(0, 45, 220, 180, White);
	GUI_Text(290, 65, "%", 1, Black, White);
	sleepBadTotal = 0;
	sleepTotal = 0;
}

void sleepTrack(void) {
	static uint8_t sleepBadValue;
	static uint8_t counter;
	if(counter >= 180) {
		GUI_Main_DrawGraph(sleepBadValue, Blue);
		if(sleepBadValue >= 2) { sleepBadTotal++; }
		sleepTotal++;
		GUI_Word(240, 65, 4, sleepBadTotal*10000/sleepTotal, 2, Black, White);
		sleepBadValue = 0;
		counter       = 0;

	}
	sleepBadValue += detectMove();
	counter++;
}