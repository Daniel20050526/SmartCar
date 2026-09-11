/*******************************************************************************
 * 模块名称：板载外设驱动头文件
 * 所属工程：智能车（STM32F103 + 板载 LED / 蜂鸣器 / 按键）
 * 模块职责：定义 LED / 蜂鸣器 / 按键的引脚操作宏，并声明对应的驱动函数
 * 说明：本模块为自研代码，逻辑未改动，仅补充中文注释便于阅读维护
 ******************************************************************************/
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

void LED_Config(void);      // LED 引脚初始化（PC0/PC1/PC2 推挽输出）
void LED_Water(void);       // 流水灯：依次点亮 LED1 -> LED2 -> LED3
void BEEP_Config(void);     // 蜂鸣器引脚初始化（PA15，使用前需关闭 JTAG）
void KEY_Config(void);      // 按键引脚初始化（PA0 浮空输入）
void Get_Key_Value(void);   // 按键处理：按下翻转蜂鸣器（消抖 + 松手检测）
uint8_t Key_Scan(void);     // 按键扫描：返回 0-无动作 1-长按 2-短按

#endif




