#include <gui_basic.h>
#include "stm32f10x_conf.h"
#include "ili9320.h"
#include "ff.h"
#include "diskio.h"
#include <stdio.h>
#include "diag/Trace.h"

UINT BytesRead;
uint8_t Buffer[_MAX_SS];

/****************************************************************************
* 名    称：GUI_CmpColor()
* 功    能：判断颜色值是否一致。
* 入口参数：color1		颜色值1
*		        color2		颜色值2
* 出口参数：返回1表示相同，返回0表示不相同。
* 说    明：
****************************************************************************/
#define  GUI_CmpColor(color1, color2)	( (color1&0x01) == (color2&0x01) )

void GUI_Init()
{
	ili9320_Initializtion();
	ili9320_Clear(0xffff);
}

/****************************************************************************
* 名    称：u16 GUI_Color565(u32 RGB)
* 功    能：将RGB颜色转换为16位颜色。
* 入口参数：RGB  颜色值
* 出口参数：返回16位RGB颜色值。
* 说    明：
* 调用方法：i=GUI_Color565(0xafafaf);
****************************************************************************/
u16 GUI_Color565(u32 RGB)
{
  u8  r, g, b;

  b = ( RGB >> (0+3) ) & 0x1f;		// ȡBɫ�ĸ�5λ
  g = ( RGB >> (8+2) ) & 0x3f;		// ȡGɫ�ĸ�6λ
  r = ( RGB >> (16+3)) & 0x1f;		// ȡRɫ�ĸ�5λ
   
  return( (r<<11) + (g<<5) + (b<<0) );		
}

/****************************************************************************
* 名    称：void GUI_Text(u16 x, u16 y, u8 *str, u16 len,u16 Color, u16 bkColor)
* 功    能：在指定座标显示字符串
* 入口参数：x      行座标
*           y      列座标
*           *str   字符串
*           len    字符串长度
*           Color  字符颜色
*           bkColor字符背景颜色
* 出口参数：无
* 说    明：
* 调用方法：GUI_Text(0,0,"0123456789",10,0x0000,0xffff);
****************************************************************************/
void GUI_Text(u16 x, u16 y, char *str, u16 len,u16 Color, u16 bkColor)
{
  u8 i;
  
  for (i=0;i<len;i++)
  {
    ili9320_PutChar((x+8*i),y,*str++,Color,bkColor);
  }
}

/****************************************************************************
* 名    称：void GUI_Text(u16 x, u16 y, u8 *str, u16 len,u16 Color, u16 bkColor)
* 功    能：在指定座标显示汉字文本
* 入口参数：x      行座标
*           y      列座标
*           *str   汉字串
*           len    字串长度
*           Color  字符颜色
*           bkColor字符背景颜色
* 出口参数：无
* 说    明：一个汉字两个字符，记得哟，5个汉字len就是10
* 调用方法：GUI_Text(0,0,"成都贝一特",10,0x0000,0xffff);
****************************************************************************/
void GUI_Chinese_Text(u16 x,u16 y, char str[],u8 len,u16 charColor,u16 bkColor)
{
	FIL myfsrc;
	FRESULT myres;
	UINT mybr;

    u16 i=0,b;
    u16 j=0;
	u16 x_add,y_add;
	u8 buffer[32];
    u16 tmp_char=0,index=0;
	x_add=x;
	y_add=y;
	
    myres = f_open(&myfsrc , "0:/GUI/songti.fon", FA_OPEN_EXISTING | FA_READ);
    if ( myres != FR_OK ) { return; }

	for(b=0;b<len/2;b++)
	{
#ifdef UTF8
		    index = 0; // TODO: 编码转换程序
#else
		    //index = ((0xd4-0xb0)*94+0xf3-0xa1)*32;
		    index = ((str[b*2] - 0xb0)*94 + str[b*2+1]-0xa1)*32;
			//index=(94*(str[b*2] - 0xa1)+(str[b*2+1] - 0xa1));
#endif
			f_lseek (&myfsrc, index);
			myres = f_read( &myfsrc, buffer, 32, &mybr );

		    for (i=0;i<16;i++)
		    {
		        tmp_char=buffer[i*2];
		        tmp_char=(tmp_char<<8);
		        tmp_char|=buffer[2*i+1];
		        for (j=0;j<16;j++)
		        {
		            if ( ((tmp_char >> (15-j)) & 0x01) == 0x01)
		            {
		            	ili9320_SetPoint(x_add+j,y_add+i,charColor);
		            }
		            else
		            {
		            	ili9320_SetPoint(x_add+j,y_add+i,bkColor);
		            }
		        }
		    }
		    x_add=x_add+17;
	}

	f_close(&myfsrc);
}
/*******************************************************************************
函数名     ： DispWord
功能       ： 显示9位以下数字，最大显示999999999
参数       ： x     : 坐标X
              y     : 坐标Y
			  maxFb : 要显示的位数，注：小于要显示的位数前面补0。
			  Da    : 要显示的数据
			  PosFb : 反现位置  从零到maxFb
			  Point : 小数点的位数
返回值     ： 无
*******************************************************************************///
void GUI_Word(u16  x, u16  y,u16  maxXS, u32  Da,  u16 Point, u16 Color, u16 bkColor)
{
 	u8  i;//,Fb;
    
	u32 NUM;
	NUM=1;
 	for(i=0; i<(maxXS-1); i++)  NUM=NUM*10;//����Ҫ��ʾ��λ��ȡλ��  
	if(Point!=0)ili9320_PutChar((x+(maxXS -Point)*8),y,0x2e,Color,bkColor);//���С����λ��������ʾС����
	
	for(i=0; i<(maxXS-0); i++)  //�����λ������ʾҪ��ʾ��λ��
	{   
		if(i<=(maxXS-Point-1))  //�����λ��С�������ڵ�λ��ǰ��λ������С�������ڵ�λ����������ʾ
		{
		   ili9320_PutChar((x+i*8),y,((Da/NUM)%10+0x30),Color,bkColor);
		   NUM=NUM/10;
                   
		}
		else                    //������ʾ������λһλ����������ʾС����
		{
		   ili9320_PutChar((x+i*8+8),y,((Da/NUM)%10+0x30),Color,bkColor);
		   NUM=NUM/10;
		} 
	}	
	return;
}


