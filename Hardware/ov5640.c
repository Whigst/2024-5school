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
//修改信息
//20160226 V1.2
//新增OV5640_Exposure函数.
//20160515 V1.2
//1，修改ov5640_uxga_init_reg_tbl数组名字为：ov5640_init_reg_tbl
//2，修改相关注释
////////////////////////////////////////////////////////////////////////////////// 

//JPEG尺寸支持列表
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

//RGB565测试
//RGB数据直接显示在LCD上面
const uint8_t*EFFECTS_TBL[7]={"Normal","Cool","Warm","B&W","Yellowish ","Inverse","Greenish"};	//7种特效
const uint8_t*JPEG_SIZE_TBL[12]={"QQVGA","QVGA","VGA","SVGA","XGA","WXGA","WXGA+","SXGA","UXGA","1080P","QXGA","500W"};//JPEG图片
void rgb565_test(void)
{ 
	uint8_t key;
	uint8_t effect=0,contrast=2,fac;
	uint8_t scale=1;		//默认是全尺寸缩放
	uint8_t msgbuf[15];	//消息缓存区 
	LCD_Clear(WHITE);
    POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV5640 RGB565 Mode");
	
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,130,200,16,16,"KEY1:Saturation"); 		//色彩饱和度
	LCD_ShowString(30,150,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,170,200,16,16,"KEY_UP:FullSize/Scale");	//1:1尺寸(显示真实尺寸)/全尺寸缩放
	
		//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init();
	
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦
	
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((uint32_t)&LCD->LCD_RAM,1,DMA_MemoryDataSize_HalfWord,DMA_MemoryInc_Disable);//DCMI DMA配置  
 	OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height); 
	DCMI_Start(); 		//启动传输
	while(1)
	{ 
		key=KEY_Scan(0); 
		if(key)
		{ 
			if(key!=KEY1_PRES)DCMI_Stop(); //非KEY1按下,停止显示
			switch(key)
			{				    
				case KEY0_PRES:	//对比度设置
					contrast++;
					if(contrast>6)contrast=0;
					OV5640_Contrast(contrast);
					sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
					break;
				case KEY1_PRES:	//执行一次自动对焦
					OV5640_Focus_Single();
					break;
				case KEY2_PRES:	//特效设置				 
					effect++;
					if(effect>6)effect=0;
					OV5640_Special_Effects(effect);//设置特效
					sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
					break;
				case WKUP_PRES:	//1:1尺寸(显示真实尺寸)/缩放	    
					scale=!scale;  
					if(scale==0)
					{
						fac=800/lcddev.height;//得到比例因子
 						OV5640_OutSize_Set((1280-fac*lcddev.width)/2,(800-fac*lcddev.height)/2,lcddev.width,lcddev.height); 	 
						sprintf((char*)msgbuf,"Full Size 1:1");
					}else 
					{
						OV5640_OutSize_Set(4,0,lcddev.width,lcddev.height);
 						sprintf((char*)msgbuf,"Scale");
					}
					break;
			}
			if(key!=KEY1_PRES)	//非KEY1按下
			{
				LCD_ShowString(30,50,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 
				DCMI_Start();	//重新开始传输
			}
		} 
		delay_ms(10);		
	}       
} 
//

//处理JPEG数据
//当采集完一帧JPEG数据后,调用此函数,切换JPEG BUF.开始下一帧采集.
uint8_t ovx_mode=0;							//bit0:0,RGB565模式;1,JPEG模式 

#define jpeg_buf_size 31*1024  			//定义JPEG数据缓存jpeg_buf的大小(*4字节)
__align(4) uint32_t jpeg_buf[jpeg_buf_size];	//JPEG数据缓存buf
volatile uint32_t jpeg_data_len=0; 			//buf中的JPEG有效数据长度 
volatile uint8_t jpeg_data_ok=0;				//JPEG数据采集完成标志 
										//0,数据没有采集完;
										//1,数据采集完了,但是还没处理;
										//2,数据已经处理完成了,可以开始下一帧接收
