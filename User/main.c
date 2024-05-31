#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
// #include "init_total.h"
#include "lcd.h"

int main(void)
{
	// init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168);
	uart1_init(115200);
	// Uart_Init(115200);
	LCD_Init();
	// uart1_init(115200);
	POINT_COLOR = RED;
	// 
	int x = 0;
	unsigned char a[10] = {0};

 	while(1)
	{	
		sprintf((char *)a, "%d", x);
		// GPIO_ReSetBits(GPIOA, GPIO_Pin_6);
		//LED_Init();
//		GPIO_ResetBits(GPIOF, GPIO_Pin_9);
//		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		LCD_Clear(WHITE);
		POINT_COLOR = RED;
		// LCD_ShowChar(30, 40, 'a', 16, 0);
		LCD_ShowString(30,40,210,24,24,a);
		delay_ms(1000);
		x++;
		// LCD_ShowNum(50, 50, 666, 3, 16);
	}
 
}










































