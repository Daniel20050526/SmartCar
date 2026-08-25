/**
 * @file    spi.c
 * @brief   LCD 使用的 SPI2 通信驱动（ST7735S 屏）
 * @note    引脚映射：SCK=PB13、MOSI=PB15、CS=PA5、RES=PB12、DC=PA12、BL=PA4
 *          SPI2 挂载在 APB1 总线（36MHz），此处为软件控制 CS/DC/RES/BL 的 3 线+控制线方案
 */
#include "spi.h"

/**
 * @brief  SPI2 初始化：配置 SPI 引脚与 SPI 外设参数
 * @note   SCK/MOSI 复用推挽输出（由 SPI 外设驱动），CS/RES/DC/BL 普通推挽输出（软件控制）；
 *         通信参数：主机模式、8 位数据、CPOL=High/CPHA=2Edge（模式3）、软件 NSS、MSB 先行
 */
void SPI2_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(LCD_CS_GPIO_CLK | LCD_SPI_MOSI_GPIO_CLK |
                         LCD_SPI_SCK_GPIO_CLK | LCD_RES_GPIO_CLK | 
												 LCD_DC_GPIO_CLK | LCD_BL_GPIO_CLK, ENABLE);
  LCD_SPI_CLK_CMD(LCD_SPI_CLK, ENABLE);      // 开启 SPI2 外设时钟（APB1）
  
  /*!< Configure LCD_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出：引脚由 SPI 外设接管
  GPIO_Init(LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure LCD_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
  GPIO_Init(LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure LCD_CS_PIN pin: LCD Card CS pin */
  GPIO_InitStructure.GPIO_Pin = LCD_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 片选：软件拉低选中 LCD
  GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStructure);
	
	 /*!< Configure LCD_RES_PIN pin: LCD Card RES pin */
  GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN;
  GPIO_Init(LCD_RES_GPIO_PORT, &GPIO_InitStructure); // 复位脚

	 /*!< Configure LCD_DC_PIN pin: LCD Card DC pin */
  GPIO_InitStructure.GPIO_Pin = LCD_DC_PIN;
  GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStructure);  // 数据/命令选择脚

  GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;
  GPIO_Init(LCD_BL_GPIO_PORT, &GPIO_InitStructure);  // 背光控制脚

	LCD_CS_CMD(1);   // 初始片选无效（高电平），防止误选中
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   // 双线全双工
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        // 主机模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    // 8 位数据帧
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                          // 空闲时钟为高
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                         // 第二个边沿采样（模式3）
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            // 软件管理 NSS（片选由 GPIO 控制）
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;   // 2 分频：SPI 时钟 = 36MHz/2 = 18MHz
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   // 高位先发
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(LCD_SPI, &SPI_InitStructure);   // 写入 SPI 配置寄存器
  SPI_Cmd(LCD_SPI, ENABLE);                // 使能 SPI2 外设
}




//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));  // 参数有效性检查
	SPI2->CR1&=0XFFC7;    // 清除 CR1 寄存器 BR[2:0] 位（波特率分频系数）
	SPI2->CR1|=SPI_BaudRatePrescaler;	//设置SPI2速度（写入新的分频值，SPI 时钟 = APB1(36MHz) / 分频数）
	SPI_Cmd(SPI2,ENABLE); // 使能 SPI2

} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI2_ReadWriteByte(uint8_t TxData)
{		
	uint8_t retry=0;				 	
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位（TXE 置位才可写入）
	{
		retry++;
		if(retry>200)	return 0;   // 超时保护：防止 SPI 异常导致死循环
	}			  
	SPI_I2S_SendData(LCD_SPI, TxData); //通过外设SPIx发送一个数据（写入 DR 寄存器，同时启动时钟产生）
	retry=0;
	while (SPI_I2S_GetFlagStatus(LCD_SPI, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位（RXNE 置位表示收到数据）
	{
		retry++;
		if(retry>200)	return 0;   // 超时保护
	}	  						    
	return SPI_I2S_ReceiveData(LCD_SPI); //返回通过SPIx最近接收的数据（读取 DR 寄存器）					    
}



