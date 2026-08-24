#ifndef __DRIVE_H_
#define __DRIVE_H_

#include "stm32f10x.h"

#define LED1_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define LED1_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_0)

#define LED2_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define LED2_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_1)

#define LED3_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define LED3_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_2)

#define BEEP_TOGGLE  GPIOA->ODR ^=1<<15
//高电平，蜂鸣器响
#define BEEP_ON  GPIO_SetBits(GPIOA,GPIO_Pin_15)
//低电平---蜂鸣器关闭
#define BEEP_OFF GPIO_ResetBits(GPIOA,GPIO_Pin_15)

#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

void LED_Config(void);
void LED_Water(void);
void BEEP_Config(void);
void KEY_Config(void);
void Get_Key_Value(void);
uint8_t Key_Scan(void);

#endif




