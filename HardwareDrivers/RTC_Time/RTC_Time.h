/** RTC_Time.c - RTC 配置、时间读写所需的基础程序
 * 参考资料：http://www.ichanging.org/stm32-rtc.html
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
