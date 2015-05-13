/** Motion.c - 运动监测与识别
 * 监测睡眠质量、设备方向等的相关程序
 */

#include "Motion.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "diag/Trace.h"

/* 相关常量与变量 *****************************************************************************/
#define MPU_VALUE_LENGTH (10)  // MPU 读取数值数组的长度，长度越长，执行滤波算法是滤波越平滑

/** 睡眠质量监测的灵敏度
 * 越小越灵敏
 * 在此处使用变量而不使用宏定义，是为了以后可以添加自动灵敏度校准程序
 */
uint16_t moveSensitivity = 260;

/** 睡眠质量统计的所需的变量
 * 在一个时间段内，若判断到睡眠质量较好，则 sleepWellStat 加一
 *                                否则 sleepBadStat 加一
 * 起床时，通过 sleepWellStat 与 sleepWellStat 的比值可得出睡眠质量
 */
uint32_t sleepWellStat = 0;
uint32_t sleepBadStat  = 0;

int16_t mpuValue[MPU_VALUE_LENGTH - 1][6];   // 存放 MPU6050 读取值，前三个值为加速度，后三个值为角速度
int16_t mpuZeroValue[6];     // MPU6050 初始值

/* 相关函数 ***********************************************************************************/
void mpuSoftwareDelay(__IO uint32_t nCount);


/** bool motionInit(void) - 运动监测初始化
 * 初始化 MPU6050 等设备
 * 输出：
 * 		TRUE  - 初始化成功
 * 		FALSE - 初始化失败
 */
bool motionInit(void)
{
	MPU6050_I2C_Init();
	MPU6050_Initialize();
#ifdef DEBUG
	if(MPU6050_TestConnection())
	{
		trace_printf("MPU6050 Init OK\n");
		return TRUE;
	}
	else
	{
		trace_printf("MPU6050 Init Failed\n");
		return FALSE;
	}
#else
	return MPU6050_TestConnection();
#endif
}

void motionDeInit() {
	MPU6050_DeInit();
}
/** void getZeroMotionValue(void) - 获取在正常情况下 MPU6050 的初始值
 * 本操作需要在设备放稳时进行
 */
void getZeroMotionValue(void)
{
	int32_t filter_sum;
	uint8_t i, j, k;
	int16_t tmp;

	// 从 MPU6050 中读取 MPU_VALUE_LENGTH 个角速度与加速度值，放入 mpuValue 中
	for (j = 0; j < MPU_VALUE_LENGTH; j++)
	{
		MPU6050_GetRawAccelGyro(mpuValue[j]);
		mpuSoftwareDelay(60000);
		mpuSoftwareDelay(60000);
		mpuSoftwareDelay(60000);
		mpuSoftwareDelay(60000);
		mpuSoftwareDelay(60000);
	}

	// 计算初始值
	for (k = 0; k < 6; k++)
	{
		// 对读取到的若干个数进行冒泡排序
		for (j = 0; j < MPU_VALUE_LENGTH - 1; j++)
		{
			for (i = 0; i < MPU_VALUE_LENGTH - 1 - j; i++)
			{
				if (mpuValue[i][k] > mpuValue[i + 1][k])
				{
					tmp = mpuValue[i][k];
					mpuValue[i][k] = mpuValue[i + 1][k];
					mpuValue[i + 1][k] = tmp;
				}
			}
		}

		// 去掉最大值和最小值，求算术平均值
		filter_sum = 0;
		for (i = 1; i < MPU_VALUE_LENGTH - 1; i++)
		{
			filter_sum += mpuValue[i][k];
		}
		mpuZeroValue[k] = filter_sum / (MPU_VALUE_LENGTH - 2);
	}
#ifdef DEBUG
	trace_printf("%d\t%d\t%d\t%d\t%d\t%d\n",
					mpuZeroValue[0], mpuZeroValue[1], mpuZeroValue[2], mpuZeroValue[3], mpuZeroValue[4], mpuZeroValue[5]);
#endif
}

