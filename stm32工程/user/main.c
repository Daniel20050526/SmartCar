/**
 * @file    main.c
 * @brief   智能车项目主程序入口
 * @note    当前演示版本实现：3 路 LED 流水灯循环点亮。
 *          蜂鸣器/按键驱动已在 drive.c 中实现，如需启用可在主循环中调用。
 */
#include "main.h"

int main(void)
{
	JTAG_SW_Config();       // 关闭 JTAG，释放 PA15/PB3/PB4 给普通 IO 使用
	Systick_Init(72000);    // 初始化 SysTick：重装载值 72000，72MHz 时钟下约 1ms 触发一次
	//初始化
	LED_Config();           // 初始化 LED 引脚（PC0/PC1/PC2 推挽输出）
	
	
	
	while(1)                // 主循环：持续执行流水灯逻辑
	{
		LED_Water();        // 流水灯：依次点亮 LED1 -> LED2 -> LED3，循环往复
		Delay_ms(500);      // 软件延时 500ms，控制流水灯切换节奏
	}
	
}
