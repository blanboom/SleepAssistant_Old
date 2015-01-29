/*
 * RTC_Time.h
 *
 *  Created on: 2014年11月16日
 *      Author: maxz
 */
#ifndef _RTC_TIME_H_
#define _RTC_TIME_H_

#include <Time.h>

void RTCTime_Init(void);
void RTCTime_SetUnixTime(time_t time);
//void RTCTime_SetCalenderTime(struct tm t);
//time_t RTCTime_ConvCalendarToUnix(struct tm t);
//struct tm RTCTime_ConvUnixToCalendar(time_t t);
extern time_t time(time_t* timer);

#endif
