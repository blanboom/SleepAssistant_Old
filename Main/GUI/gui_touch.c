/*
 * gui_touch.c
 *
 *  Created on: 2015年3月10日
 *      Author: maxz
 */

#include "stm32f10x_conf.h"
#include "ili9320.h"
#include "gui_touch.h"
#include "SysTick.h"
#include "gui_basic.h"

#define RED	   0XF800
#define GREEN  0X07E0
#define BLUE   0X001F
#define BRED   0XF81F
#define GRED   0XFFE0
#define GBLUE  0X07FF
#define BLACK  0X0000
#define WHITE  0XFFFF
#define CAL_BACKGROUND_COLOR 	BLUE

Parameter touch_para;

Coordinate  display ;

/* DisplaySample LCD坐标上对应的ads7843采样AD值 如：LCD 坐标45,45 应该的X Y采样ADC分别为3388,920 */
Coordinate ScreenSample[4];

/* LCD上的坐标 */
Coordinate DisplaySample[4] =
{
    { 45, 45 },
    { 45, 270},
    { 190,190},
    {210,280}
} ;

/* 差值门限 */
#define THRESHOLD 2


void GUI_Touch_Init(void)
{
	XPT2046_Init();
}

/******************************************************
* 函数名：GUI_Touch_DrawCross
* 描述  ：在LCD指定位置画十字
* 输入  : Xpos		--X方向位置
Ypos		--Y方向位置
* 输出  ：无
* 举例  ：GUI_Touch_DrawCross(100,100);
* 注意  ：触摸校正专用
*********************************************************/
void GUI_Touch_DrawCross(uint16_t Xpos,uint16_t Ypos)
{
    u8 b;
    for(b=0; b<10; b++)
    {
    	ili9320_SetPoint(Ypos-b,240-Xpos,WHITE);
    	ili9320_SetPoint(Ypos+b,240-Xpos,WHITE);
    	ili9320_SetPoint(Ypos,240-(Xpos+b),WHITE);
    	ili9320_SetPoint(Ypos,240-(Xpos-b),WHITE);
    }
}

/******************************************************
* 函数名：GUI_Touch_Read_2046
* 描述  ：得到滤波之后的X Y
* 输入  : 无
* 输出  ：Coordinate结构体地址
* 举例  ：无
* 注意  ：速度相对比较慢
*********************************************************/
Coordinate *GUI_Touch_Read_2046(void)
{
    static Coordinate  screen;
    int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
    uint8_t count=0;

		/* 坐标X和Y进行9次采样*/
    int buffer[2][9]={{0},{0}};
    do
    {
        XPT2046_GetAdXY(TP_X,TP_Y);
        buffer[0][count]=TP_X[0];
        buffer[1][count]=TP_Y[0];
        count++;

    }	/*用户点击触摸屏时即TP_INT_IN信号为低 并且 count<9*/
    while(!INT_IN_2046&& count<9);


		/*如果触笔弹起*/
		if(INT_IN_2046 ==1)
    {
        /*中断标志复位*/
				touch_flag =0;
    }

		/* 如果成功采样9次,进行滤波 */
    if(count==9)
    {
        /* 为减少运算量,分别分3组取平均值 */
        temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
        temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
        temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;

        /* 计算3组数据的差值 */
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];

        /* 对上述差值取绝对值 */
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);

        /* 判断绝对差值是否都超过差值门限，如果这3个绝对差值都超过门限值，则判定这次采样点为野点,抛弃采样点，差值门限取为2 */
        if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD )
            return 0;

        /* 计算它们的平均值，同时赋值给screen */
        if(m0<m1)
        {
            if(m2<m0)
                screen.x=(temp[0]+temp[2])/2;
            else
                screen.x=(temp[0]+temp[1])/2;
        }
        else if(m2<m1)
            screen.x=(temp[0]+temp[2])/2;
        else
            screen.x=(temp[1]+temp[2])/2;

        /* 同上 计算Y的平均值 */
        temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
        temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
        temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        if(m0>THRESHOLD&&m1>THRESHOLD&&m2>THRESHOLD)
            return 0;

        if(m0<m1)
        {
            if(m2<m0)
                screen.y=(temp[0]+temp[2])/2;
            else
                screen.y=(temp[0]+temp[1])/2;
        }
        else if(m2<m1)
            screen.y=(temp[0]+temp[2])/2;
        else
            screen.y=(temp[1]+temp[2])/2;

        return &screen;
    }

    else if(count>1)
    {
        screen.x=buffer[0][0];
        screen.y=buffer[1][0];
        return &screen;
    }
    return 0;
}