/****************************************************************************
* 名    称：void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color)
* 功    能：在指定座标画直线
* 入口参数：x0     A点行座标
*           y0     A点列座标
*           x1     B点行座标
*           y1     B点列座标
*           color  线颜色
* 出口参数：无
* 说    明：
* 调用方法：GUI_Line(0,0,240,320,0x0000);
****************************************************************************/
void GUI_Line(u16 x0, u16 y0, u16 x1, u16 y1,u16 color)
{
 	u16 x,y;
 	u16 dx;// = abs(x1 - x0);
 	u16 dy;// = abs(y1 - y0);

	if(y0==y1)
	{
		if(x0<=x1)
		{
			x=x0;
		}
		else
		{
			x=x1;
			x1=x0;
		}
  		while(x <= x1)
  		{
   			ili9320_SetPoint(x,y0,color);
   			x++;
  		}
  		return;
	}
	else if(y0>y1)
	{
		dy=y0-y1;
	}
	else
	{
		dy=y1-y0;
	}
 
 	if(x0==x1)
	{
		if(y0<=y1)
		{
			y=y0;
		}
		else
		{
			y=y1;
			y1=y0;
		}
  		while(y <= y1)
  		{
   			ili9320_SetPoint(x0,y,color);
   			y++;
  		}
  		return;
	}
	else if(x0 > x1)
 	{
		dx=x0-x1;
  		x = x1;
  		x1 = x0;
  		y = y1;
  		y1 = y0;
 	}
 	else
 	{
		dx=x1-x0;
  		x = x0;
  		y = y0;
 	}

 	if(dx == dy)
 	{
  		while(x <= x1)
  		{

   			x++;
			if(y>y1)
			{
				y--;
			}
			else
			{
   				y++;
			}
   			ili9320_SetPoint(x,y,color);
  		}
 	}
 	else
 	{
 		ili9320_SetPoint(x, y, color);
  		if(y < y1)
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
    			s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
     				{
      					p += twoDy;
     				}
     				else
     				{
      					y++;
      					p += twoDyMinusDx;
     				}
     				ili9320_SetPoint(x, y,color);
    			}
   			}
   			else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
    			s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y < y1)
    			{
     				y++;
     				if(p < 0)
     				{
      					p += twoDx;
     				}
     				else
     				{
      					x++;
      					p+= twoDxMinusDy;
     				}
     				ili9320_SetPoint(x, y, color);
    			}
   			}
  		}
  		else
  		{
   			if(dx > dy)
   			{
    			s16 p = dy * 2 - dx;
    			s16 twoDy = 2 * dy;
	    		s16 twoDyMinusDx = 2 * (dy - dx);
    			while(x < x1)
    			{
     				x++;
     				if(p < 0)
	     			{
    	  				p += twoDy;
     				}
     				else
     				{
      					y--;
	      				p += twoDyMinusDx;
    	 			}
     				ili9320_SetPoint(x, y,color);
    			}
   			}
	   		else
   			{
    			s16 p = dx * 2 - dy;
    			s16 twoDx = 2 * dx;
	    		s16 twoDxMinusDy = 2 * (dx - dy);
    			while(y1 < y)
    			{
     				y--;
     				if(p < 0)
	     			{
    	  				p += twoDx;
     				}
     				else
     				{
      					x++;
	      				p+= twoDxMinusDy;
    	 			}
     				ili9320_SetPoint(x, y,color);
    			}
   			}
  		}
 	}
}