/** int8_t getDevicePosition(void) - 获得设备方向
 * 	通过判断 z 轴加速度值来判断设备方向
 * 	  如果 z 轴加速度值大于一定值则返回“上”(DEVICE_POSITION_UP)，
 * 	  如果小于一定值则返回“下”(DEVICE_POSITION_DOWN)，
 * 	  如果不满足这两种情况，说明设备正在翻转或受到震动，返回“不稳定”(DEVICE_POSITION_UNSTABLE)
 *
 * 	为得到更加稳定的结果，可以在一定时间间隔内调用两次或多次该函数，若返回的结果一致，说明此时设备的方向稳定
 */
int8_t getDevicePosition(void)
{
	int16_t mpuPositionValue[6];
	MPU6050_GetRawAccelGyro(mpuPositionValue);
	if(mpuPositionValue[2] > 12500)
	{
		return DEVICE_POSITION_UP;
	}
	else if(mpuPositionValue[2] < -13200)
	{
		return DEVICE_POSITION_DOWN;
	}
	else
	{
		return DEVICE_POSITION_UNSTABLE;
	}
}

/** bool detectMove(void) - 判断是否有身体移动
 * 通过角速度来判断，如果有身体移动则返回 1，否则返回 0
 * 可在一段时间内对此函数执行固定次数，统计次数并根据这个次数判断这段时间的睡眠质量
 */
bool detectMove(void)
{
	int32_t filter_sum;
	uint32_t ret = 0;
	uint8_t i, j, k;
	static uint8_t l = 0;
	int16_t tmp;

	// 读取数值
	MPU6050_GetRawAccelGyro(mpuValue[l]);
	l++;
	if(l == MPU_VALUE_LENGTH) {l = 0;}

	// 对三个角速度进行限幅平均滤波
	for (k = 3; k < 6; k++)
	{
		// 对读取到的若干个数进行冒泡排序
		for (j = 0; j < MPU_VALUE_LENGTH - 1; j++)
		{
			for (i = 0; i < MPU_VALUE_LENGTH - 1 - j; i++)
			{
				if (mpuValue[i][k] > mpuValue[i + 1][k])
				{
					tmp = mpuValue[i][k];
					mpuValue[i][k] = mpuValue[i + 1][k];
					mpuValue[i + 1][k] = tmp;
				}
			}
		}

		// 去掉最大值和最小值，求算术平均值
		filter_sum = 0;
		for (i = 1; i < MPU_VALUE_LENGTH - 1; i++)
		{
			filter_sum += mpuValue[i][k];
		}
		filter_sum = abs(filter_sum / (MPU_VALUE_LENGTH - 2)) - abs(mpuZeroValue[k]);
		// 求三个角速度的平方和
		ret += filter_sum*filter_sum;
	}

	ret = sqrt(ret);
	// trace_printf("%d\n",ret);

	// 判断三个角速度的平方和的大小，如果比设定的灵敏度大，就返回 1
	if(ret > moveSensitivity)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/** void sleepStat(void) - 判断睡眠质量
 * 本函数通过多次调用 detectMove() 来判断睡眠质量，
 * 		并统计在 sleepBadStat 和 sleepWellStat 中
 */
void sleepStat(void)
{
	static uint16_t counter = 0;
	static uint16_t sleepBadCounter = 0;
	if(detectMove() == 1)
	{
		sleepBadCounter++;
	}
	counter++;
	if (counter >= 1000)
	{
		counter = 0;
		if(sleepBadCounter >= 2)
		{
			sleepBadStat++;
			trace_printf("SleepBad\n");
		}
		else
		{
			sleepWellStat++;
			trace_printf("SleepGood\n");
		}
		sleepBadCounter = 0;
	}
}

/** void clearSleepStatData(void) - 清除睡眠质量相关数据
 * 	使 sleepBadStat 和 sleepWellStat 归零
 */
void clearSleepStatData(void)
{
	sleepWellStat = 0;
	sleepBadStat  = 0;
}

/* 软件延时 */
void mpuSoftwareDelay(__IO uint32_t nCount)
{
	for (; nCount != 0; nCount--)
	{
		__NOP();
		__NOP();
		__NOP();
		__NOP();
	}
}
