/**
 * Motion.c
 *
 * 动作监测与识别
 */
#include "Motion.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "diag/Trace.h"

#define MPU_VALUE_LENGTH (10)     // MPU 读取数值数组的长度，长度越长，执行滤波算法是滤波越平滑
uint16_t moveSensitivity = 330;   // 判断身体移动的灵敏度，越小越灵敏，在此处使用变量而不使用宏定义，
								  // 是为了以后可以添加自动灵敏度校准程序

// 私有函数
void mpuSoftwareDelay(__IO uint32_t nCount);

int16_t mpuValue[MPU_VALUE_LENGTH - 1][6];   // 存放 MPU6050 读取值，前三个值为加速度，后三个值为角速度
int16_t mpuZeroValue[6];     // MPU6050 初始值

/**
 * 初始化 MPU6050
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

/*
 * 获取在正常情况下 MPU6050 的初始值
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

/**
 * 获得设备方向
 * 	判断 z 轴加速度值，
 * 	  如果大于一定值则返回“上”(DEVICE_POSITION_UP)，
 * 	  如果小于一定值则返回“下”(DEVICE_POSITION_DOWN)，
 * 	  如果不满足这两种情况，说明设备正在翻转或收到震动，返回“不稳定”(DEVICE_POSITION_UNSTABLE)
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

/**
 * 判断是否有身体移动
 * 	如果有身体移动则返回 1，可在一段时间内对此函数执行固定次数，统计次数并根据这个次数判断这段时间的睡眠质量
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
	//trace_printf("%d\n",ret);

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

/**
 * 软件延时
 */
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
