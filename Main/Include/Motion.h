/** Motion.h - 运动监测与识别
 * 监测睡眠质量、设备方向等的相关程序
 */

#ifndef INCLUDE_MOTION_H_
#define INCLUDE_MOTION_H_
/* 头文件 *************************************************************************************/
#include "MPU6050.h"
#include "stdbool.h"

/* 常量 ***************************************************************************************/
/* 设备方向 */
#define DEVICE_POSITION_UP       (1)
#define DEVICE_POSITION_DOWN     (-1)
#define DEVICE_POSITION_UNSTABLE (0)

/* 全局变量 ************************************************************************************/
extern uint32_t sleepWellStat;
extern uint32_t sleepBadStat;

/* 函数 ****************************************************************************************/
bool motionInit(void);
void motionDeInit(void);
void getZeroMotionValue(void);
int8_t getDevicePosition(void);
bool detectMove(void);
void sleepStat(void);
void clearSleepStatData(void);

#endif /* INCLUDE_MOTION_H_ */
