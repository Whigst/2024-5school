#ifndef _OV5640_H
#define _OV5640_H
#include "sys.h"
#include "sccb.h"
#include "dcmi.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//OV5640 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/4/30
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//*******************************************************************************
//修改信息w
//20160226 V1.1
//新增OV5640_Exposure函数.
////////////////////////////////////////////////////////////////////////////////// 

#define OV5640_PWDN  	PGout(9)			//POWER DOWN控制信号
#define OV5640_RST  	PGout(15)			//复位控制信号 
////////////////////////////////////////////////////////////////////////////////// 
#define OV5640_ID               0X5640  	//OV5640的芯片ID
 

#define OV5640_ADDR        		0X78		//OV5640的IIC地址
 
//OV5640相关寄存器定义  
#define OV5640_CHIPIDH          0X300A  	//OV5640芯片ID高字节
#define OV5640_CHIPIDL          0X300B  	//OV5640芯片ID低字节
 
extern uint8_t ovx_mode;
extern const uint16_t jpeg_img_size_tbl[][2];

void jpeg_data_process(void);//JPEG数据处理函数			//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.		
void rgb565_test(void);//RGB565测试			//RGB数据直接显示在LCD上面
void jpeg_test(void);//JPEG测试			//JPEG数据,通过串口2发送给电脑.



uint8_t OV5640_WR_Reg(uint16_t reg,uint8_t data);
uint8_t OV5640_RD_Reg(uint16_t reg);
void OV5640_PWDN_Set(uint8_t sta);
uint8_t OV5640_Init(void);  
void OV5640_JPEG_Mode(void);
void OV5640_RGB565_Mode(void);
void OV5640_Exposure(uint8_t exposure);
void OV5640_Light_Mode(uint8_t mode);
void OV5640_Color_Saturation(uint8_t sat);
void OV5640_Brightness(uint8_t bright);
void OV5640_Contrast(uint8_t contrast);
void OV5640_Sharpness(uint8_t sharp);
void OV5640_Special_Effects(uint8_t eft);
void OV5640_Test_Pattern(uint8_t mode);
void OV5640_Flash_Ctrl(uint8_t sw);
void ov5640_test(void);
uint8_t OV5640_OutSize_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height);
uint8_t OV5640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height); 
uint8_t OV5640_Focus_Init(void);
uint8_t OV5640_Focus_Single(void);
uint8_t OV5640_Focus_Constant(void);
#endif





















