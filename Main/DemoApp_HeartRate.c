/*
 * 演示程序：心率
 */

#include "DemoApp_HeartRate.h"
#include "gui_basic.h"
#include "gui_main.h"
#include "gui_touch.h"
#include "ili9320.h"
#include "motion.h"
#include <stdio.h>
#include "diag/Trace.h"
#include "SysTick.h"
#include "AnalogSensors.h"
#include "RTC_Time.h"

#define TRUE  1
#define FALSE 0

volatile int16_t  BPM;               // int that holds raw Analog in 0. updated every 2mS
volatile int16_t  Signal;            // holds the incoming raw data
volatile int16_t  IBI = 600;         // int that holds the time interval between beats! Must be seeded!
volatile uint8_t  Pulse = FALSE;     // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile uint8_t  QS = FALSE;        // becomes true when Arduoino finds a beat.
volatile int16_t  rate[10];          // array to hold last ten IBI values
volatile uint32_t sampleCounter = 0; // used to determine pulse timing
volatile uint32_t lastBeatTime = 0;  // used to find IBI
volatile int16_t  P = 2820;           // used to find peak in pulse wave, seeded
volatile int16_t  T = 2820;           // used to find trough in pulse wave, seeded
volatile int16_t  thresh = 2820;      // used to find instant moment of heart beat, seeded
volatile int16_t  amp = 100;         // used to hold amplitude of pulse waveform, seeded
volatile uint8_t  firstBeat = TRUE;  // used to seed rate array so we startup with reasonable BPM
volatile uint8_t  secondBeat = FALSE;// used to seed rate array so we startup with reasonable BPM


void showMainScreen_HeartRate(void);
void calcHeartRate(void);
void updateAnimation(void);

void DemoApp_HeartRate(void) {
	static time_t tmp_time1, tmp_time2;
	static uint32_t tmp_sysTime = 0;
	showMainScreen_HeartRate();
	AnalogSensors_HeartRate_Init();
	delay(5000);
	touch_flag = 0;
	for(;;) {
		//trace_printf("%d\n", AnalogSensors_ADCValue);
		time(&tmp_time1);
		if(sysTime % 2 == 0 && tmp_sysTime != sysTime) {
			tmp_sysTime = sysTime;
			calcHeartRate();
		}

		if(tmp_time1 != tmp_time2) {
			tmp_time2 = tmp_time1;
			updateAnimation();  // 更新动画
			if(tmp_time1 % 8 == 0) {
				trace_printf("%d, %d, %d, %d, %d\n", BPM, P, T, thresh, amp);
				// 更新显示数据
				if      (BPM < 60)  GUI_DisplayBMP(103, 80, "/GUI/f2/75.bmp");
				else if (BPM < 65)  GUI_DisplayBMP(103, 80, "/GUI/f2/65.bmp");
				else if (BPM < 70)  GUI_DisplayBMP(103, 80, "/GUI/f2/70.bmp");
				else if (BPM < 75)  GUI_DisplayBMP(103, 80, "/GUI/f2/75.bmp");
				else if (BPM < 80)  GUI_DisplayBMP(103, 80, "/GUI/f2/80.bmp");
				else if (BPM < 85)  GUI_DisplayBMP(103, 80, "/GUI/f2/85.bmp");
				else if (BPM < 90)  GUI_DisplayBMP(103, 80, "/GUI/f2/90.bmp");
				else if (BPM < 95)  GUI_DisplayBMP(103, 80, "/GUI/f2/95.bmp");
				else if (BPM < 100) GUI_DisplayBMP(103, 80, "/GUI/f2/100.bmp");
				else if (BPM < 105) GUI_DisplayBMP(103, 80, "/GUI/f2/105.bmp");
				else if (BPM < 110) GUI_DisplayBMP(103, 80, "/GUI/f2/110.bmp");
				else if (BPM < 115) GUI_DisplayBMP(103, 80, "/GUI/f2/115.bmp");
				else if (BPM < 120) GUI_DisplayBMP(103, 80, "/GUI/f2/120.bmp");
				else if (BPM < 125) GUI_DisplayBMP(103, 80, "/GUI/f2/125.bmp");
				else if (BPM < 130) GUI_DisplayBMP(103, 80, "/GUI/f2/130.bmp");
				else if (BPM < 135) GUI_DisplayBMP(103, 80, "/GUI/f2/135.bmp");
				else if (BPM < 140) GUI_DisplayBMP(103, 80, "/GUI/f2/140.bmp");
				else                GUI_DisplayBMP(103, 80, "/GUI/f2/110.bmp");
			}
		}
		if(1 == touch_flag) break;
	}
	AnalogSensors_HeartRate_DeInit();
}

