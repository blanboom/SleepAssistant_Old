/*
 * TimeAlarm.c
 *
 * 时钟和闹钟相关函数
 */

#include "TimeAlarm.h"

// 闹钟和时间的全局变量，靠 RTC 的秒中断更新
uint8_t alarmHour       = 7;  // 闹钟时
uint8_t alarmMinute     = 30; // 闹钟分
uint8_t alarmHourDiff   = 12;
uint8_t alarmMinuteDiff = 59; // 距闹钟响起还有多少小时多少分钟
struct tm currentTime   = {0, 0, 0, 0, 0, 0, 0, 0, 0}; // 现在的时间

/** 时钟初始化
 * 初始化 RTC，并将 RTC 中的时间读入内存
 */
void timeInit(void)
{
	time_t tmpTime;

	RTCTime_Init(); // 初始化 RTC
	time(&tmpTime); // 从 RTC 获取时间
	currentTime = *(localtime(&tmpTime)); // 转换时间格式
}

/** 设置时间日期
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
