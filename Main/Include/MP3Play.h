/** MP3Play.h - MP3 应用函数库
 * 程序来源于《STM32库开发实战指南》随书光盘，有修改
 */

#ifndef __MP3PLAY_H
#define	__MP3PLAY_H

#include "stm32f10x.h"
#include "ff.h"
#include "diskio.h"

#define MIN(x, y)			((x) < (y)? (x) : (y))
#define READBUF_SIZE  ( 5*1024 )   /*5*1024*/

void mp3Init(void);
void mp3Play(void);

#endif /* __MP3PLAY_H */
