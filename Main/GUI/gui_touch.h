/*
 * gui_touch.h
 *
 *  Created on: 2015年3月10日
 *      Author: maxz
 */

#ifndef GUI_GUI_TOUCH_H_
#define GUI_GUI_TOUCH_H_

#include "xpt2046.h"

typedef struct Parameter
{
long double An,  			 //注:sizeof(long double) = 8
            Bn,
            Cn,
            Dn,
            En,
            Fn,
            Divider ;
}Parameter;

typedef	struct POINT
{
   uint16_t x;
   uint16_t y;
}Coordinate;

void GUI_Touch_Init();
Coordinate* GUI_Touch_Read_2046(void);
Coordinate* GUI_Touch_Read_2046_2(void);
int GUI_Touch_Calibrate(void);
void GUI_Touch_DrawCross(uint16_t Xpos,uint16_t Ypos);
FunctionalState GUI_Touch_CalPara( Coordinate * displayPtr,Coordinate * screenPtr,Parameter * para);
FunctionalState GUI_Touch_GetPoint(Coordinate * displayPtr,Coordinate * screenPtr,Parameter * para);
char GUI_Touch_GetPlace(u16 *x, u16 *y);

#endif /* GUI_GUI_TOUCH_H_ */
