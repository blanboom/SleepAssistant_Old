/** TimeAlarm.c - 维持时间更新，处理时间和闹钟
 */

#ifndef INCLUDE_TIMEALARM_H_
#define INCLUDE_TIMEALARM_H_

#include "stm32f10x_conf.h"
#include "RTC_Time.h"

extern volatile int8_t alarmHour, alarmMinute, alarmHourDiff, alarmMinuteDiff, alarmON;
extern struct tm currentTime;

void timeInit(void);
int8_t setTime(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void timeAlarmUpdate(void);

#endif /* INCLUDE_TIMEALARM_H_ */
