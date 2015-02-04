#include "USART1.h"
#include <stdarg.h>
#include <stdio.h>
#include "string.h"
#include "diag/Trace.h"

uint8_t USART1_ReadBuffer[USART1_BUFFER_LENTH] = {0};  // USART1 接收缓冲区长度，暂定为 1 字节
uint8_t volatile USART1_ReadBufferPointer = 0;  // USART1 接收缓冲区指针

void USART1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200ul;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

int USART1_Printf(const char* format, ...)
{
  uint16_t counter;
  int ret;
  va_list ap;

  va_start (ap, format);

  static char buf[128];

  // Print to the local buffer
  ret = vsnprintf (buf, sizeof(buf), format, ap);
  if (ret > 0)
    {
	  for(counter = 0; counter < ret; counter++)
	  {
		  while( USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET );
		  USART_SendData(USART1, buf[counter]);
	  }
    }

  va_end (ap);
  return ret;
}

void USART1_ISR(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		USART1_ReadBufferPointer++;   // 第 0 字节浪费
	    USART1_ReadBuffer[USART1_ReadBufferPointer]=USART1->DR;
	  	trace_printf("U1:%c\n", USART1_ReadBuffer[USART1_ReadBufferPointer]);    //将接受到的数据直接返回打印
	  	if(USART1_ReadBufferPointer >= USART1_BUFFER_LENTH)
	  	{
	  		USART1_ReadBufferPointer = 0;
	  	}
	}
}