void showMainScreen_HeartRate(void) {
	ili9320_Clear(White);
	GUI_DisplayBMP(0, 0, "/GUI/f2/f2_1.bmp");
	GUI_DisplayBMP(160, 0, "/GUI/f2/f2_2.bmp");
}

/* 更新动画 */
void updateAnimation(void) {
	static uint8_t count1 = 0;
	static uint8_t count2 = 0;
	switch (count2) {
	case 0:
		GUI_Circle(100 + 30*count1, 160, 4, White, 1); break;
	case 1:
		GUI_Circle(100 + 30*count1, 160, 4, 0xfc46, 1); break;
	case 2:
		GUI_Circle(100 + 30*count1, 160, 4, 0x001f, 1); break;
	default:
		break;
	}
	count1++;
	if(count1 > 4) {
		count1 = 0;
		count2++;
		if(count2 > 2) count2 = 0;
	}
}

/* 心率计算
 * TODO: 采用 PulseSensor 的 Arduino 程序，但目前不能正常工作
 */
void calcHeartRate(void) {
	Signal = AnalogSensors_ADCValue;              // read the Pulse Sensor
	sampleCounter += 2;       // keep track of the time in mS with this variable
	int N = sampleCounter - lastBeatTime; // monitor the time since the last beat to avoid noise

	//  find the peak and trough of the pulse wave
	if (Signal < thresh && N > (IBI / 5) * 3) { // avoid dichrotic noise by waiting 3/5 of last IBI
		if (Signal < T) {                        // T is the trough
			T = Signal;             // keep track of lowest point in pulse wave
		}
	}

	if (Signal > thresh && Signal > P) {   // thresh condition helps avoid noise
		P = Signal;                             // P is the peak
	}                               // keep track of highest point in pulse wave

	//  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
	// signal surges up in value every time there is a pulse
	if (N > 250) {                                 // avoid high frequency noise
		if ((Signal > thresh) && (Pulse == false) && (N > (IBI / 5) * 3)) {
			Pulse = true;   // set the Pulse flag when we think there is a pulse
			//      digitalWrite(blinkPin,HIGH);                // turn on pin 13 LED
			IBI = sampleCounter - lastBeatTime; // measure time between beats in mS
			lastBeatTime = sampleCounter;   // keep track of time for next pulse

			if (secondBeat) { // if this is the second beat, if secondBeat == TRUE
				secondBeat = false;                  // clear secondBeat flag
				for (int i = 0; i <= 9; i++) { // seed the running total to get a realisitic BPM at startup
					rate[i] = IBI;
				}
			}

			if (firstBeat) { // if it's the first time we found a beat, if firstBeat == TRUE
				firstBeat = false;                   // clear firstBeat flag
				secondBeat = true;                   // set the second beat flag
				return;                 // IBI value is unreliable so discard it
			}

			// keep a running total of the last 10 IBI values
			uint16_t runningTotal = 0;        // clear the runningTotal variable

			for (int i = 0; i <= 8; i++) {       // shift data in the rate array
				rate[i] = rate[i + 1];         // and drop the oldest IBI value
				runningTotal += rate[i];       // add up the 9 oldest IBI values
			}

			rate[9] = IBI;               // add the latest IBI to the rate array
			runningTotal += rate[9];       // add the latest IBI to runningTotal
			runningTotal /= 10;               // average the last 10 IBI values
			BPM = 60000 / runningTotal; // how many beats can fit into a minute? that's BPM!
			QS = true;                              // set Quantified Self flag
			// QS FLAG IS NOT CLEARED INSIDE THIS ISR
		}
	}

	if (Signal < thresh && Pulse == true) { // when the values are going down, the beat is over
		//    digitalWrite(blinkPin,LOW);            // turn off pin 13 LED
		Pulse = false;             // reset the Pulse flag so we can do it again
		amp = P - T;                          // get amplitude of the pulse wave
		thresh = amp / 2 + T;              // set thresh at 50% of the amplitude
		P = thresh;                            // reset these for next time
		T = thresh;
	}

	if (N > 2500) {                       // if 2.5 seconds go by without a beat
		thresh = 2820;                          // set thresh default
		P = 2820;                               // set P default
		T = 2820;                               // set T default
		lastBeatTime = sampleCounter; // bring the lastBeatTime up to date
		firstBeat = true;                      // set these to avoid noise
		secondBeat = false;                    // when we get the heartbeat back
	}

}
