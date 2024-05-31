#include "LineTrack.h"
#include "sys.h"


/********
*
*  前(灰度  ||  jy61)
*
************/
u8 Front[8]={0};
u8 USART4_value=0;
//void UART4_IRQHandler(void)                	//串口4中断服务程序   前
//{
////	static char txt[30];
//	u8 temp=0, i;//前
//	
//	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
//	{
//		temp = uart_getchar(UART4);
//		for(i=0; i<8; i++)
//        {
//			Front[i] = temp&0x01;	
//			temp = temp>>1;
//		}
////		sprintf(txt,"%d%d%d%d%d%d%d%d%n",Front[0],Front[1],Front[2],Front[3],Front[4],Front[5],Front[6],Front[7]);
////		uart_putstr(USART1,txt);
////		uart_putstr(USART3,txt);	
//		//USART3_value = temp;
//	}
//}
/********
*
*  后
*
************/
u8 Behind[8]={0};
u8 USART6_value=0;
//void USART6_IRQHandler(void)                	
//{
////	static char txt[30];
//	u8 temp=0, i;//前
//	
//	if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET)
//	{
//		temp = uart_getchar(USART6);
//		for(i=0; i<8; i++)
//        {
//			Behind[i] = temp&0x01;
//			temp = temp>>1;
//		}
////		sprintf(txt,"%d%d%d%d%d%d%d%d%n",Behind[0],Behind[1],Behind[2],Behind[3],Behind[4],Behind[5],Behind[6],Behind[7]);
////		uart_putstr(USART1,txt);
////		uart_putstr(USART3,txt);	
////		USART3_value = temp;
//	}
//}
/********
*
*  左
*
************/
u8 Left[8]={0};
u8 USART5_value=0;
//void UART5_IRQHandler(void)                	
//{
////	static char txt[30];
//	u8 temp=0, i;
//	
//	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
//	{
//		temp = uart_getchar(UART5);
//		for(i=0; i<8; i++)
//        {
//			Left[i] = temp&0x01;
//			temp = temp>>1;
//		}
////		sprintf(txt,"%d%d%d%d%d%d%d%d%n",Left[0],Left[1],Left[2],Left[3],Left[4],Left[5],Left[6],Left[7]);
////		uart_putstr(USART1,txt);
////		uart_putstr(USART3,txt);	
////		USART3_value = temp;
//	}
//}
/********
*
*  右
*
************/
u8 Right[8]={0};
u8 USART2_value=0;
void USART2_IRQHandler(void)                	
{
//	static char txt[30];
	u8 temp=0, i;
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		temp = uart_getchar(USART2);
		for(i=0; i<8; i++)
        {
			Right[i] = temp&0x01;
			temp = temp>>1;
		}
//		sprintf(txt,"%d%d%d%d%d%d%d%d%n",Right[0],Right[1],Right[2],Right[3],Right[4],Right[5],Right[6],Right[7]);
//		uart_putstr(USART1,txt);
//		uart_putstr(USART3,txt);	
//		USART3_value = temp;
	}
}


/************
*
*
*获取灰度传感器的值
******************/
u8 Front_old[8];
u8 Behind_old[8];
u8 Left_old[8];
u8 Right_old[8];
u8 Front_flag;//
u8 Behind_flag;
u8 Left_flag;
u8 Right_flag;
u8 Front_flag_old;//
u8 Behind_flag_old;
u8 Left_flag_old;
u8 Right_flag_old;
int8_t Front_left,Front_right;
int8_t err_Front,err_Behind,err_Left,err_Right=100;
u8 Behind_left,Behind_right;
u8 Left_left,Left_right;
u8 Right_left,Right_right;
int8_t Front_end,Back_end,Left_end,Right_end;
u8 Front_flag_b,Behind_flag_b,Left_flag_b,Right_flag_b;

