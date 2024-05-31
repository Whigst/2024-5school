#include "sys.h"
#include "ov5640.h"
#include "ov5640cfg.h"
#include "ov5640af.h"
#include "timer.h"	  
#include "delay.h"
#include "usart.h"			 
#include "sccb.h"	 
#include "lcd.h"
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
//�޸���Ϣ
//20160226 V1.2
//����OV5640_Exposure����.
//20160515 V1.2
//1���޸�ov5640_uxga_init_reg_tbl��������Ϊ��ov5640_init_reg_tbl
//2���޸����ע��
////////////////////////////////////////////////////////////////////////////////// 

//JPEG�ߴ�֧���б�
const uint16_t jpeg_img_size_tbl[][2]=
{
	160,120,	//QQVGA 
	320,240,	//QVGA  
	640,480,	//VGA
	800,600,	//SVGA
	1024,768,	//XGA
	1280,800,	//WXGA 
	1440,900,	//WXGA+
	1280,1024,	//SXGA
	1600,1200,	//UXGA	
	1920,1080,	//1080P
	2048,1536,	//QXGA
	2592,1944,	//500W 
};

//RGB565����
//RGB����ֱ����ʾ��LCD����
const uint8_t*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7����Ч
const uint8_t*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEGͼƬ
void rgb565_test(void)
{ 
	uint8_t key;
	uint8_t effect=0,contrast=2,fac;
	uint8_t scale=1;		//Ĭ����ȫ�ߴ�����
	uint8_t msgbuf[15];	//��Ϣ������ 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode");
	
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,130,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,150,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,170,200,16,16,"KEY_UP:FullSize/Scale");	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/ȫ�ߴ�����
	
		//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ 
	OV5640_Focus_Init();
	
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((uint32_t)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA����  
 	OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height); 
	DCMI_Start(); 		//��������
	while(1)
	{ 
		key=KEY_Scan(0); 
		if(key)
		{ 
			if(key!=KEY1_PRES)DCMI_Stop(); //��KEY1����,ֹͣ��ʾ
			switch(key)
			{				    
				case KEY0_PRES:	//�Աȶ�����
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					break;
				case KEY1_PRES:	//ִ��һ���Զ��Խ�
					OV5640_Focus_Single();
					break;
				case KEY2_PRES:	//��Ч����				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//������Ч
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					break;
				case WKUP_PRES:	//1:1�ߴ�(��ʾ��ʵ�ߴ�)/����	    
					scale=!scale;  
					if(scale==0)
					{
						fac=800/lcddev.height;//�õ���������
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*lcddev.height)/2,lcddev.width,lcddev.height); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height);
 						sprintf((char*)msgbuf,"Scale");
					}
					break;
			}
			if(key!=KEY1_PRES)	//��KEY1����
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 
				DCMI_Start();	//���¿�ʼ����
			}
		} 
		delay_ms(10);		
	}       
} 
//

//����JPEG����
//���ɼ���һ֡JPEG���ݺ�,���ô˺���,�л�JPEG BUF.��ʼ��һ֡�ɼ�.
uint8_t ovx_mode=0;							//bit0:0,RGB565ģʽ;1,JPEGģʽ 

#define jpeg_buf_size 31*1024  			//����JPEG���ݻ���jpeg_buf�Ĵ�С(*4�ֽ�)
__align(4) uint32_t jpeg_buf[jpeg_buf_size];	//JPEG���ݻ���buf
volatile uint32_t jpeg_data_len=0; 			//buf�е�JPEG��Ч���ݳ��� 
volatile uint8_t jpeg_data_ok=0;				//JPEG���ݲɼ���ɱ�־ 
										//0,����û�вɼ���;
										//1,���ݲɼ�����,���ǻ�û����;
										//2,�����Ѿ����������,���Կ�ʼ��һ֡����
