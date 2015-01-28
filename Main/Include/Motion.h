/*
 * Motion.h
 *
 * 动作监测与识别
 */

#ifndef INCLUDE_MOTION_H_
#define INCLUDE_MOTION_H_
/**
 * 头文件
 */
#include "MPU6050.h"

/**
 * 设备方向
 */
#define DEVICE_POSITION_UP       (1)
#define DEVICE_POSITION_DOWN     (-1)
#define DEVICE_POSITION_UNSTABLE (0)

/**
 * 函数
 */
bool motionInit(void);
void getZeroMotionValue(void);
int8_t getDevicePosition(void);

#endif /* INCLUDE_MOTION_H_ */
