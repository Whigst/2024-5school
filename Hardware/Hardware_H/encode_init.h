#ifndef __ENCODE_INIT_H
#define __ENCODE_INIT_H

#include "sys.h"

void TIM2_Encode_init(u16 arr,u16 psc);//a1,a5 
void TIM3_Encode_init(u16 arr,u16 psc);//b4,b5
void TIM4_Encode_init(u16 arr,u16 psc);//b6,b7    
void TIM8_Encode_init(u16 arr,u16 psc);//c6,c7    //Õý½»±àÂë(65535,0)

int TIM2_Encoder_Read(void);
int TIM3_Encoder_Read(void);
int TIM4_Encoder_Read(void);
int TIM8_Encoder_Read(void);

////////////////
void TIM2_Encoder_Write(int data);
void TIM3_Encoder_Write(int data);
void TIM4_Encoder_Write(int data);
void TIM8_Encoder_Write(int data);



#endif
















