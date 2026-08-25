/**
 * @file    delay.c
 * @brief   延时模块：SysTick 配置 + NOP 软件延时，提供 us/ms 级延时
 * @note    实际延时由 delay.h 中的 delay_1us()（约 72 条 NOP）循环完成；
 *          SysTick 仅用于提供 1ms 时基，中断函数当前为空，可扩展为系统调度节拍
 */
#include "delay.h"


//系统滴答定时器的中断函数
//1ms触发一次
void SysTick_Handler(void)
{
	/* 当前为空实现：SysTick 每 1ms 进入一次，
	   后续可在此累加系统 tick 计数，实现超时管理 / 任务调度 */
}




//系统定时器初始化
void Systick_Init(uint32_t load)
{
	if(SysTick_Config(load) == 1)   // SysTick_Config 返回 1 表示配置失败（如重装载值非法）
	{
		while(1);                   // 配置失败则死循环，便于调试时立即发现
	}
}


/**
 * @brief  微秒级延时（软件近似延时，非精确定时）
 * @param  time  延时微秒数
 * @note   通过循环执行 NOP 指令近似延时，精度受中断和编译器优化影响
 */
void Delay_us(uint32_t time)
{
	while(time--) {
		delay_1us();
	}
}
//1s = 1000ms  1ms=1000us
/**
 * @brief  毫秒级延时（软件近似延时，非精确定时）
 * @param  time  延时毫秒数
 * @note   内部将毫秒换算为微秒（x1000）后循环调用 delay_1us()
 */
void Delay_ms(uint32_t time)
{
	uint64_t t = time*1000;   // 1ms = 1000us
	while(t--) {
		delay_1us();
	}
}