/****************************************************************************
* 名    称：void GUI_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
* 功    能：在指定座标画圆，可填充
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void GUI_Circle(u16 cx,u16 cy,u16 r,u16 color,u8 fill)
{
	u16 x,y;
	s16 delta,tmp;
	x=0;
	y=r;
	delta=3-(r<<1);

	while(y>x)
	{
		if(fill)
		{
			GUI_Line(cx+x,cy+y,cx-x,cy+y,color);
			GUI_Line(cx+x,cy-y,cx-x,cy-y,color);
			GUI_Line(cx+y,cy+x,cx-y,cy+x,color);
			GUI_Line(cx+y,cy-x,cx-y,cy-x,color);
		}
		else
		{
			ili9320_SetPoint(cx+x,cy+y,color);
			ili9320_SetPoint(cx-x,cy+y,color);
			ili9320_SetPoint(cx+x,cy-y,color);
			ili9320_SetPoint(cx-x,cy-y,color);
			ili9320_SetPoint(cx+y,cy+x,color);
			ili9320_SetPoint(cx-y,cy+x,color);
			ili9320_SetPoint(cx+y,cy-x,color);
			ili9320_SetPoint(cx-y,cy-x,color);
		}
		x++;
		if(delta>=0)
		{
			y--;
			tmp=(x<<2);
			tmp-=(y<<2);
			delta+=(tmp+10);
		}
		else
		{
			delta+=((x<<2)+6);
		}
	}
}

/****************************************************************************
* 名    称：void GUI_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill)
* 功    能：在指定区域画矩形，可填充颜色
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void GUI_Rectangle(u16 x0, u16 y0, u16 x1, u16 y1,u16 color,u8 fill)
{
	if(fill)
	{
		u16 i;
		if(x0>x1)
		{
			i=x1;
			x1=x0;
		}
		else
		{
			i=x0;
		}
		for(;i<=x1;i++)
		{
			GUI_Line(i,y0,i,y1,color);
		}
		return;
	}
	GUI_Line(x0,y0,x0,y1,color);
	GUI_Line(x0,y1,x1,y1,color);
	GUI_Line(x1,y1,x1,y0,color);
	GUI_Line(x1,y0,x0,y0,color);
}

/****************************************************************************
* 名    称：void  GUI_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill)
* 功    能：在指定区域画正方形，可填充颜色
* 入口参数：
* 出口参数：
* 说    明：
* 调用方法：
****************************************************************************/
void  GUI_Square(u16 x0, u16 y0, u16 with, u16 color,u8 fill)
{
	GUI_Rectangle(x0, y0, x0+with, y0+with, color,fill);
}

/**
 * @brief  Displays a bitmap picture loaded in the sd card.(16 位，R5 G6 B5)
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @param  BmpName: Bmp file name in the sd card.
 * @retval None
 */