void Data_Update(void)
{
//	u8 t,i;
	u8 i;
	for(i=0;i<8;i++)
	{
		Front_old[i] = Front[i];
		Behind_old[i] = Behind[i];
		Left_old[i] = Left[i];
		Right_old[i] = Right[i];
	}
//	uart_putchar(UART4, 0x57);//前
//	uart_putchar(USART6, 0x57);//后
//	uart_putchar(UART5, 0x57);//左
	uart_putchar(USART2, 0x57);//右
	delay_ms(2);
	//小车前校验和
	Front_left=Front[0]*16+Front[1]*8+Front[2]*4+Front[3]*2;
	Front_right=Front[7]*16+Front[6]*8+Front[5]*4+Front[4]*2;
//	Front_end=-16*!Front[0]-8*!Front[1]-4*!Front[2]-2*!Front[3]+2*!Front[4]+4*!Front[5]+8*!Front[6]+16*!Front[7];
//	Front_end=1*!Front[0]+3*!Front[1]+5*!Front[2]+7*!Front[3]+9*!Front[4]+11*!Front[5]+13*!Front[6]+15*!Front[7];
	
	//小车后校验和
	Behind_left=Behind[0]*16+Behind[1]*8+Behind[2]*4+Behind[3]*2;
	Behind_right=Behind[7]*16+Behind[6]*8+Behind[5]*4+Behind[4]*2;
//	Back_end=!Behind[7]*16+!Behind[6]*8+!Behind[5]*4+!Behind[4]*2-!Behind[0]*16-!Behind[1]*8-!Behind[2]*4-!Behind[3]*2;
//	Back_end=!Behind[7]*2+!Behind[6]*4+!Behind[5]*6+!Behind[4]*8+!Behind[0]*16+!Behind[1]*14+!Behind[2]*12+!Behind[3]*10;
	
	//小车左校验和
	Left_left=Left[0]*16+Left[1]*8+Left[2]*4+Left[3]*2;
	Left_right=Left[7]*16+Left[6]*8+Left[5]*4+Left[4]*2;
//	Left_end=!Left[7]*16+!Left[6]*8+!Left[5]*4+!Left[4]*2-!Left[0]*16-!Left[1]*8-!Left[2]*4-!Left[3]*2;
//	Left_end=!Left[7]*15+!Left[6]*13+!Left[5]*11+!Left[4]*9+!Left[0]*1+!Left[1]*3+!Left[2]*5+!Left[3]*7;
	
	//小车右校验和
	Right_left=Right[0]*16+Right[1]*8+Right[2]*4+Right[3]*2;
	Right_right=Right[7]*16+Right[6]*8+Right[5]*4+Right[4]*2;
//	Right_end=!Right[7]*16+!Right[6]*8+!Right[5]*4+!Right[4]*2-!Right[0]*16-!Right[1]*8-!Right[2]*4-!Right[3]*2;
//	Right_end=!Right[7]*2+!Right[6]*4+!Right[5]*6+!Right[4]*8-!Right[0]*16+!Right[1]*14+!Right[2]*12+!Right[3]*10;
	
	//全黑的标志flag 
	Front_flag_b=(!(Front_left+Front_right))?1:0;
	Behind_flag_b=(!(Behind_left+Behind_right))?1:0;
	Left_flag_b=(!(Left_left+Left_right))?1:0;
	Right_flag_b=(!(Right_left+Right_right))?1:0;

	//全白的标志位flag
	Front_flag = (Front_left==30&&Front_right==30)?1:0;
	Behind_flag = (Behind_left==30&&Behind_right==30)?1:0;
	Left_flag = (Left_left==30&&Left_right==30)?1:0;
	Right_flag = (Right_left==30&&Right_right==30)?1:0;

	
	//灰度传感器左右差值
	err_Front=Front_left-Front_right;
	err_Behind=Behind_right-Behind_left;
	err_Left=Left_left-Left_right;
	err_Right=Right_right-Right_left;
//	err_Front=Front_end;
//	err_Behind=-Back_end;
//	err_Left=Left_end;
//	err_Right=-Right_end;
//	err_Right=Right[7];
}

