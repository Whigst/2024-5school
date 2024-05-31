#include "encode_init.h"


void TIM3_Encode_init(u16 arr,u16 psc)//b4,b5    /正交编码
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTE时钟
	
	/************定时器3引脚初始化******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5; //PB4 PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //浮空输入，其实默认就是浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource4,GPIO_AF_TIM3);   //映射复用定时器
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3);
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM3, ENABLE); 	//使能定时器3
}
void TIM4_Encode_init(u16 arr,u16 psc)//b6,b7    /正交编码
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTE时钟
	
	/************定时器3引脚初始化******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //PB4 PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //浮空输入，其实默认就是浮空输入
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);   //映射复用定时器
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_TIM4);
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM4, ENABLE); 	//使能定时器3
}

void TIM8_Encode_init(u16 arr,u16 psc)//c6,c7    /正交编码
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能PORTE时钟
	
	/************定时器3引脚初始化******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7; //PB4 PB5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //浮空输入，其实默认就是浮空输入
	GPIO_Init(GPIOC,&GPIO_InitStructure); 
	  
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);   //映射复用定时器
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	TIM_EncoderInterfaceConfig(TIM8,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM8, ENABLE); 	//使能定时器3
}
void TIM2_Encode_init(u16 arr,u16 psc)//a1,a5   //正交编码
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	//TIM2时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟
	
	/************定时器2引脚初始化******************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_5; //PTA1 PTA5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //浮空输入，其实默认就是浮空输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);  
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM2);   //映射复用定时器
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM2);
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	
	TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM2, ENABLE); 	//使能定时器2
}

//
/*********************************************************
*函数功能：读取定时器编码器值
*
*********************************************************/
int TIM2_Encoder_Read(void)
{ 
	s16 data;
	data=(s16)(TIM_GetCounter(TIM2));
	return (int)data;
}
int TIM3_Encoder_Read(void)
{ 
	s16 data;
	data=(s16)(TIM_GetCounter(TIM3));
	return (int)data;
}
int TIM4_Encoder_Read(void)
{ 
	s16 data;
	data=(s16)(TIM_GetCounter(TIM4));
	return (int)data;
}
int TIM8_Encoder_Read(void)
{ 
	s16 data;
	data=(s16)(TIM_GetCounter(TIM8));
	return (int)data;
}

///
/*********************************************************
*函数功能：定时器编码器写入值
*参数：data:待写入的值
*********************************************************/
void TIM2_Encoder_Write(int data)
{
    TIM2->CNT = data;
}
void TIM3_Encoder_Write(int data)
{
    TIM3->CNT = data;
}
void TIM4_Encoder_Write(int data)
{
    TIM4->CNT = data;
}

void TIM8_Encoder_Write(int data)
{
    TIM8->CNT = data;
}
//