void GUI_DisplayBMP(unsigned short int Xpos, unsigned short int Ypos,
		char* pic_name) {
	uint32_t size = 0, offset = 0;
	uint32_t width = 0, height = 0, l_width;
	uint32_t compression, clrUsed;
	//uint16_t type
	uint16_t bitCount;
	uint32_t BmpAddress;
	uint32_t x, y;
	uint32_t Color;
	//uint32_t BkColor, MixColor;
	FIL F;
	unsigned char tmp_name[20];

	sprintf((char*) tmp_name, "0:%s", pic_name);
	f_open(&F, (char *) tmp_name, FA_READ);
	f_read(&F, Buffer, 54, &BytesRead);
	BmpAddress = (uint32_t) Buffer;

//	type = *(uint16_t *) (BmpAddress);
	/* Read bitmap size */
	size = *(uint16_t *) (BmpAddress + 2);
	size |= (*(uint16_t *) (BmpAddress + 4)) << 16;
	/* Get bitmap data address offset */
	offset = *(uint16_t *) (BmpAddress + 10);
	offset |= (*(uint16_t *) (BmpAddress + 12)) << 16;
	/* Read bitmap width */
	width = *(uint16_t *) (BmpAddress + 18);
	width |= (*(uint16_t *) (BmpAddress + 20)) << 16;
	/* Read bitmap height */
	height = *(uint16_t *) (BmpAddress + 22);
	height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

	bitCount = *(uint16_t *) (BmpAddress + 28);

	compression = *(uint16_t *) (BmpAddress + 30);
	compression |= *(uint16_t *) (BmpAddress + 32) << 16;

	clrUsed = *(uint16_t *) (BmpAddress + 46);
	clrUsed |= *(uint16_t *) (BmpAddress + 48) << 16;

	size = (size - offset) / 2;
	l_width = (((width * bitCount)+31)/32*4);
	f_read(&F, Buffer, offset - 54, &BytesRead);
	switch (bitCount) {
	case 1:
	case 4:
	case 8:
		break;
	case 16:
		//for (y = height - 1; (y < height) && (y >= 0); y -= 1)
		for (y = height - 1; y < height; y -= 1) {
			f_read(&F, Buffer, l_width, &BytesRead);
			for (x = 0; x < width; x++) {
				Color = *(uint16_t*) (Buffer + 2 * x);
				ili9320_SetCursor(Xpos + x, Ypos + y);
				LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
				LCD_WriteRAM(Color);
				//trace_printf("%x\n", Color);
			}
		}
		f_close(&F);
		break;
	case 24:
	case 32:
		break;
	}
}

void GUI_DisplayBMP_SkipBackground(unsigned short int Xpos, unsigned short int Ypos,
		char* pic_name, uint16_t backgroundColor) {
	uint32_t size = 0, offset = 0;
	uint32_t width = 0, height = 0, l_width;
	uint32_t compression, clrUsed;
	//uint16_t type
	uint16_t bitCount;
	uint32_t BmpAddress;
	uint32_t x, y;
	uint32_t Color;
	//uint32_t BkColor, MixColor;
	FIL F;
	unsigned char tmp_name[20];

	sprintf((char*) tmp_name, "0:%s", pic_name);
	f_open(&F, (char *) tmp_name, FA_READ);
	f_read(&F, Buffer, 54, &BytesRead);
	BmpAddress = (uint32_t) Buffer;

//	type = *(uint16_t *) (BmpAddress);
	/* Read bitmap size */
	size = *(uint16_t *) (BmpAddress + 2);
	size |= (*(uint16_t *) (BmpAddress + 4)) << 16;
	/* Get bitmap data address offset */
	offset = *(uint16_t *) (BmpAddress + 10);
	offset |= (*(uint16_t *) (BmpAddress + 12)) << 16;
	/* Read bitmap width */
	width = *(uint16_t *) (BmpAddress + 18);
	width |= (*(uint16_t *) (BmpAddress + 20)) << 16;
	/* Read bitmap height */
	height = *(uint16_t *) (BmpAddress + 22);
	height |= (*(uint16_t *) (BmpAddress + 24)) << 16;

	bitCount = *(uint16_t *) (BmpAddress + 28);

	compression = *(uint16_t *) (BmpAddress + 30);
	compression |= *(uint16_t *) (BmpAddress + 32) << 16;

	clrUsed = *(uint16_t *) (BmpAddress + 46);
	clrUsed |= *(uint16_t *) (BmpAddress + 48) << 16;

	size = (size - offset) / 2;
	l_width = (((width * bitCount)+31)/32*4);
	f_read(&F, Buffer, offset - 54, &BytesRead);
	switch (bitCount) {
	case 1:
	case 4:
	case 8:
		break;
	case 16:
		//for (y = height - 1; (y < height) && (y >= 0); y -= 1)
		for (y = height - 1; y < height; y -= 1) {
            f_read(&F, Buffer, l_width, &BytesRead);
			for (x = 0; x < width; x++) {
				Color = *(uint16_t*) (Buffer + 2 * x);
				if (Color != backgroundColor) {
					ili9320_SetCursor(Xpos + x, Ypos + y);
					LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */
					LCD_WriteRAM(Color);
				}
			}
		}
		f_close(&F);
		break;
	case 24:
	case 32:
		break;
	}
}
