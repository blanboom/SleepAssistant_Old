/**
  ******************************************************************************
  * @file    vs1053.c
  * @date    7-May-2011
  * @brief   vs1053 应用函数库
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
  */

#include "vs1053.h"
#include "SysTick.h"

/**
 * @brief Init VS1053 IO
 * @param None
 * @retval None
 */
void VS1053_SPI_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 使能 VS1053 所用 I/O 及 SPI 的时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_SPI1, ENABLE);

	/* 配置 SPI1 引脚 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA4-XCS(片选) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PC1-RST(复位) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC2-XDCS(数据命令选择) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* PC3-DREQ(数据中断) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  /* 上拉输入 */
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	/* Enable SPI1  */
	SPI_Cmd(SPI1, ENABLE);
}


/**
 * @brief 以 SPI 方式向 VS1053 写入一个字节
 * @param byte 被写入的数值
 * @retval None
 */
u8 VS1053_WriteByte(u8 byte)
{
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);		// 判断SPI1 发送缓冲区是否空
  SPI_I2S_SendData(SPI1, byte);						                   // 发送8位数据
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	  // 判断是否接收缓冲区非空
  return SPI_I2S_ReceiveData(SPI1);
}

/**
 * @brief 以 SPI 方式向 VS1053 读取一个字节
 * @param None
 * @retval SPI 读取的信息
 */
u8 VS1053_ReadByte(void)
{
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, 0);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	return SPI_I2S_ReceiveData(SPI1);
}

/**
 * @brief 写 VS1053 寄存器
 * @param highbyte 高 8 位
 * @param lowbyte  低 8 位
 * @param addressbyte 寄存器地址
 * @retval None
 */
void VS1053_WriteRegister(u8 addressbyte, u8 highbyte, u8 lowbyte)
{
	TXDCS_SET(1);
	TCS_SET(0);

	VS1053_WriteByte( VS_WRITE_COMMAND );
	VS1053_WriteByte( addressbyte );
	VS1053_WriteByte( highbyte );
	VS1053_WriteByte( lowbyte );

	TCS_SET(1);
}

/*
 * @brief 读 VS1053 寄存器
 * @param addressbyte 寄存器地址
 * @retval VS1053 对应地址寄存器中的值
 */
u16 VS1053_ReadRegister(u8 addressbyte)
{
	u16 resultvalue = 0;
	TXDCS_SET(1);
	TCS_SET(0);

	VS1053_WriteByte(VS_READ_COMMAND); 		                   //发送读寄存器命令
	VS1053_WriteByte((addressbyte));				                 //发送寄存器的地址
	resultvalue = (unsigned int )(VS1053_ReadByte() << 8);   //读取高8位数据
	resultvalue |= VS1053_ReadByte();  		                   //读取低8位数据

	TCS_SET(1);
	return resultvalue;                                      //返回16位寄存器的值
}

/**
 * @brief 初始化 VS1053 的 MP3 播放功能
 * @param None
 * @retval None
 */
void VS1053_MP3_Start(void)
{
	u8 BassEnhanceValue = 0x00;      /* 低音值先初始化为 0 */
	u8 TrebleEnhanceValue = 0x00;     /* 高音值先初始化为 0 */
	TRST_SET(0);
	delay(1);

	VS1053_WriteByte(0xff);                   // 发送一个字节的无效数据，启动SPI传输
	TXDCS_SET(1);
	TCS_SET(1);
	TRST_SET(1);
	delay(1);

	VS1053_WriteRegister( SPI_MODE,0x08,0x00);  	// 进入VS1003的播放模式
	VS1053_WriteRegister(3, 0x98, 0x00);   		    // 设置vs1003的时钟,3倍频
	VS1053_WriteRegister(5, 0xBB, 0x81);   		    // 采样率48k，立体声
	VS1053_WriteRegister(SPI_BASS, TrebleEnhanceValue, BassEnhanceValue);// 设置重低音
	VS1053_WriteRegister(0x0b,0x00,0x00);      	                         // VS1003 音量
	delay(1);

	while( DREQ == 0 );						   	        // 等待DREQ为高  表示能够接受音乐数据输入

}
