/*
 * Switches.h
 *
 *  Created on: 2015年5月9日
 *      Author: maxz
 */

#ifndef SWITCHES_SWITCHES_H_
#define SWITCHES_SWITCHES_H_

#include "stm32f10x_conf.h"

#define S1_PIN      GPIO_Pin_7
#define S1_PORT     GPIOB
#define S2_PIN      GPIO_Pin_9
#define S2_PORT     GPIOB

#define REED_PIN    GPIO_Pin_1
#define REED_PORT   GPIOC

#define BUZZER_PIN  GPIO_Pin_5
#define BUZZER_PORT GPIOE
#define LED1_PIN    GPIO_Pin_6
#define LED1_PORT   GPIOE

#define LED2_PIN    GPIO_Pin_1
#define LED2_PORT   GPIOF

void Switches_Init(void);

inline void Switches_LED1_ON(void);
inline void Switches_LED2_ON(void);
inline void Switches_LED1_OFF(void);
inline void Switches_LED2_OFF(void);
inline void Switches_BUZZER_ON(void);
inline void Switches_BUZZER_OFF(void);
inline uint8_t Switches_Read_Reed(void);
inline uint8_t Switches_Read_S1(void);
inline uint8_t Switches_Read_S2(void);

inline void
__attribute__((always_inline)) Switches_LED1_ON(void) {
	GPIO_SetBits(LED1_PORT, LED1_PIN);
}

inline void
__attribute__((always_inline)) Switches_LED2_ON(void) {
	GPIO_SetBits(LED2_PORT, LED2_PIN);
}

inline void
__attribute__((always_inline)) Switches_LED1_OFF(void) {
	GPIO_ResetBits(LED1_PORT, LED1_PIN);
}

inline void
__attribute__((always_inline)) Switches_LED2_OFF(void) {
	GPIO_ResetBits(LED2_PORT, LED2_PIN);
}

inline void
__attribute__((always_inline)) Switches_BUZZER_ON(void) {
	GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

inline void
__attribute__((always_inline)) Switches_BUZZER_OFF(void) {
	GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

inline uint8_t
__attribute__((always_inline)) Switches_Read_Reed(void) {
	return GPIO_ReadInputDataBit(REED_PORT, REED_PIN);
}

inline uint8_t
__attribute__((always_inline)) Switches_Read_S1(void) {
	return GPIO_ReadInputDataBit(S1_PORT, S1_PIN);
}

inline uint8_t
__attribute__((always_inline)) Switches_Read_S2(void) {
	return GPIO_ReadInputDataBit(S2_PORT, S2_PIN);
}

#endif /* SWITCHES_SWITCHES_H_ */
