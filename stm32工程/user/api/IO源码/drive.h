/**
 * @file    drive.h
 * @brief   板载外设驱动头文件：LED / 蜂鸣器 / 按键的引脚宏定义与函数声明
 * @note    LED 低电平点亮（ResetBits=ON），蜂鸣器高电平响（SetBits=ON）
 */
#ifndef __DRIVE_H_
#define __DRIVE_H_

#include "stm32f10x.h"

/* =================== LED 控制宏（PC0/PC1/PC2，低电平点亮） =================== */
#define LED1_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_0)
#define LED1_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_0)

#define LED2_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_1)
#define LED2_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_1)

#define LED3_ON      GPIO_ResetBits(GPIOC,GPIO_Pin_2)
#define LED3_OFF     GPIO_SetBits(GPIOC,GPIO_Pin_2)

/* =================== 蜂鸣器控制宏（PA15，高电平响） =================== */
#define BEEP_TOGGLE  GPIOA->ODR ^=1<<15   // 直接异或翻转 ODR 位15：寄存器级输出取反
//高电平，蜂鸣器响
#define BEEP_ON  GPIO_SetBits(GPIOA,GPIO_Pin_15)
//低电平---蜂鸣器关闭
#define BEEP_OFF GPIO_ResetBits(GPIOA,GPIO_Pin_15)

/* =================== 按键读取宏（PA0，按下为低电平） =================== */
#define KEY1 GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)

void LED_Config(void);
void LED_Water(void);
void BEEP_Config(void);
void KEY_Config(void);
void Get_Key_Value(void);
uint8_t Key_Scan(void);

#endif




