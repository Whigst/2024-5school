#ifndef _OV5640_H
#define _OV5640_H
#include "sys.h"
#include "sccb.h"
#include "dcmi.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//OV5640 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/4/30
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//*******************************************************************************
//�޸���Ϣw
//20160226 V1.1
//����OV5640_Exposure����.
////////////////////////////////////////////////////////////////////////////////// 

#define OV5640_PWDN  	PGout(9)			//POWER DOWN�����ź�
#define OV5640_RST  	PGout(15)			//��λ�����ź� 
////////////////////////////////////////////////////////////////////////////////// 
#define OV5640_ID               0X5640  	//OV5640��оƬID
 

#define OV5640_ADDR        		0X78		//OV5640��IIC��ַ
 
//OV5640��ؼĴ�������  
#define OV5640_CHIPIDH          0X300A  	//OV5640оƬID���ֽ�
#define OV5640_CHIPIDL          0X300B  	//OV5640оƬID���ֽ�
 
extern uint8_t ovx_mode;
extern const uint16_t jpeg_img_size_tbl[][2];

void jpeg_data_process(void);//JPEG���ݴ�����			//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.		
void rgb565_test(void);//RGB565����			//RGB����ֱ����ʾ��LCD����
void jpeg_test(void);//JPEG����			//JPEG����,ͨ������2���͸�����.



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





