//JPEG�ߴ�֧���б�
void jpeg_data_process(void)//JPEG���ݴ�����
{
	if(ovx_mode&0X01)	//ֻ����JPEG��ʽ��,����Ҫ������.
	{
		if(jpeg_data_ok==0)	//jpeg���ݻ�δ�ɼ���?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//ֹͣ��ǰ���� 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//�ȴ�DMA2_Stream1������  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//�õ��˴����ݴ���ĳ���
				
			jpeg_data_ok=1; 				//���JPEG���ݲɼ��갴��,�ȴ�������������
		}
		if(jpeg_data_ok==2)	//��һ�ε�jpeg�����Ѿ���������
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//���䳤��Ϊjpeg_buf_size*4�ֽ�
			DMA_Cmd(DMA2_Stream1, ENABLE);			//���´���
			jpeg_data_ok=0;						//�������δ�ɼ�
		}
	}else
	{
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare();		//��ʼд��GRAM
	}	
} 	



//JPEG����
//JPEG����,ͨ������2���͸�����.
void jpeg_test(void)
{
	uint32_t i,jpgstart,jpglen; 
	uint8_t *p;
	uint8_t key,headok=0;
	uint8_t effect=0,contrast=2;
	uint8_t size=2;			//Ĭ����QVGA 640*480�ߴ�
	uint8_t msgbuf[15];		//��Ϣ������ 
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//�Աȶ�
	LCD_ShowString(30,120,200,16,16,"KEY1:Saturation"); 		//ɫ�ʱ��Ͷ�
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//��Ч 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//�ֱ������� 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//��ʾ��ǰJPEG�ֱ���
	
 	//�Զ��Խ���ʼ��
	OV5640_RGB565_Mode();	//RGB565ģʽ 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEGģʽ
	
	OV5640_Light_Mode(0);	//�Զ�ģʽ
	OV5640_Color_Saturation(3);//ɫ�ʱ��Ͷ�0
	OV5640_Brightness(4);	//����0
	OV5640_Contrast(3);		//�Աȶ�0
	OV5640_Sharpness(33);	//�Զ����
	OV5640_Focus_Constant();//���������Խ�
	
	My_DCMI_Init();			//DCMI����
	DCMI_DMA_Init((uint32_t)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA����   
	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ� 
	DCMI_Start(); 		//��������
	while(1)
	{
		if(jpeg_data_ok==1)	//�Ѿ��ɼ���һ֡ͼ����
		{  
			p=(uint8_t*)jpeg_buf;
			printf("jpeg_data_len:%d\r\n",jpeg_data_len*4);//��ӡ֡��
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //��ʾ���ڴ������� 
			jpglen=0;	//����jpg�ļ���СΪ0
			headok=0;	//���jpgͷ���
			for(i=0;i<jpeg_data_len*4;i++)//����0XFF,0XD8��0XFF,0XD9,��ȡjpg�ļ���С
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//�ҵ�FF D8
				{
					jpgstart=i;
					headok=1;	//����ҵ�jpgͷ(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//�ҵ�ͷ�Ժ�,����FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			if(jpglen)	//������jpeg���� 
			{
				p+=jpgstart;			//ƫ�Ƶ�0XFF,0XD8�� 
				for(i=0;i<jpglen;i++)	//��������jpg�ļ�
				{
					while((USART2->SR&0X40)==0);	//ѭ������,ֱ���������   
					USART2->DR=p[i];  
					key=KEY_Scan(0); 
					if(key)break;
				}  
			}
			if(key)	//�а�������,��Ҫ����
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//��ʾ�˳����ݴ���
				switch(key)
				{				    
					case KEY0_PRES:	//�Աȶ�����
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
						break; 
					case KEY1_PRES:	//ִ��һ���Զ��Խ�
						OV5640_Focus_Single();
						break;
					case KEY2_PRES:	//��Ч����				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//������Ч
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG����ߴ�����   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//��������ߴ�  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//��ʾ��ʾ����
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//��ʾ����������� 
			jpeg_data_ok=2;	//���jpeg���ݴ�������,������DMAȥ�ɼ���һ֡��.
		}		
	}     
} 

//OV5640д�Ĵ���
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t OV5640_WR_Reg(uint16_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start(); 					//����SCCB����
	if(SCCB_WR_Byte(OV5640_ADDR))res=1;	//д����ID	  
   	if(SCCB_WR_Byte(reg>>8))res=1;	//д�Ĵ�����8λ��ַ
   	if(SCCB_WR_Byte(reg))res=1;		//д�Ĵ�����8λ��ַ		  
   	if(SCCB_WR_Byte(data))res=1; 	//д����	 
  	SCCB_Stop();	  
  	return	res;
}
//OV5640���Ĵ���
//����ֵ:�����ļĴ���ֵ
uint8_t OV5640_RD_Reg(uint16_t reg)
{
	uint8_t val=0;
	SCCB_Start(); 				//����SCCB����
	SCCB_WR_Byte(OV5640_ADDR);	//д����ID
   	SCCB_WR_Byte(reg>>8);	    //д�Ĵ�����8λ��ַ   
  	SCCB_WR_Byte(reg);			//д�Ĵ�����8λ��ַ	  
 	SCCB_Stop();   
 	//���üĴ�����ַ�󣬲��Ƕ�
	SCCB_Start();
	SCCB_WR_Byte(OV5640_ADDR|0X01);//���Ͷ�����	  
   	val=SCCB_RD_Byte();		 	//��ȡ����
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}
//��ʼ��OV5640 
//����ֵ:0,�ɹ�
//    ����,�������
uint8_t OV5640_Init(void)
{ 
	uint16_t i=0;
	uint16_t reg;
	//����IO     	   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  //GPIOG9,15��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_15;//PG9,15�������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��
	
	OV5640_RST=0;			//����������OV5640��RST��,���ϵ�
	delay_ms(20); 
	OV5640_PWDN=0;		//POWER ON
	delay_ms(5);  
	OV5640_RST=1;			//������λ 
	delay_ms(20);      
  	SCCB_Init();			//��ʼ��SCCB ��IO�� 
	delay_ms(5); 
	reg=OV5640_RD_Reg(OV5640_CHIPIDH);	//��ȡID �߰�λ
	reg<<=8;
	reg|=OV5640_RD_Reg(OV5640_CHIPIDL);	//��ȡID �Ͱ�λ
	if(reg!=OV5640_ID)
	{
		printf("ID:%d\r\n",reg);
		return 1;
	}  
	OV5640_WR_Reg(0x3103,0X11);	//system clock from pad, bit[1]
	OV5640_WR_Reg(0X3008,0X82);	//��λ
	delay_ms(10);
 	//��ʼ�� OV5640
	for(i=0;i<sizeof(ov5640_init_reg_tbl)/4;i++)
	{
	   	OV5640_WR_Reg(ov5640_init_reg_tbl[i][0],ov5640_init_reg_tbl[i][1]);
 	}    
	//���������Ƿ�����
	OV5640_Flash_Ctrl(1);//�������
	delay_ms(50);
	OV5640_Flash_Ctrl(0);//�ر������
  	return 0x00; 	//ok
} 
//OV5640�л�ΪJPEGģʽ
void OV5640_JPEG_Mode(void) 
{
	uint16_t i=0; 
	//����:���JPEG����
	for(i=0;i<(sizeof(OV5640_jpeg_reg_tbl)/4);i++)
	{
		OV5640_WR_Reg(OV5640_jpeg_reg_tbl[i][0],OV5640_jpeg_reg_tbl[i][1]);  
	}  
}
//OV5640�л�ΪRGB565ģʽ
void OV5640_RGB565_Mode(void) 
{
	uint16_t i=0;
	//����:RGB565���
	for(i=0;i<(sizeof(ov5640_rgb565_reg_tbl)/4);i++)
	{
		OV5640_WR_Reg(ov5640_rgb565_reg_tbl[i][0],ov5640_rgb565_reg_tbl[i][1]); 
	} 
} 
//EV�عⲹ�����ò�����֧��7���ȼ�
const static uint8_t OV5640_EXPOSURE_TBL[7][6]=
{
    0x10,0x08,0x10,0x08,0x20,0x10,//-3  
    0x20,0x18,0x41,0x20,0x18,0x10,//-
    0x30,0x28,0x61,0x30,0x28,0x10,//-1 
    0x38,0x30,0x61,0x38,0x30,0x10,//0  
    0x40,0x38,0x71,0x40,0x38,0x10,//+1 
    0x50,0x48,0x90,0x50,0x48,0x20,//+2   
    0x60,0x58,0xa0,0x60,0x58,0x20,//+3    
};

//EV�عⲹ��
//exposure:0~6,������-3~3. 
void OV5640_Exposure(uint8_t exposure)
{
    OV5640_WR_Reg(0x3212,0x03);	//start group 3
    OV5640_WR_Reg(0x3a0f,OV5640_EXPOSURE_TBL[exposure][0]); 
	OV5640_WR_Reg(0x3a10,OV5640_EXPOSURE_TBL[exposure][1]); 
    OV5640_WR_Reg(0x3a1b,OV5640_EXPOSURE_TBL[exposure][2]); 
	OV5640_WR_Reg(0x3a1e,OV5640_EXPOSURE_TBL[exposure][3]); 
    OV5640_WR_Reg(0x3a11,OV5640_EXPOSURE_TBL[exposure][4]); 
    OV5640_WR_Reg(0x3a1f,OV5640_EXPOSURE_TBL[exposure][5]); 
    OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3
}

//�ƹ�ģʽ������,֧��5��ģʽ
const static uint8_t OV5640_LIGHTMODE_TBL[5][7]=
{ 
	0x04,0X00,0X04,0X00,0X04,0X00,0X00,//Auto,�Զ� 
	0x06,0X1C,0X04,0X00,0X04,0XF3,0X01,//Sunny,�չ�
	0x05,0X48,0X04,0X00,0X07,0XCF,0X01,//Office,�칫��
	0x06,0X48,0X04,0X00,0X04,0XD3,0X01,//Cloudy,���� 
	0x04,0X10,0X04,0X00,0X08,0X40,0X01,//Home,����
}; 
//��ƽ������
//0:�Զ�
//1:�չ�sunny
//2,�칫��office
//3,����cloudy
//4,����home
void OV5640_Light_Mode(uint8_t mode)
{
	uint8_t i;
	OV5640_WR_Reg(0x3212,0x03);	//start group 3
	for(i=0;i<7;i++)OV5640_WR_Reg(0x3400+i,OV5640_LIGHTMODE_TBL[mode][i]);//���ñ��Ͷ� 
	OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3	
}
//ɫ�ʱ��Ͷ����ò�����,֧��7���ȼ�
const static uint8_t OV5640_SATURATION_TBL[7][6]=
{ 
	0X0C,0x30,0X3D,0X3E,0X3D,0X01,//-3 
	0X10,0x3D,0X4D,0X4E,0X4D,0X01,//-2	
	0X15,0x52,0X66,0X68,0X66,0X02,//-1	
	0X1A,0x66,0X80,0X82,0X80,0X02,//+0	
	0X1F,0x7A,0X9A,0X9C,0X9A,0X02,//+1	
	0X24,0x8F,0XB3,0XB6,0XB3,0X03,//+2
 	0X2B,0xAB,0XD6,0XDA,0XD6,0X04,//+3
}; 
//ɫ������
//sat:0~6,�����Ͷ�-3~3. 
void OV5640_Color_Saturation(uint8_t sat)
{ 
	uint8_t i;
	OV5640_WR_Reg(0x3212,0x03);	//start group 3
	OV5640_WR_Reg(0x5381,0x1c);
	OV5640_WR_Reg(0x5382,0x5a);
	OV5640_WR_Reg(0x5383,0x06);
	for(i=0;i<6;i++)OV5640_WR_Reg(0x5384+i,OV5640_SATURATION_TBL[sat][i]);//���ñ��Ͷ� 
	OV5640_WR_Reg(0x538b, 0x98);
	OV5640_WR_Reg(0x538a, 0x01);
	OV5640_WR_Reg(0x3212, 0x13); //end group 3
	OV5640_WR_Reg(0x3212, 0xa3); //launch group 3	
}
//��������
//bright:0~8,��������-4~4.
void OV5640_Brightness(uint8_t bright)
{
	uint8_t brtval;
	if(bright<4)brtval=4-bright;
	else brtval=bright-4;
	OV5640_WR_Reg(0x3212,0x03);	//start group 3
	OV5640_WR_Reg(0x5587,brtval<<4);
	if(bright<4)OV5640_WR_Reg(0x5588,0x09);
	else OV5640_WR_Reg(0x5588,0x01);
	OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3
}
//�Աȶ�����
//contrast:0~6,��������-3~3.
void OV5640_Contrast(uint8_t contrast)
{
	uint8_t reg0val=0X00;//contrast=3,Ĭ�϶Աȶ�
	uint8_t reg1val=0X20;
  	switch(contrast)
	{
		case 0://-3
			reg1val=reg0val=0X14;	 	 
			break;	
		case 1://-2
			reg1val=reg0val=0X18; 	 
			break;	
		case 2://-1
			reg1val=reg0val=0X1C;	 
			break;	
		case 4://1
			reg0val=0X10;	 	 
			reg1val=0X24;	 	 
			break;	
		case 5://2
			reg0val=0X18;	 	 
			reg1val=0X28;	 	 
			break;	
		case 6://3
			reg0val=0X1C;	 	 
			reg1val=0X2C;	 	 
			break;	
	} 
	OV5640_WR_Reg(0x3212,0x03); //start group 3
	OV5640_WR_Reg(0x5585,reg0val);
	OV5640_WR_Reg(0x5586,reg1val); 
	OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3
}
//�������
//sharp:0~33,0,�ر�;33,auto;����ֵ,��ȷ�Χ.
void OV5640_Sharpness(uint8_t sharp)
{
	if(sharp<33)//�������ֵ
	{
		OV5640_WR_Reg(0x5308,0x65);
		OV5640_WR_Reg(0x5302,sharp);
	}else	//�Զ����
	{
		OV5640_WR_Reg(0x5308,0x25);
		OV5640_WR_Reg(0x5300,0x08);
		OV5640_WR_Reg(0x5301,0x30);
		OV5640_WR_Reg(0x5302,0x10);
		OV5640_WR_Reg(0x5303,0x00);
		OV5640_WR_Reg(0x5309,0x08);
		OV5640_WR_Reg(0x530a,0x30);
		OV5640_WR_Reg(0x530b,0x04);
		OV5640_WR_Reg(0x530c,0x06);
	}
	
}
//��Ч���ò�����,֧��7����Ч
const static uint8_t OV5640_EFFECTS_TBL[7][3]=
{ 
	0X06,0x40,0X10,//���� 
	0X1E,0xA0,0X40,//��ɫ
	0X1E,0x80,0XC0,//ůɫ
	0X1E,0x80,0X80,//�ڰ�
	0X1E,0x40,0XA0,//���� 
	0X40,0x40,0X10,//��ɫ
	0X1E,0x60,0X60,//ƫ��
}; 
//��Ч����
//0:����    
//1,��ɫ
//2,ůɫ   
//3,�ڰ�
//4,ƫ��
//5,��ɫ
//6,ƫ��	    
void OV5640_Special_Effects(uint8_t eft)
{ 
	OV5640_WR_Reg(0x3212,0x03); //start group 3
	OV5640_WR_Reg(0x5580,OV5640_EFFECTS_TBL[eft][0]);
	OV5640_WR_Reg(0x5583,OV5640_EFFECTS_TBL[eft][1]);// sat U
	OV5640_WR_Reg(0x5584,OV5640_EFFECTS_TBL[eft][2]);// sat V
	OV5640_WR_Reg(0x5003,0x08);
	OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3
}
//��������
//mode:0,�ر�
//     1,���� 
//     2,ɫ��
void OV5640_Test_Pattern(uint8_t mode)
{
	if(mode==0)OV5640_WR_Reg(0X503D,0X00);
	else if(mode==1)OV5640_WR_Reg(0X503D,0X80);
	else if(mode==2)OV5640_WR_Reg(0X503D,0X82);
} 
//����ƿ���
//mode:0,�ر�
//     1,�� 
void OV5640_Flash_Ctrl(uint8_t sw)
{
	OV5640_WR_Reg(0x3016,0X02);
	OV5640_WR_Reg(0x301C,0X02); 
	if(sw)OV5640_WR_Reg(0X3019,0X02); 
	else OV5640_WR_Reg(0X3019,0X00);
} 
//����ͼ�������С
//OV5640���ͼ��Ĵ�С(�ֱ���),��ȫ�ɸú���ȷ��
//offx,offy,Ϊ���ͼ����OV5640_ImageWin_Set�趨����(���賤��Ϊxsize��ysize)�ϵ�ƫ��
//���ڿ�����scale����,���������ͼ�񴰿�Ϊ:xsize-2*offx,ysize-2*offy
//width,height:ʵ�����ͼ��Ŀ�Ⱥ͸߶�
//ʵ�����(width,height),����xsize-2*offx,ysize-2*offy�Ļ����Ͻ������Ŵ���.
//һ������offx��offy��ֵΪ16��4,��СҲ�ǿ���,����Ĭ����16��4 
//����ֵ:0,���óɹ�
//    ����,����ʧ��
uint8_t OV5640_OutSize_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{ 
    OV5640_WR_Reg(0X3212,0X03);  	//��ʼ��3
    //�������þ���ʵ������ߴ�(������)
    OV5640_WR_Reg(0x3808,width>>8);	//����ʵ�������ȸ��ֽ�
    OV5640_WR_Reg(0x3809,width&0xff);//����ʵ�������ȵ��ֽ�  
    OV5640_WR_Reg(0x380a,height>>8);//����ʵ������߶ȸ��ֽ�
    OV5640_WR_Reg(0x380b,height&0xff);//����ʵ������߶ȵ��ֽ�
	//�������þ�������ߴ���ISP�����ȡͼ��Χ
	//��Χ:xsize-2*offx,ysize-2*offy
    OV5640_WR_Reg(0x3810,offx>>8);	//����X offset���ֽ�
    OV5640_WR_Reg(0x3811,offx&0xff);//����X offset���ֽ�
	
    OV5640_WR_Reg(0x3812,offy>>8);	//����Y offset���ֽ�
    OV5640_WR_Reg(0x3813,offy&0xff);//����Y offset���ֽ�
	
    OV5640_WR_Reg(0X3212,0X13);		//������3
    OV5640_WR_Reg(0X3212,0Xa3);		//������3����
	return 0; 
}

//����ͼ�񿪴���С(ISP��С),�Ǳ�Ҫ,һ��������ô˺���
//���������������濪��(���2592*1944),����OV5640_OutSize_Set�����
//ע��:�������Ŀ�Ⱥ͸߶�,������ڵ���OV5640_OutSize_Set�����Ŀ�Ⱥ͸߶�
//     OV5640_OutSize_Set���õĿ�Ⱥ͸߶�,���ݱ��������õĿ�Ⱥ͸߶�,��DSP
//     �Զ��������ű���,������ⲿ�豸.
//width,height:���(��Ӧ:horizontal)�͸߶�(��Ӧ:vertical)  
//����ֵ:0,���óɹ�
//    ����,����ʧ��
uint8_t OV5640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t xst,yst,xend,yend;
	xst=offx;
	yst=offy;
	xend=offx+width-1;
	yend=offy+height-1;  
    OV5640_WR_Reg(0X3212,0X03);		//��ʼ��3
	OV5640_WR_Reg(0X3800,xst>>8);	
	OV5640_WR_Reg(0X3801,xst&0XFF);	
	OV5640_WR_Reg(0X3802,yst>>8);	
	OV5640_WR_Reg(0X3803,yst&0XFF);	
	OV5640_WR_Reg(0X3804,xend>>8);	
	OV5640_WR_Reg(0X3805,xend&0XFF);
	OV5640_WR_Reg(0X3806,yend>>8);	
	OV5640_WR_Reg(0X3807,yend&0XFF);
    OV5640_WR_Reg(0X3212,0X13);		//������3
    OV5640_WR_Reg(0X3212,0Xa3);		//������3����	 
	return 0;
}   
//��ʼ���Զ��Խ�
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t OV5640_Focus_Init(void)
{ 
	uint16_t i; 
	uint16_t addr=0x8000;
	uint8_t state=0x8F;
	OV5640_WR_Reg(0x3000, 0x20);			//reset MCU	 
	for(i=0;i<sizeof(OV5640_AF_Config);i++) //������������
	{
		OV5640_WR_Reg(addr,OV5640_AF_Config[i]);
		addr++;
	}  
	OV5640_WR_Reg(0x3022,0x00);
	OV5640_WR_Reg(0x3023,0x00);
	OV5640_WR_Reg(0x3024,0x00);
	OV5640_WR_Reg(0x3025,0x00);
	OV5640_WR_Reg(0x3026,0x00);
	OV5640_WR_Reg(0x3027,0x00);
	OV5640_WR_Reg(0x3028,0x00);
	OV5640_WR_Reg(0x3029,0x7f);
	OV5640_WR_Reg(0x3000,0x00); 
	i=0;
	do
	{
		state=OV5640_RD_Reg(0x3029);	
		delay_ms(5);
		i++;
		if(i>1000)return 1;
	}while(state!=0x70); 
	return 0;    
}  
//ִ��һ���Զ��Խ�
//����ֵ:0,�ɹ�;1,ʧ��.
uint8_t OV5640_Focus_Single(void)
{
	uint8_t temp; 
	uint16_t retry=0; 
	OV5640_WR_Reg(0x3022,0x03);		//����һ���Զ��Խ� 
	while(1)
	{
		retry++;
		temp=OV5640_RD_Reg(0x3029);	//���Խ����״̬
		if(temp==0x10)break;		// focus completed
		delay_ms(5);
		if(retry>1000)return 1;
	}
	return 0;	 		
}
//�����Զ��Խ�,��ʧ����,���Զ������Խ�
//����ֵ:0,�ɹ�;����,ʧ��.
uint8_t OV5640_Focus_Constant(void)
{
	uint8_t temp=0;   
	uint16_t retry=0; 
	OV5640_WR_Reg(0x3023,0x01);
	OV5640_WR_Reg(0x3022,0x08);//����IDLEָ�� 
	do 
	{
		temp=OV5640_RD_Reg(0x3023); 
		retry++;
		if(retry>1000)return 2;
		delay_ms(5);
	} while(temp!=0x00);   
	OV5640_WR_Reg(0x3023,0x01);
	OV5640_WR_Reg(0x3022,0x04);//���ͳ����Խ�ָ�� 
	retry=0;
	do 
	{
		temp=OV5640_RD_Reg(0x3023); 
		retry++;
		if(retry>1000)return 2;
		delay_ms(5);
	}while(temp!=0x00);//0,�Խ����;1:���ڶԽ�
	return 0;
} 

void ov5640_test(void)
{
	uint8_t key,t;
	while(1)
	{
	key=KEY_Scan(0);
		if(key==KEY0_PRES)			//RGB565ģʽ
		{
			ovx_mode=0;		
			break;
		}else if(key==KEY1_PRES)	//JPEGģʽ
		{
			ovx_mode=1;		
			break;
		}
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //��˸��ʾ��ʾ��Ϣ
 		if(t==200)
		{	
			LCD_Fill(30,150,210,150+16,WHITE);
			t=0; 
		}
		delay_ms(5);		
	}
	if(ovx_mode==1)jpeg_test();
	else rgb565_test();
}




