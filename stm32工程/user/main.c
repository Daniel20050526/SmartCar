#include "main.h"

int main(void)
{
	JTAG_SW_Config();
	Systick_Init(72000);//1ms
	//≥ı ºªØ
	LED_Config();
	
	
	
	while(1)
	{
		LED_Water();
		Delay_ms(500);
	}
	
}
