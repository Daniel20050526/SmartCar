/*******************************************************************************
 * 模块名称：延时模块头文件
 * 所属工程：智能车（STM32F103 + 板载 LED / 蜂鸣器 / 按键）
 * 模块职责：声明 SysTick 初始化与延时函数，定义约 1us 的 NOP 软件延时宏
 * 说明：本模块为自研代码，逻辑未改动，仅补充中文注释便于阅读维护
 ******************************************************************************/
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

/**
 * 函数名：delay_1us（宏）
 * 描述  ：约 1 微秒的软件延时
 * 输入  ：无
 * 输出  ：无
 * 说明  ：__NOP() 为 ARM 空操作指令，仅消耗 1 个时钟周期；STM32F103 主频
 *         72MHz 时 1 个时钟周期约 13.9ns，72 条 NOP 加上循环跳转开销合计
 *         约 1us（近似值，受编译器优化等级影响）
 */
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

void Systick_Init(uint32_t load);   // SysTick 初始化：load 为重装载值，72000 约对应 1ms
void Delay_us(uint32_t time);       // 微秒级软件延时
void Delay_ms(uint32_t time);       // 毫秒级软件延时
#endif
