/*
 * Switches.c
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
 */

#include "Switches.h"

void Switches_Init(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
			RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_Pin = S1_PIN | S2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = REED_PIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN | LED1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = LED2_PIN;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

	Switches_BUZZER_OFF();
	Switches_LED1_OFF();
	Switches_LED2_OFF();
}
