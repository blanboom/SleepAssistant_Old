/**
  ******************************************************************************
  * @file    vs1053.h
  * @date    7-May-2011
  * @brief   vs1053 应用函数库的头文件
  ******************************************************************************
  * @note
  * 硬件连接：  --------------------------------
  *           | PA5-SPI1_SCK : VS1053-SCLK     |
  *           | PA6-SPI1_MISO: VS1053-SO	   |
  *           | PA7-SPI1_MOSI: VS1053-SI	   |
  *           | PA4-SPI1_NSS : VS1053-XCS	   |
  *           | PC1          : VS1053-RST	   |
  *           | PC2          : VS1053-XDCS	   |
  *           | PC3          : VS1053-DREQ	   |
  *            --------------------------------
  ******************************************************************************
  * 程序来源于《STM32库开发实战指南》随书光盘，有修改
  */

#ifndef __VS1053_H
#define	__VS1053_H

#include "stm32f10x_conf.h"

/*
 * VS1003(音频解码芯片)控制接口宏定义，这些I/O都是普通I/O，如需改变I/O，
 * 只要改变这写宏定义即可，其他应用函数不用改变，可方便移植。
 * 这些接口分别为：
 * 1-XCS   VS1003片选
 * 2-RST   VS1003复位
 * 3-XDCS	 VS1003数据命令选择
 * 4-DREQ  VS1003数据中断
 */

#define TCS   (1<<4)  // PA4-XCS
#define TCS_SET(x)  GPIOA->ODR=(GPIOA->ODR&~TCS)|(x ? TCS:0)

#define RST   (1<<6)  // PF6-RST
#define TRST_SET(x)  GPIOF->ODR=(GPIOF->ODR&~RST)|(x ? RST:0)

#define XDCS   (1<<4)  // PF4-XDCS
#define TXDCS_SET(x)  GPIOC->ODR=(GPIOF->ODR&~XDCS)|(x ? XDCS:0)

											 // PF8-DREQ
#define DREQ  GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)


/* VS1003(音频解码芯片) 命令宏定义 */
#define VS_WRITE_COMMAND 			0x02
#define VS_READ_COMMAND 			0x03

#define SPI_MODE        			0x00
#define SPI_STATUS      			0x01
#define SPI_BASS        			0x02
#define SPI_CLOCKF      			0x03
#define SPI_DECODE_TIME 			0x04
#define SPI_AUDATA      			0x05
#define SPI_WRAM        			0x06
#define SPI_WRAMADDR    			0x07
#define SPI_HDAT0       			0x08
#define SPI_HDAT1       			0x09
#define SPI_AIADDR      			0x0a
#define SPI_VOL         			0x0b
#define SPI_AICTRL0     			0x0c
#define SPI_AICTRL1     			0x0d
#define SPI_AICTRL2     			0x0e
#define SPI_AICTRL3     			0x0f

#define SM_DIFF         			0x01
#define SM_JUMP         			0x02
#define SM_RESET        			0x04
#define SM_OUTOFWAV     			0x08
#define SM_PDOWN        			0x10
#define SM_TESTS        			0x20
#define SM_STREAM       			0x40
#define SM_PLUSV        			0x80
#define SM_DACT         			0x100
#define SM_SDIORD       			0x200
#define SM_SDISHARE     			0x400
#define SM_SDINEW       			0x800
#define SM_ADPCM        			0x1000
#define SM_ADPCM_HP     			0x2000


#define MP3CMD_InitVS1003			0x11
#define MP3CMD_Play				  	0x12
#define MP3CMD_Pause			  	0x13
#define MP3CMD_Stop				  	0x14
#define MP3CMD_Next				  	0x15
#define MP3CMD_TestVS1003			0x16


void VS1053_Init(void);
u8 VS1053_WriteByte( u8 byte );
u8 VS1053_ReadByte(void);
void VS1053_WriteRegister(u8 addressbyte, u8 highbyte, u8 lowbyte);
u16 VS1053_ReadRegister(u8 addressbyte);

#endif /* __VS1003_H */
