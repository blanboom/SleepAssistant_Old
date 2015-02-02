/**
  ******************************************************************************
  * @file    MP3Play.c
  * @date    2-May-2011
  * @brief   mp3 应用函数库
  ******************************************************************************
  */

#include "MP3Play.h"
#include "SysTick.h"
#include "vs1053.h"
#include "sdio_sdcard.h"
#include <string.h>
#include <stdio.h>
#include "diag/Trace.h"

#define TRUE 1
#define FALSE 0

uint8_t readBuf[READBUF_SIZE];      /* 用于存储 MP3 帧头信息 */
struct tag_info id3v1;              /* ID3V1 结构体变量，用于储存歌曲附加信息 */
struct tag_info id3v2;              /* ID3V2 结构体变量，用于储存歌曲附加信息 */

void mp3Init(void)
{
	VS1053_SPI_Init();
	VS1053_MP3_Start();
}

/**
  * @brief  读取并播放音频文件
  * @note   支持 mp3, mid, wma 和部分 wav 格式
  * @param  None
  * @retval None
  */
void mp3Play(void)
{
	FATFS fs;      /* Work area (file system object) for logical drive */
	FRESULT res;
	UINT br;       /* 读取出的字节数，用于判断是否到达文件尾 */
	FIL fsrc;      /* file objects */
	FILINFO finfo; /* 文件信息 */
	DIR dirs;
	uint16_t count = 0;

	//char lfn[70];  /* 为支持长文件的数组，[] 最大支持 255 */
	char j = 0;
	char path[100] = {""};  /* SD 卡根目录 */
	char *result1, *result2, *result3, *result4;

	BYTE buffer[512];       /* 存放读取出的文件数据 */

	f_mount(0, &fs);        /* 挂载文件系统到 0 区 */

	if(f_opendir(&dirs, path) == FR_OK)                /* 打开根目录 */
	{
		while(f_readdir(&dirs, &finfo) == FR_OK)       /* 依次读文件名 */
		{
			//trace_printf("Read!");
			if(finfo.fattrib & AM_ARC)                 /* 判断是否为存档型文件 */
			{
				if(!finfo.fname[0])                   /* 文件名为空即达到了目录的末尾，退出 */
				{
					break;
				}

				trace_printf("\r\n文件名为：%s\r\n", finfo.fname);

				result1 = strstr(finfo.fname, ".mp3"); /* 判断是否为音频文件 */
				result2 = strstr(finfo.fname, ".MP3");
				result3 = strstr(finfo.fname, ".wav");
				result4 = strstr(finfo.fname, ".WAV");

				if (result1 != NULL || result2 != NULL || result3 != NULL
						|| result4 != NULL)
				{
					res = f_open(&fsrc, finfo.fname,
							FA_OPEN_EXISTING | FA_READ); /* 以只读方式打开 */
					br = 1;
					TXDCS_SET(0); /* 选择 VS1053 的数据接口 */

					/*--------------- 一曲开始 ---------------*/
					trace_printf("\r\n开始播放\r\n");
					for (;;)
					{
						res = f_read(&fsrc, buffer, sizeof(buffer), &br);
						if (res == 0)
						{
							count = 0; /* 512 字节完重新计数 */

							delay(5); /* 5ms */
							while (count < 512) /* SD 卡读取一个 sector，一个 sector 为 512 字节 */
							{
								if (DREQ != 0) /* 等待 DREQ 为高，请求数据输入 */
								{
									for (j = 0; j < 32; j++)
									{
										VS1053_WriteByte(buffer[count]); /* VS1053 的 FIFO 只有 32 个字节的缓冲 */
										count++;
									}
								}
							}
						}
						if (res || (br == 0))
						{
							break;
						}
					}
					trace_printf("\r\n播放结束\r\n");
					/*--------------- 一曲结束 ---------------*/
					/* 根据 VS1053 的要求，在一曲结束后需发送 2048 个 0 来确保下一首的正常播放 */
					while (count < 2048)
					{
						if ( DREQ != 0)
						{
							for (j = 0; j < 32; j++)
							{
								VS1053_WriteByte(0);
								count++;
							}
						}
					}
					count = 0;
					TXDCS_SET(1); /* 关闭 VS1053 数据端口 */
					f_close(&fsrc); /* 关闭打开的文件 */
				}
			}
		} /* while (f_readdir(&dirs, &finfo) == FR_OK) */
	} /* if (f_opendir(&dirs, path) == FR_OK)  */
} /* end of MP3_Play */
