#ifndef _DCMI_H
#define _DCMI_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//DCMI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/14
//版本：V1.2
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//升级说明
//V1.1  20160302
//修改DCMI_DMA_Init和DCMI_Init函数对CR寄存器的操作方式,防止意外死机.
//V1.2  20160515
//修改DMA2_Stream1_IRQn的优先级，防止数据覆盖，导致图片末尾错位.
////////////////////////////////////////////////////////////////////////////////// 	

void My_DCMI_Init(void);
void DCMI_DMA_Init(uint32_t DMA_Memory0BaseAddr,uint16_t DMA_BufferSize,uint32_t DMA_MemoryDataSize,uint32_t DMA_MemoryInc);
void DCMI_Start(void);
void DCMI_Stop(void);

void DCMI_Set_Window(uint16_t sx,uint16_t sy,uint16_t width,uint16_t height);
void DCMI_CR_Set(uint8_t pclk,uint8_t hsync,uint8_t vsync);

#endif





















