#include "xpt2046.h"
#include "systick.h"
#include "stm32f10x_conf.h"
#include "misc.h"

volatile unsigned char touch_flag;

/******************************************************
* 函数名：Touch_init
* 描述  ：2046 Touch 端口初始化
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：无
*********************************************************/
void XPT2046_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef   SPI_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOG,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE ) ;

    //Configure SPI2 pins: SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //Configure SPI2 pins: SCK, MISO and MOSI
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //Configure PB12 pin: TP_CS pin
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //Interrupt Pin
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOG,&GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource7);
    EXTI_InitStructure.EXTI_Line = EXTI_Line7;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    CS_2046(1);

    // SPI2 Config
    SPI_Cmd(SPI2, DISABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;   //SPI_NSS_Hard
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2,&SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE);
}

/******************************************************
* 函数名：TOUCH_NVIC_Configuration
* 描述  ：2046 Touch INT 嵌套中断配置
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：留意一下优先级
*********************************************************/
void XPT2046_NVIC_Conf(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    /*使能EXTI9_5 中断 */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn  ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/******************************************************
* 函数名：DelayUS
* 描述  ：粗略延时cnt 微妙
* 输入  : cnt
* 输出  ：无
* 举例  ：无
* 注意  ：该函数创建的目的是局部使用,从而不必特意去使用定时器来延时
*********************************************************/
static void DelayUS(vu32 cnt)
{
    uint16_t i;
    for(i = 0;i<cnt;i++)
    {
        uint8_t us = 12; /* 设置值为12，大约延1微秒 */
        while (us--)     /* 延1微秒	*/
        {
            ;
        }
    }
}

/******************************************************
* 函数名：WR_CMD
* 描述  ：向 Touch 2046写数据
* 输入  : cmd 		--需要写入的数据
* 输出  ：无
* 举例  ：无
* 注意  ：内部使用
*********************************************************/
static void WR_CMD (uint8_t cmd)
{
    /* Wait for SPI2 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send SPI2 data */
    SPI_I2S_SendData(SPI2,cmd);
    /* Wait for SPI2 data reception */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Read SPI2 received data */
    SPI_I2S_ReceiveData(SPI2);
}

/******************************************************
* 函数名：Read_touch_ad
* 描述  ：通过SPI方式读取 2046所获得的触摸AD转换值
* 输入  : 无
* 输出  ：无
* 举例  ：无
* 注意  ：内部使用
*********************************************************/
static int Read_touch_ad(void)
{
    unsigned short buf,temp;
    /*  等待数据寄存器空 */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /* 通过SPI2发送数据 */
    SPI_I2S_SendData(SPI2,0x0000);

    /* 等待接收到一个字节的数据 */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* 读取SPI2接收的数据 */
    temp=SPI_I2S_ReceiveData(SPI2);

    buf=temp<<8;
    DelayUS(1);

		/*  等待数据寄存器空 */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);

    /* 通过SPI2发送数据 */
    SPI_I2S_SendData(SPI2,0x0000);

    /* 等待接收到一个字节的数据 */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);

    /* 读取SPI2接收的数据 */
    temp=SPI_I2S_ReceiveData(SPI2);

    buf |= temp;
    buf>>=3;
    buf&=0xfff;
    return buf;
}

/******************************************************
* 函数名：Read_X
* 描述  ：通过SPI方式读取 2046所获得的触摸 通道X+ AD转换值
* 输入  : 无
* 输出  ：X+通道AD转换值
* 举例  ：无
* 注意  ：无
*********************************************************/
int XPT2046_ReadX(void)
{
    int i;
    CS_2046(0);
    DelayUS(1);
    WR_CMD(CHX);
    DelayUS(1);
    i=Read_touch_ad();
    CS_2046(1);
    return i;
}

/******************************************************
* 函数名：Read_Y
* 描述  ：通过SPI方式读取 2046所获得的触摸 通道Y+ AD转换值
* 输入  : 无
* 输出  ：Y+通道AD转换值
* 举例  ：无
* 注意  ：无
*********************************************************/
int XPT2046_ReadY(void)
{
    int i;
    CS_2046(0);
    DelayUS(1);
    WR_CMD(CHY);
    DelayUS(1);
    i=Read_touch_ad();
    CS_2046(1);
    return i;
}

/******************************************************
* 函数名：Touch_GetAdXY
* 描述  ：SPI方式 读取2046 通道X+ 通道Y+的ADC值
* 输入  : 无
* 输出  ：通道X+ 通道Y+的ADC值
* 举例  ：无
* 注意  ：无
*********************************************************/
void XPT2046_GetAdXY(int *x,int *y)
{
    int adx,ady;
    adx=XPT2046_ReadX();
    DelayUS(1);
    ady=XPT2046_ReadY();
    *x=adx;
    *y=ady;
}

void XPT2046_ISR(void) {
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        GPIO_ResetBits(GPIOG, GPIO_Pin_7);
        touch_flag=1;
        EXTI_ClearITPendingBit(EXTI_Line7);
    }
}
