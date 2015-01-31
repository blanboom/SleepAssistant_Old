//参考资料：http://www.ichanging.org/stm32-rtc.html

#include "RTC_Time.h"
#include "stm32f10x_conf.h"


// 重新配置 RTC 和 BKP，仅在检测到后备寄存器数据丢失时使用
void RTCTime_Config()
{
	// 启用PWR和BKP的时钟（fromAPB1）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
	// 后备域解锁
	PWR_BackupAccessCmd(ENABLE);
	// 备份寄存器模块复位
	BKP_DeInit();
	// 外部晶振 32.768K
	RCC_LSEConfig(RCC_LSE_ON);
	// 等待稳定
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	//RTC时钟源配置成LSE（外部32.768K）
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	//RTC开启
	RCC_RTCCLKCmd(ENABLE);
	//开启后需要等待APB1时钟与RTC时钟同步，才能读写寄存器
	RTC_WaitForSynchro();
	//读写寄存器前，要确定上一个操作已经结束
	RTC_WaitForLastTask();
	// 启动 RTC 中断
	RTC_ITConfig(RTC_IT_SEC, ENABLE);
	RTC_WaitForLastTask();
	//设置RTC分频器，使RTC时钟为1Hz
	//RTCperiod=RTCCLK/RTC_PR=(32.768KHz)/(32767+1)
	RTC_SetPrescaler(32767);
	//等待寄存器写入完成
	RTC_WaitForLastTask();

}

void RTCTime_Init()  // RTC 初始化，上电后调用
{
	//在BKP的后备寄存器1中，存了一个特殊字符0xA5A5
	//第一次上电或后备电源掉电后，该寄存器数据丢失，
	//表明RTC数据丢失，需要重新配置
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A5)
	{
		//重新配置RTC
		RTCTime_Config();
		//配置完成后，向后备寄存器中写特殊字符0xA5A5
		BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	}
	else
	{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		// 启动 RTC 秒中断
		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
	}
}

// 设置 time_t 格式的时间
void RTCTime_SetUnixTime(time_t time)
{
	RTC_WaitForLastTask();
    RTC_SetCounter((uint32_t)time);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
	RTC_WaitForLastTask();
}

// 与 C 标准库中的 time() 用法相同，获取 time_t 格式的时间
time_t time(time_t* timer)
{
	time_t tmp = (uint32_t)RTC_GetCounter();
	*timer = tmp;
	return tmp;
}

////////////////////////////////////////////////////////////////
// 下面代码暂时无需使用
//
//// 设置 tm 格式的时间日期
//void RTCTime_SetCalenderTime(struct tm t)
//{
//	RTCTime_SetUnixTime(RTCTime_ConvCalendarToUnix(t));
//}
//// 将 tm 格式 时间日期转换为 time_t 格式
//time_t RTCTime_ConvCalendarToUnix(struct tm t)
//{
//	t.tm_year -= 1900;
//	return mktime(&t);
//}
//
//// 将 time_t 格式时间日期转换为 tm 格式
//struct tm RTCTime_ConvUnixToCalendar(time_t t)
//{
//	struct tm *t_tm;
//	t_tm = localtime(&t);
//	t_tm->tm_year += 1900;
//	return *t_tm;
//}
