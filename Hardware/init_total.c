#include "init_total.h"
#define ov5640 0

void init(void)
{
	
	delay_init(168);
	IO_A_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	
	// uart1_init(115200);
	// uart2_init(9600);    //Jy61p串口接收
	// uart6_init(115200);  //TOF测距串口接收
	
	LED_Init();					//初始化LED 
 	LCD_Init();					//LCD初始化  
 	KEY_Init();					//按键初始化 
	Jy61p_Init();       //Jy61p陀螺仪初始化  使用Jy61p_getdata();得到数据存储到fAngle[0-3]里
	Ms53l1m_Init();     //TOF测距初始化  使用Normal_DataGet();得到数据存储到distance变量里
	
	//以下为摄像头模块初始化
#if ov5640
	while(OV5640_Init())//初始化OV5640
	{
		LCD_ShowString(30,130,240,16,16,"OV5640 ERR");		
		delay_ms(200);
	  LCD_Fill(30,130,239,170,WHITE);		
		delay_ms(200);
		LEDR=!LEDR;
	}	LCD_ShowString(30,130,200,16,16,"OV5640 OK");  	  

	OV5640_RGB565_Mode();//设置为RGB565模式
#endif
	POINT_COLOR=RED;//设置字体为红色
}










