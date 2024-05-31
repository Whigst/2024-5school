#include "io_init.h" 


void IO_A_Init(void)//a7
{
	GPIO_InitTypeDef  GPIO_InitStrcut;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
		
	GPIO_InitStrcut.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStrcut.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStrcut.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStrcut.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStrcut.GPIO_Pin=GPIO_Pin_6;
	
	GPIO_Init(GPIOA,&GPIO_InitStrcut);//GPIO初始化

}
void IO_B_Init(void)//b4,5
{
	GPIO_InitTypeDef  GPIO_InitStrcut;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
		
	GPIO_InitStrcut.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStrcut.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStrcut.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStrcut.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStrcut.GPIO_Pin=GPIO_Pin_4|GPIO_Pin_5;
	
	GPIO_Init(GPIOB,&GPIO_InitStrcut);//GPIO初始化

}
void IO_C_Init(void)//c2,3,4,5
{
	GPIO_InitTypeDef  GPIO_InitStrcut;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
		
	GPIO_InitStrcut.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_InitStrcut.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStrcut.GPIO_Speed=GPIO_Speed_100MHz;
	GPIO_InitStrcut.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStrcut.GPIO_Pin=GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_4|GPIO_Pin_5;
	
	GPIO_Init(GPIOC,&GPIO_InitStrcut);//GPIO初始化

}
//



















