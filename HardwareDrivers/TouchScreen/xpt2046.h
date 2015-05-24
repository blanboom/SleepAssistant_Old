#ifndef _TOUCHPANEL_H_
#define _TOUCHPANEL_H_

#include "stm32f10x.h"

/* AD通道选择命令字和工作寄存器 */
#define	CHX 	0xd0 	/* 通道Y+的选择控制字 */
#define	CHY 	0x90		/* 通道X+的选择控制字 */

#define CS_2046(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_12);\
					else	\
					GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define INT_IN_2046   GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7)

extern volatile unsigned char touch_flag;

void XPT2046_ISR(void);
void XPT2046_Init(void);
void XPT2046_NVIC_Conf(void);
void XPT2046_GetAdXY(int *x,int *y);
int XPT2046_ReadY(void);
int XPT2046_ReadX(void);

#endif

