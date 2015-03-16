#ifndef _GUI_BASIC_H_
#define _GUI_BASIC_H_

#include "stm32f10x_conf.h"

u16 GUI_Color565(u32 RGB);  // RGB��ɫתΪ16λ(565)

void GUI_Init();
void GUI_Text(u16 x, u16 y, char *str, u16 len,u16 Color, u16 bkColor);
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color);
void GUI_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill);
void GUI_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill);
void GUI_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill);
void GUI_Chinese_Text(u16 x,u16 y, char str[],u8 len,u16 charColor,u16 bkColor);
void GUI_Word(u16  x, u16  y,u16  maxXS, u32  Da,  u16 Point, u16 Color, u16 bkColor);

/* LCD color */
#define White          0xFFFF
#define Black          0x0000
#define Grey           0xF7DE
#define Blue           0x001F
#define Blue2          0x051F
#define Red            0xF800
#define Magenta        0xF81F
#define Green          0x07E0
#define Cyan           0x7FFF
#define Yellow         0xFFE0

#define Line0          0
#define Line1          24
#define Line2          48
#define Line3          72
#define Line4          96
#define Line5          120
#define Line6          144
#define Line7          168
#define Line8          192
#define Line9          216

#endif
