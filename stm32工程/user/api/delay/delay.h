/**
 * @file    delay.h
 * @brief   延时模块头文件：声明 SysTick 初始化与延时函数，定义 1us 近似延时宏
 */
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

/**
 * @brief  约 1 微秒的软件延时
 * @note   原理：__NOP() 是 ARM 空操作指令，仅消耗 1 个时钟周期；
 *         STM32F103 主频 72MHz 时 1 个时钟周期约 13.9ns，
 *         72 条 NOP 加上循环跳转开销合计约 1us（近似值，受优化等级影响）
 */
//nop指令会让芯片暂停一个时钟周期
//一个时钟周期多久？
//时钟频率72mhz  
#define delay_1us()	{\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();\
__NOP();__NOP();\
}

void Systick_Init(uint32_t load);
void Delay_us(uint32_t time);
void Delay_ms(uint32_t time);
#endif