//JPEG尺寸支持列表
void jpeg_data_process(void)//JPEG数据处理函数
{
	if(ovx_mode&0X01)	//只有在JPEG格式下,才需要做处理.
	{
		if(jpeg_data_ok==0)	//jpeg数据还未采集完?
		{	
			DMA_Cmd(DMA2_Stream1, DISABLE);//停止当前传输 
			while (DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}//等待DMA2_Stream1可配置  
			jpeg_data_len=jpeg_buf_size-DMA_GetCurrDataCounter(DMA2_Stream1);//得到此次数据传输的长度
				
			jpeg_data_ok=1; 				//标记JPEG数据采集完按成,等待其他函数处理
		}
		if(jpeg_data_ok==2)	//上一次的jpeg数据已经被处理了
		{
			DMA2_Stream1->NDTR=jpeg_buf_size;	
			DMA_SetCurrDataCounter(DMA2_Stream1,jpeg_buf_size);//传输长度为jpeg_buf_size*4字节
			DMA_Cmd(DMA2_Stream1, ENABLE);			//重新传输
			jpeg_data_ok=0;						//标记数据未采集
		}
	}else
	{
		LCD_SetCursor(0,0);  
		LCD_WriteRAM_Prepare();		//开始写入GRAM
	}	
} 	