/******************************************************
* 函数名：GUI_Touch_Read_2046_2
* 描述  ：得到简单滤波之后的X Y
* 输入  : 无
* 输出  ：Coordinate结构体地址
* 举例  ：无
* 注意  ：”画板应用实例"专用,不是很精准，但是速度比较快
*********************************************************/
Coordinate *GUI_Touch_Read_2046_2(void)
{
    static Coordinate  screen2;
    int TP_X[1],TP_Y[1];
    uint8_t count=0;
    int buffer[2][10]={{0},{0}};  /*坐标X和Y进行多次采样*/
    int min_x,max_x;
    int min_y,max_y;
    int	i=0;

    do
    {		/* 循环采样10次 */
        XPT2046_GetAdXY(TP_X,TP_Y);
        buffer[0][count]=TP_X[0];
        buffer[1][count]=TP_Y[0];
        count++;
    }	/*用户点击触摸屏时即TP_INT_IN信号为低 并且 count<10*/
    while(!INT_IN_2046&& count<10);

		/*如果触笔弹起*/
    if(INT_IN_2046)
    {
				/*中断标志复位*/
        touch_flag = 0;
    }

		/*如果成功采样10个样本*/
    if(count ==10)
    {
        max_x=min_x=buffer[0][0];
        max_y=min_y=buffer[1][0];
        for(i=1; i<10; i++)
        {
            if(buffer[0][i]<min_x)
            {
                min_x=buffer[0][i];
            }
            else
            if(buffer[0][i]>max_x)
            {
                max_x = buffer[0][i];
            }
        }

        for(i=1; i<10; i++)
        {
            if(buffer[1][i]<min_y)
            {
                min_y=buffer[1][i];
            }
            else
            if(buffer[1][i]>max_y)
            {
                max_y = buffer[1][i];
            }
        }

				/*去除最小值和最大值之后求平均值*/
        screen2.x=(buffer[0][0]+buffer[0][1]+buffer[0][2]+buffer[0][3]+buffer[0][4]+buffer[0][5]+buffer[0][6]+buffer[0][7]+buffer[0][8]+buffer[0][9]-min_x-max_x)>>3;
        screen2.y=(buffer[1][0]+buffer[1][1]+buffer[1][2]+buffer[1][3]+buffer[1][4]+buffer[1][5]+buffer[1][6]+buffer[1][7]+buffer[1][8]+buffer[1][9]-min_y-max_y)>>3;

        return &screen2;
    }

    return 0;

}

/******************************************************
* 函数名：GUI_Touch_CalPara
* 描述  ：计算出触摸屏到液晶屏坐标变换的转换函数的 K A B C D E F系数
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：只有在LCD和触摸屏间的误差角度非常小时,才能运用下面公式
*********************************************************/
FunctionalState GUI_Touch_CalPara( Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para)
{

    FunctionalState retTHRESHOLD = ENABLE ;

    /* K＝(X0－X2) (Y1－Y2)－(X1－X2) (Y0－Y2) */
    para->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                    ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;

		if( para->Divider == 0 )
    {
        retTHRESHOLD = DISABLE;
    }
    else
    {
        /* A＝((XD0－XD2) (Y1－Y2)－(XD1－XD2) (Y0－Y2))／K	*/
        para->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) -
                   ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y));

        /* B＝((X0－X2) (XD1－XD2)－(XD0－XD2) (X1－X2))／K	*/
        para->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) -
                   ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x));

        /* C＝(Y0(X2XD1－X1XD2)+Y1(X0XD2－X2XD0)+Y2(X1XD0－X0XD1))／K */
        para->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;

        /* D＝((YD0－YD2) (Y1－Y2)－(YD1－YD2) (Y0－Y2))／K	*/
        para->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) -
                   ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;

        /* E＝((X0－X2) (YD1－YD2)－(YD0－YD2) (X1－X2))／K	*/
        para->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) -
                   ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;


        /* F＝(Y0(X2YD1－X1YD2)+Y1(X0YD2－X2YD0)+Y2(X1YD0－X0YD1))／K */
        para->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
                   (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
                   (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y;
    }
    return( retTHRESHOLD ) ;
}


/******************************************************
* 函数名：GUI_Touch_GetPoint
* 描述  ：通过 K A B C D E F 把通道X Y的值转换为液晶屏坐标
* 输入  : 无
* 输出  ：返回1表示成功 0失败
* 举例  ：无
* 注意  ：如果获取的触点信息有误，将返回DISABLE
*********************************************************/
long double linear=0 ;
long double aa1=0,bb1=0,cc1=0,aa2=0,bb2=0,cc2=0;
FunctionalState GUI_Touch_GetPoint(Coordinate * displayPtr,
                                   Coordinate * screenPtr,
                                   Parameter * para )
{
    FunctionalState retTHRESHOLD =ENABLE ;

    if(screenPtr==0)
    {
        /*如果获取的触点信息有误，则返回DISABLE*/
				retTHRESHOLD = DISABLE;
    }
    else
        if( para->Divider != 0 )
        {


        displayPtr->x = ( (aa1 * screenPtr->x) +
                          (bb1 * screenPtr->y) +
                          cc1
                          ) ;

        displayPtr->y = ((aa2 * screenPtr->x) +
                         (bb2 * screenPtr->y) +
                         cc2
                         );
    }
    else
    {
        retTHRESHOLD = DISABLE;
    }
    return(retTHRESHOLD);
}


