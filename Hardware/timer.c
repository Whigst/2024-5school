#include "timer.h"

void TIM5_js_init(void)//(5000,84)Ϊ0.005s��5ms��//��a��b��T=a*b/84000000
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	
	//�¶˴����޸ļ�������Ϊ10ms
	TIM_TimeBaseInitStruct.TIM_Period=10000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;
	
	//�¶δ����޸ļ�������Ϊ5ms
//	TIM_TimeBaseInitStruct.TIM_Period=5000-1;
//	TIM_TimeBaseInitStruct.TIM_Prescaler=84-1;

	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM5_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0x01;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0x01;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM5,ENABLE);
}
//
void TIM5_IRQHandler(void)                          //��ʱ��5�жϷ�����
{
	if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET)      //����ж�
	{
		;		
	}
	TIM_ClearITPendingBit(TIM5,TIM_IT_Update);        //����жϱ�־λ
}
//

















