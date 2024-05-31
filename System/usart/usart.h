#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "isr.h"
#include "lcd.h"
//////////////////////////////////////////////////////////////////////////////////	 
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
////////////////////////////////////////////////////////////////////////////////// 	
#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart1_init(unsigned int bound);
void uart2_init(unsigned int bound);
void uart3_init(unsigned int bound);
void uart4_init(unsigned int bound);
void uart5_init(unsigned int bound);
void uart6_init(unsigned int bound);
void usart6_send(uint8_t *data, uint8_t len);
void uart_putchar (USART_TypeDef* uratn, char ch);
void uart_putbuff (USART_TypeDef* uratn, u8 *buff, u32 len);
void Uart2Send(unsigned char *p_data, unsigned int uiSize);
u8 uart_getchar(USART_TypeDef* uratn);
void vcan_sendware(void *wareaddr, uint32_t waresize);
void test_uart(void);
void test_uart_Handler(void);
#endif