/******************************************************
* 函数名：GUI_Touch_GetPlace
* 描述  ：获取触摸LCD位置
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：如果触摸笔没有按下,将直接返回1
*********************************************************/
char  GUI_Touch_GetPlace(u16 *x, u16 *y)
{
		/*如果触笔有按下*/
    if(touch_flag == 1)
    {
        if(GUI_Touch_GetPoint(&display, GUI_Touch_Read_2046(), &touch_para ) !=DISABLE)
        {
            *x = display.x ;
            *y = display.y ;
            return 0;
        }
    }
    return 1;
}

/******************************************************
* 函数名：GUI_Touch_Calibrate
* 描述  ：触摸屏校正函数
* 输入  : 无
* 输出  ：0	---	校正成功
		 1	---	校正失败
* 举例  ：无
* 注意  ：无
*********************************************************/
int GUI_Touch_Calibrate(void)
{
    uint8_t i;
    u16 test_x=0, test_y=0;
    u16 gap_x=0, gap_y=0;
    Coordinate * Ptr;
   // delay_init();
    //Set_direction(0);
    for(i=0;i<4;i++)
    {
    	GUI_Rectangle(0,0,320,240,CAL_BACKGROUND_COLOR,CAL_BACKGROUND_COLOR);  //使整个屏幕显示背景颜色
    	GUI_Text(10, 10,"Touch Calibrate", 15, WHITE, CAL_BACKGROUND_COLOR);  //显示提示信息
    	GUI_Word(10, 25, 3, i+1, 0, WHITE, CAL_BACKGROUND_COLOR);				  //显示触点次数

        delay(500);
        GUI_Touch_DrawCross(DisplaySample[i].x,DisplaySample[i].y);  //显示校正用的“十”字
        do
        {
            Ptr=GUI_Touch_Read_2046();  //读取TSC2046数据到变量ptr
        }
        while( Ptr == (void*)0 );  //当ptr为空时表示没有触点被按下
        ScreenSample[i].x= Ptr->x; 	  //把读取的原始数据存放到ScreenSample结构体
        ScreenSample[i].y= Ptr->y;
    }

		/* 用原始参数计算出 原始参数与坐标的转换系数。 */
    GUI_Touch_CalPara( &DisplaySample[0],&ScreenSample[0],&touch_para ) ;

		/*计算X值*/
    test_x = ( (touch_para.An * ScreenSample[3].x) +
               (touch_para.Bn * ScreenSample[3].y) +
               touch_para.Cn
               ) / touch_para.Divider ;

		/*计算Y值*/
    test_y = ( (touch_para.Dn * ScreenSample[3].x) +
               (touch_para.En * ScreenSample[3].y) +
               touch_para.Fn
               ) / touch_para.Divider ;

    gap_x = (test_x > DisplaySample[3].x)?(test_x - DisplaySample[3].x):(DisplaySample[3].x - test_x);
    gap_x = (test_y > DisplaySample[3].y)?(test_y - DisplaySample[3].y):(DisplaySample[3].y - test_y);

    GUI_Rectangle(0,0,320,240,CAL_BACKGROUND_COLOR,CAL_BACKGROUND_COLOR);
    if((gap_x>11)||(gap_y>11))
    {
    	GUI_Text(100, 100,"Calibrate fail", 14, WHITE, CAL_BACKGROUND_COLOR);
    	GUI_Text(100, 120,"  try again   ", 14, WHITE, CAL_BACKGROUND_COLOR);
		delay(2000);
        return 1;
    }


    aa1 = (touch_para.An*1.0)/touch_para.Divider;
    bb1 = (touch_para.Bn*1.0)/touch_para.Divider;
    cc1 = (touch_para.Cn*1.0)/touch_para.Divider;

    aa2 = (touch_para.Dn*1.0)/touch_para.Divider;
    bb2 = (touch_para.En*1.0)/touch_para.Divider;
    cc2 = (touch_para.Fn*1.0)/touch_para.Divider;

    GUI_Text(100, 100,"Calibrate Success", 17, WHITE, CAL_BACKGROUND_COLOR);
    delay(1000);

    return 0;
}