//JPEG测试
//JPEG数据,通过串口2发送给电脑.
void jpeg_test(void)
{
	uint32_t i,jpgstart,jpglen; 
	uint8_t *p;
	uint8_t key,headok=0;
	uint8_t effect=0,contrast=2;
	uint8_t size=2;			//默认是QVGA 640*480尺寸
	uint8_t msgbuf[15];		//消息缓存区 
	LCD_Clear(WHITE);
  POINT_COLOR=RED; 
	LCD_ShowString(30,50,200,16,16,"ALIENTEK STM32F4");
	LCD_ShowString(30,70,200,16,16,"OV5640 JPEG Mode");
	LCD_ShowString(30,100,200,16,16,"KEY0:Contrast");			//对比度
	LCD_ShowString(30,120,200,16,16,"KEY1:Saturation"); 		//色彩饱和度
	LCD_ShowString(30,140,200,16,16,"KEY2:Effects"); 			//特效 
	LCD_ShowString(30,160,200,16,16,"KEY_UP:Size");				//分辨率设置 
	sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
	LCD_ShowString(30,180,200,16,16,msgbuf);					//显示当前JPEG分辨率
	
 	//自动对焦初始化
	OV5640_RGB565_Mode();	//RGB565模式 
	OV5640_Focus_Init(); 
	
 	OV5640_JPEG_Mode();		//JPEG模式
	
	OV5640_Light_Mode(0);	//自动模式
	OV5640_Color_Saturation(3);//色彩饱和度0
	OV5640_Brightness(4);	//亮度0
	OV5640_Contrast(3);		//对比度0
	OV5640_Sharpness(33);	//自动锐度
	OV5640_Focus_Constant();//启动持续对焦
	
	My_DCMI_Init();			//DCMI配置
	DCMI_DMA_Init((uint32_t)&jpeg_buf,jpeg_buf_size,DMA_MemoryDataSize_Word,DMA_MemoryInc_Enable);//DCMI DMA配置   
	OV5640_OutSize_Set(4,0,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸 
	DCMI_Start(); 		//启动传输
	while(1)
	{
		if(jpeg_data_ok==1)	//已经采集完一帧图像了
		{  
			p=(uint8_t*)jpeg_buf;
			printf("jpeg_data_len:%d\r\n",jpeg_data_len*4);//打印帧率
			LCD_ShowString(30,210,210,16,16,"Sending JPEG data..."); //提示正在传输数据 
			jpglen=0;	//设置jpg文件大小为0
			headok=0;	//清除jpg头标记
			for(i=0;i<jpeg_data_len*4;i++)//查找0XFF,0XD8和0XFF,0XD9,获取jpg文件大小
			{
				if((p[i]==0XFF)&&(p[i+1]==0XD8))//找到FF D8
				{
					jpgstart=i;
					headok=1;	//标记找到jpg头(FF D8)
				}
				if((p[i]==0XFF)&&(p[i+1]==0XD9)&&headok)//找到头以后,再找FF D9
				{
					jpglen=i-jpgstart+2;
					break;
				}
			}
			if(jpglen)	//正常的jpeg数据 
			{
				p+=jpgstart;			//偏移到0XFF,0XD8处 
				for(i=0;i<jpglen;i++)	//发送整个jpg文件
				{
					while((USART2->SR&0X40)==0);	//循环发送,直到发送完毕   
					USART2->DR=p[i];  
					key=KEY_Scan(0); 
					if(key)break;
				}  
			}
			if(key)	//有按键按下,需要处理
			{  
				LCD_ShowString(30,210,210,16,16,"Quit Sending data   ");//提示退出数据传输
				switch(key)
				{				    
					case KEY0_PRES:	//对比度设置
						contrast++;
						if(contrast>6)contrast=0;
						OV5640_Contrast(contrast);
						sprintf((char*)msgbuf,"Contrast:%d",(signed char)contrast-3);
						break; 
					case KEY1_PRES:	//执行一次自动对焦
						OV5640_Focus_Single();
						break;
					case KEY2_PRES:	//特效设置				 
						effect++;
						if(effect>6)effect=0;
						OV5640_Special_Effects(effect);//设置特效
						sprintf((char*)msgbuf,"%s",EFFECTS_TBL[effect]);
						break;
					case WKUP_PRES:	//JPEG输出尺寸设置   
						size++;  
						if(size>11)size=0;   
						OV5640_OutSize_Set(16,4,jpeg_img_size_tbl[size][0],jpeg_img_size_tbl[size][1]);//设置输出尺寸  
						sprintf((char*)msgbuf,"JPEG Size:%s",JPEG_SIZE_TBL[size]);
						break;
				}
				LCD_Fill(30,180,239,190+16,WHITE);
				LCD_ShowString(30,180,210,16,16,msgbuf);//显示提示内容
				delay_ms(800); 				  
			}else LCD_ShowString(30,210,210,16,16,"Send data complete!!");//提示传输结束设置 
			jpeg_data_ok=2;	//标记jpeg数据处理完了,可以让DMA去采集下一帧了.
		}		
	}     
} 

//OV5640写寄存器
//返回值:0,成功;1,失败.
uint8_t OV5640_WR_Reg(uint16_t reg,uint8_t data)
{
	uint8_t res=0;
	SCCB_Start(); 					//启动SCCB传输
	if(SCCB_WR_Byte(OV5640_ADDR))res=1;	//写器件ID	  
   	if(SCCB_WR_Byte(reg>>8))res=1;	//写寄存器高8位地址
   	if(SCCB_WR_Byte(reg))res=1;		//写寄存器低8位地址		  
   	if(SCCB_WR_Byte(data))res=1; 	//写数据	 
  	SCCB_Stop();	  
  	return	res;
}
//OV5640读寄存器
//返回值:读到的寄存器值
uint8_t OV5640_RD_Reg(uint16_t reg)
{
	uint8_t val=0;
	SCCB_Start(); 				//启动SCCB传输
	SCCB_WR_Byte(OV5640_ADDR);	//写器件ID
   	SCCB_WR_Byte(reg>>8);	    //写寄存器高8位地址   
  	SCCB_WR_Byte(reg);			//写寄存器低8位地址	  
 	SCCB_Stop();   
 	//设置寄存器地址后，才是读
	SCCB_Start();
	SCCB_WR_Byte(OV5640_ADDR|0X01);//发送读命令	  
   	val=SCCB_RD_Byte();		 	//读取数据
  	SCCB_No_Ack();
  	SCCB_Stop();
  	return val;
}
//初始化OV5640 
//返回值:0,成功
//    其他,错误代码
uint8_t OV5640_Init(void)
{ 
	uint16_t i=0;
	uint16_t reg;
	//设置IO     	   
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  //GPIOG9,15初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_15;//PG9,15推挽输出
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //推挽输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化
	
	OV5640_RST=0;			//必须先拉低OV5640的RST脚,再上电
	delay_ms(20); 
	OV5640_PWDN=0;		//POWER ON
	delay_ms(5);  
	OV5640_RST=1;			//结束复位 
	delay_ms(20);      
  	SCCB_Init();			//初始化SCCB 的IO口 
	delay_ms(5); 
	reg=OV5640_RD_Reg(OV5640_CHIPIDH);	//读取ID 高八位
	reg<<=8;
	reg|=OV5640_RD_Reg(OV5640_CHIPIDL);	//读取ID 低八位
	if(reg!=OV5640_ID)
	{
		printf("ID:%d\r\n",reg);
		return 1;
	}  
	OV5640_WR_Reg(0x3103,0X11);	//system clock from pad, bit[1]
	OV5640_WR_Reg(0X3008,0X82);	//软复位
	delay_ms(10);
 	//初始化 OV5640
	for(i=0;i<sizeof(ov5640_init_reg_tbl)/4;i++)
	{
	   	OV5640_WR_Reg(ov5640_init_reg_tbl[i][0],ov5640_init_reg_tbl[i][1]);
 	}    
	//检查闪光灯是否正常
	OV5640_Flash_Ctrl(1);//打开闪光灯
	delay_ms(50);
	OV5640_Flash_Ctrl(0);//关闭闪光灯
  	return 0x00; 	//ok
} 
//OV5640切换为JPEG模式
void OV5640_JPEG_Mode(void) 
{
	uint16_t i=0; 
	//设置:输出JPEG数据
	for(i=0;i<(sizeof(OV5640_jpeg_reg_tbl)/4);i++)
	{
		OV5640_WR_Reg(OV5640_jpeg_reg_tbl[i][0],OV5640_jpeg_reg_tbl[i][1]);  
	}  
}
//OV5640切换为RGB565模式
void OV5640_RGB565_Mode(void) 
{
	uint16_t i=0;
	//设置:RGB565输出
	for(i=0;i<(sizeof(ov5640_rgb565_reg_tbl)/4);i++)
	{
		OV5640_WR_Reg(ov5640_rgb565_reg_tbl[i][0],ov5640_rgb565_reg_tbl[i][1]); 
	} 
} 
//EV曝光补偿设置参数表，支持7个等级
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

//EV曝光补偿
//exposure:0~6,代表补偿-3~3. 
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

//灯光模式参数表,支持5个模式
const static uint8_t OV5640_LIGHTMODE_TBL[5][7]=
{ 
	0x04,0X00,0X04,0X00,0X04,0X00,0X00,//Auto,自动 
	0x06,0X1C,0X04,0X00,0X04,0XF3,0X01,//Sunny,日光
	0x05,0X48,0X04,0X00,0X07,0XCF,0X01,//Office,办公室
	0x06,0X48,0X04,0X00,0X04,0XD3,0X01,//Cloudy,阴天 
	0x04,0X10,0X04,0X00,0X08,0X40,0X01,//Home,室内
}; 
//白平衡设置
//0:自动
//1:日光sunny
//2,办公室office
//3,阴天cloudy
//4,家里home
void OV5640_Light_Mode(uint8_t mode)
{
	uint8_t i;
	OV5640_WR_Reg(0x3212,0x03);	//start group 3
	for(i=0;i<7;i++)OV5640_WR_Reg(0x3400+i,OV5640_LIGHTMODE_TBL[mode][i]);//设置饱和度 
	OV5640_WR_Reg(0x3212,0x13); //end group 3
	OV5640_WR_Reg(0x3212,0xa3); //launch group 3	
}
//色彩饱和度设置参数表,支持7个等级
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
//色度设置
//sat:0~6,代表饱和度-3~3. 
void OV5640_Color_Saturation(uint8_t sat)
{ 
	uint8_t i;
	OV5640_WR_Reg(0x3212,0x03);	//start group 3
	OV5640_WR_Reg(0x5381,0x1c);
	OV5640_WR_Reg(0x5382,0x5a);
	OV5640_WR_Reg(0x5383,0x06);
	for(i=0;i<6;i++)OV5640_WR_Reg(0x5384+i,OV5640_SATURATION_TBL[sat][i]);//设置饱和度 
	OV5640_WR_Reg(0x538b, 0x98);
	OV5640_WR_Reg(0x538a, 0x01);
	OV5640_WR_Reg(0x3212, 0x13); //end group 3
	OV5640_WR_Reg(0x3212, 0xa3); //launch group 3	
}
//亮度设置
//bright:0~8,代表亮度-4~4.
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
//对比度设置
//contrast:0~6,代表亮度-3~3.
void OV5640_Contrast(uint8_t contrast)
{
	uint8_t reg0val=0X00;//contrast=3,默认对比度
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
//锐度设置
//sharp:0~33,0,关闭;33,auto;其他值,锐度范围.
void OV5640_Sharpness(uint8_t sharp)
{
	if(sharp<33)//设置锐度值
	{
		OV5640_WR_Reg(0x5308,0x65);
		OV5640_WR_Reg(0x5302,sharp);
	}else	//自动锐度
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
//特效设置参数表,支持7个特效
const static uint8_t OV5640_EFFECTS_TBL[7][3]=
{ 
	0X06,0x40,0X10,//正常 
	0X1E,0xA0,0X40,//冷色
	0X1E,0x80,0XC0,//暖色
	0X1E,0x80,0X80,//黑白
	0X1E,0x40,0XA0,//泛黄 
	0X40,0x40,0X10,//反色
	0X1E,0x60,0X60,//偏绿
}; 
//特效设置
//0:正常    
//1,冷色
//2,暖色   
//3,黑白
//4,偏黄
//5,反色
//6,偏绿	    
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
//测试序列
//mode:0,关闭
//     1,彩条 
//     2,色块
void OV5640_Test_Pattern(uint8_t mode)
{
	if(mode==0)OV5640_WR_Reg(0X503D,0X00);
	else if(mode==1)OV5640_WR_Reg(0X503D,0X80);
	else if(mode==2)OV5640_WR_Reg(0X503D,0X82);
} 
//闪光灯控制
//mode:0,关闭
//     1,打开 
void OV5640_Flash_Ctrl(uint8_t sw)
{
	OV5640_WR_Reg(0x3016,0X02);
	OV5640_WR_Reg(0x301C,0X02); 
	if(sw)OV5640_WR_Reg(0X3019,0X02); 
	else OV5640_WR_Reg(0X3019,0X00);
} 
//设置图像输出大小
//OV5640输出图像的大小(分辨率),完全由该函数确定
//offx,offy,为输出图像在OV5640_ImageWin_Set设定窗口(假设长宽为xsize和ysize)上的偏移
//由于开启了scale功能,用于输出的图像窗口为:xsize-2*offx,ysize-2*offy
//width,height:实际输出图像的宽度和高度
//实际输出(width,height),是在xsize-2*offx,ysize-2*offy的基础上进行缩放处理.
//一般设置offx和offy的值为16和4,更小也是可以,不过默认是16和4 
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV5640_OutSize_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{ 
    OV5640_WR_Reg(0X3212,0X03);  	//开始组3
    //以下设置决定实际输出尺寸(带缩放)
    OV5640_WR_Reg(0x3808,width>>8);	//设置实际输出宽度高字节
    OV5640_WR_Reg(0x3809,width&0xff);//设置实际输出宽度低字节  
    OV5640_WR_Reg(0x380a,height>>8);//设置实际输出高度高字节
    OV5640_WR_Reg(0x380b,height&0xff);//设置实际输出高度低字节
	//以下设置决定输出尺寸在ISP上面的取图范围
	//范围:xsize-2*offx,ysize-2*offy
    OV5640_WR_Reg(0x3810,offx>>8);	//设置X offset高字节
    OV5640_WR_Reg(0x3811,offx&0xff);//设置X offset低字节
	
    OV5640_WR_Reg(0x3812,offy>>8);	//设置Y offset高字节
    OV5640_WR_Reg(0x3813,offy&0xff);//设置Y offset低字节
	
    OV5640_WR_Reg(0X3212,0X13);		//结束组3
    OV5640_WR_Reg(0X3212,0Xa3);		//启用组3设置
	return 0; 
}

//设置图像开窗大小(ISP大小),非必要,一般无需调用此函数
//在整个传感器上面开窗(最大2592*1944),用于OV5640_OutSize_Set的输出
//注意:本函数的宽度和高度,必须大于等于OV5640_OutSize_Set函数的宽度和高度
//     OV5640_OutSize_Set设置的宽度和高度,根据本函数设置的宽度和高度,由DSP
//     自动计算缩放比例,输出给外部设备.
//width,height:宽度(对应:horizontal)和高度(对应:vertical)  
//返回值:0,设置成功
//    其他,设置失败
uint8_t OV5640_ImageWin_Set(uint16_t offx,uint16_t offy,uint16_t width,uint16_t height)
{
	uint16_t xst,yst,xend,yend;
	xst=offx;
	yst=offy;
	xend=offx+width-1;
	yend=offy+height-1;  
    OV5640_WR_Reg(0X3212,0X03);		//开始组3
	OV5640_WR_Reg(0X3800,xst>>8);	
	OV5640_WR_Reg(0X3801,xst&0XFF);	
	OV5640_WR_Reg(0X3802,yst>>8);	
	OV5640_WR_Reg(0X3803,yst&0XFF);	
	OV5640_WR_Reg(0X3804,xend>>8);	
	OV5640_WR_Reg(0X3805,xend&0XFF);
	OV5640_WR_Reg(0X3806,yend>>8);	
	OV5640_WR_Reg(0X3807,yend&0XFF);
    OV5640_WR_Reg(0X3212,0X13);		//结束组3
    OV5640_WR_Reg(0X3212,0Xa3);		//启用组3设置	 
	return 0;
}   
//初始化自动对焦
//返回值:0,成功;1,失败.
uint8_t OV5640_Focus_Init(void)
{ 
	uint16_t i; 
	uint16_t addr=0x8000;
	uint8_t state=0x8F;
	OV5640_WR_Reg(0x3000, 0x20);			//reset MCU	 
	for(i=0;i<sizeof(OV5640_AF_Config);i++) //发送配置数组
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
//执行一次自动对焦
//返回值:0,成功;1,失败.
uint8_t OV5640_Focus_Single(void)
{
	uint8_t temp; 
	uint16_t retry=0; 
	OV5640_WR_Reg(0x3022,0x03);		//触发一次自动对焦 
	while(1)
	{
		retry++;
		temp=OV5640_RD_Reg(0x3029);	//检查对焦完成状态
		if(temp==0x10)break;		// focus completed
		delay_ms(5);
		if(retry>1000)return 1;
	}
	return 0;	 		
}
//持续自动对焦,当失焦后,会自动继续对焦
//返回值:0,成功;其他,失败.
uint8_t OV5640_Focus_Constant(void)
{
	uint8_t temp=0;   
	uint16_t retry=0; 
	OV5640_WR_Reg(0x3023,0x01);
	OV5640_WR_Reg(0x3022,0x08);//发送IDLE指令 
	do 
	{
		temp=OV5640_RD_Reg(0x3023); 
		retry++;
		if(retry>1000)return 2;
		delay_ms(5);
	} while(temp!=0x00);   
	OV5640_WR_Reg(0x3023,0x01);
	OV5640_WR_Reg(0x3022,0x04);//发送持续对焦指令 
	retry=0;
	do 
	{
		temp=OV5640_RD_Reg(0x3023); 
		retry++;
		if(retry>1000)return 2;
		delay_ms(5);
	}while(temp!=0x00);//0,对焦完成;1:正在对焦
	return 0;
} 

void ov5640_test(void)
{
	uint8_t key,t;
	while(1)
	{
	key=KEY_Scan(0);
		if(key==KEY0_PRES)			//RGB565模式
		{
			ovx_mode=0;		
			break;
		}else if(key==KEY1_PRES)	//JPEG模式
		{
			ovx_mode=1;		
			break;
		}
		t++; 									  
		if(t==100)LCD_ShowString(30,150,230,16,16,"KEY0:RGB565  KEY1:JPEG"); //闪烁显示提示信息
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




