#ifndef _DCMI_H
#define _DCMI_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//DCMI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/14
//�汾��V1.2
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//����˵��
//V1.1  20160302
//�޸�DCMI_DMA_Init��DCMI_Init������CR�Ĵ����Ĳ�����ʽ,��ֹ��������.
//V1.2  20160515
//�޸�DMA2_Stream1_IRQn�����ȼ�����ֹ���ݸ��ǣ�����ͼƬĩβ��λ.
////////////////////////////////////////////////////////////////////////////////// 	

void My_DCMI_Init(void);
void DCMI_DMA_Init(uint32_t DMA_Memory0BaseAddr,uint16_t DMA_BufferSize,uint32_t DMA_MemoryDataSize,uint32_t DMA_MemoryInc);
void DCMI_Start(void);
void DCMI_Stop(void);

void DCMI_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void DCMI_CR_Set(uint8_t pclk,uint8_t hsync,uint8_t vsync);

#endif





















