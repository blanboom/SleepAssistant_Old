/** TimeAlarm.c - 维持时间更新，处理时间和闹钟
 */

#include "TimeAlarm.h"
#include <stdio.h>
#include "diag/Trace.h"

/* 相关变量 ***********************************************************************************/
/* 闹钟和时间的全局变量，靠 RTC 的秒中断更新 */
volatile int8_t alarmHour       = 7;  // 闹钟时
volatile int8_t alarmMinute     = 30; // 闹钟分
volatile int8_t alarmHourDiff   = 12; // 离下一次闹钟响还有多少小时
volatile int8_t alarmMinuteDiff = 59; // 当前分钟与闹钟分钟之间的差值（可以为负数）
volatile int8_t alarmON         = 0;  // 闹钟开启指示，0 代表闹钟关闭，其他数值代表闹钟的不同模式（待定）
struct tm currentTime   = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // 现在的时间

/* 相关函数 ***********************************************************************************/

/** void timeInit(void) - 时钟初始化
 * 初始化 RTC，并将 RTC 中的时间读入内存
 */
void timeInit(void)
{
	time_t tmpTime;

	RTCTime_Init(); // 初始化 RTC
	time(&tmpTime); // 从 RTC 获取时间
	currentTime = *(localtime(&tmpTime)); // 转换时间格式
}

/** settime() - 设置时间日期
 * 年为减去 1900 后的值，月份的值为 0~11，而不是 1~12
 */
int8_t setTime(uint8_t year, uint8_t month, uint8_t day,
		uint8_t hour, uint8_t minute, uint8_t second)
{
	time_t tmpTime;
	currentTime.tm_sec  = second;
	currentTime.tm_min  = minute;
	currentTime.tm_hour = hour;
	currentTime.tm_mday = day;
	currentTime.tm_mon  = month;
	currentTime.tm_year = year;
	tmpTime = mktime(&currentTime);   // 转化为 time_t 格式
	if(tmpTime < 0) {return tmpTime;}
	RTCTime_SetUnixTime(tmpTime);     // 将时间存放在 RTC
	return 0;
}

/** void timeAlarmUpdate(void) - 时间与闹钟更新
 * 放在 RTC 的中断服务程序里
 */
void timeAlarmUpdate(void)
{
	time_t tmpTime;
	time(&tmpTime); // 从 RTC 获取时间
	currentTime = *(localtime(&tmpTime)); // 转换时间格式
	alarmHourDiff = alarmHour - currentTime.tm_hour;
	if(alarmHourDiff < 0) { alarmHourDiff += 24; }
	alarmMinuteDiff = alarmMinute - currentTime.tm_min;
}
