#include "init_total.h"
#define ov5640 0

void init(void)
{
	
	delay_init(168);
	IO_A_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	
	// uart1_init(115200);
	// uart2_init(9600);    //Jy61p���ڽ���
	// uart6_init(115200);  //TOF��മ�ڽ���
	
	LED_Init();					//��ʼ��LED 
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ�� 
	Jy61p_Init();       //Jy61p�����ǳ�ʼ��  ʹ��Jy61p_getdata();�õ����ݴ洢��fAngle[0-3]��
	Ms53l1m_Init();     //TOF����ʼ��  ʹ��Normal_DataGet();�õ����ݴ洢��distance������
	
	//����Ϊ����ͷģ���ʼ��
#if ov5640
	while(OV5640_Init())//��ʼ��OV5640
	{
		LCD_ShowString(30,130,240,16,16,"OV5640 ERR");		
		delay_ms(200);
	  LCD_Fill(30,130,239,170,WHITE);		
		delay_ms(200);
		LEDR=!LEDR;
	}	LCD_ShowString(30,130,200,16,16,"OV5640 OK");  	  

	OV5640_RGB565_Mode();//����ΪRGB565ģʽ
#endif
	POINT_COLOR=RED;//��������Ϊ��ɫ
}










