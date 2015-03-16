/*
 * gui_touch.c
 *
 *  Created on: 2015年3月10日
 *      Author: maxz
 */

#include "stm32f10x_conf.h"

void touchInit(void)
{
	Screen_Touch_Init();
}

uint16_t getTouchX(void) 	  //Èí¼þÂË²¨
{
	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//Êý¾Ý×é
	unsigned short temp=0,X=0;

	while(count<10)//Ñ­»·¶ÁÊý10´Î
	{
		databuffer[count]=Screen_Touch_ReadX();
		count++;
	}
	if(count==10)//Ò»¶¨Òª¶Áµ½10´ÎÊý¾Ý,·ñÔò¶ªÆú
	{
	    do//½«Êý¾ÝXÉýÐòÅÅÁÐ
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//ÉýÐòÅÅÁÐ
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);
		X=(databuffer[3]+databuffer[4]+databuffer[5])/3;
	}
	return(X);
}

uint16_t getTouchY(void)  //Èí¼þÂË²¨
{
  	unsigned char t=0,t1,count=0;
	unsigned short int databuffer[10]={5,7,9,3,2,6,4,0,3,1};//Êý¾Ý×é
	unsigned short temp=0,Y=0;

    while(count<10)	//Ñ­»·¶ÁÊý10´Î
	{
		databuffer[count]=Screen_Touch_ReadY();
		count++;
	}
	if(count==10)//Ò»¶¨Òª¶Áµ½10´ÎÊý¾Ý,·ñÔò¶ªÆú
	{
	    do//½«Êý¾ÝXÉýÐòÅÅÁÐ
		{
			t1=0;
			for(t=0;t<count-1;t++)
			{
				if(databuffer[t]>databuffer[t+1])//ÉýÐòÅÅÁÐ
				{
					temp=databuffer[t+1];
					databuffer[t+1]=databuffer[t];
					databuffer[t]=temp;
					t1=1;
				}
			}
		}while(t1);
		Y=(databuffer[3]+databuffer[4]+databuffer[5])/3;
	}
	return(Y);
}
