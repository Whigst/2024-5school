#include "ms53l1m.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "led.h"
#include "display.h"
_Tof_Info Tof_Info;
_Tof_Data Tof_Data;

/**
  * @brief  计算CRC校验和
  * @param   *buff 缓存首地址
  * @param   len  数据长度
  * @retval  uint16_t CRC校验和值
  */
 static uint16_t CRC_Check_Sum(uint8_t* buff, uint16_t len)
{
    uint16_t checksum=0;
    
    while(len--) checksum+=*buff++;

    return checksum ;
}

/**
  * @brief  帧数据包解析
  * @param  data: 读取的数据
  * @retval uint8_t 工作状态 0,成功 >0,失败
  */
 static uint8_t Ms53l1m_Unpack(uint16_t *data)
{  
    uint16_t i=0;
    uint8_t res =0; /*状态返回*/
    uint16_t framh_index=0;/*记录帧头位置*/
    uint16_t fram_len=0; /*帧长度*/
    uint8_t oper_type=0;/*操作类型 0X00-读 0X01-写 0XFF-异常*/
    uint16_t data_len=0;/*获取帧数据长度*/
    uint16_t rx_len=0; /*获取串口接收长度*/
    uint16_t rxchkval=0;/*接收到的CRC校验值*/
    uint16_t calchkval=0;/*计算得到的CRC校验值*/
    
    while(!Tof_Data.rx_ok) /*等待接收完成*/
    {
         delay_ms(1);
         i++;
        if(i==5000) /*接收超时*/
        {
            res = PACK_RXTIMEOUT;/*接收超时*/
            goto error;

        }
    }
    rx_len = Tof_Data.rx_len;
    
    if((rx_len<RPACK_MIN_LEN) || rx_len>RPACK_MAX_LEN) /*接收长度异常*/
    {
        res=PACK_RXLEN_ERROR;/*长度异常*/
        goto error;
    }

    i=0;
    do
    {
       /*查找帧头位置*/
       if((Tof_Data.rx_buff[i]==SFRAME_H)&&(Tof_Data.rx_buff[i+1]==SENSOR_TYPE)) break;
       if(i!=(rx_len-2))
          i++;
        else  /*遍历找不到帧头位置*/
        {
            res=PACK_ERROR; /*帧异常*/
            goto error; 
        } 

    }while(1);
    
    
    framh_index = i; /*记录帧头位置*/
    fram_len = rx_len - framh_index;/*帧头到尾端的长度大小*/
    
    if((fram_len<RPACK_MIN_LEN) || fram_len>RPACK_MAX_LEN) /*接收长度异常*/
    {
        res=PACK_RXLEN_ERROR;/*长度异常*/
        goto error;
    }
    
    oper_type =  Tof_Data.rx_buff[framh_index+4];/*获取操作类型*/
    
    if(oper_type == 0x00)/*读操作*/
    {
        
        data_len = Tof_Data.rx_buff[framh_index+7];/*获取数据长度*/
        
        if((data_len+10)>fram_len) /*接收的一帧长度小于读操作正常帧的长度*/
        {
            res=PACK_ERROR; /*帧异常*/
            goto error;    
        }
        
        calchkval = CRC_Check_Sum(&Tof_Data.rx_buff[framh_index],data_len+8);/*计算CRC校验和*/
        rxchkval = ((u16)Tof_Data.rx_buff[framh_index+data_len+8]<<8) + Tof_Data.rx_buff[framh_index+data_len+9] ; /*计算帧的CRC校验和*/
        
        if(calchkval == rxchkval)/*校验和正确*/
        {                          
              /*读取数据*/
              if(Tof_Data.rx_buff[framh_index+7]==1)/*数据长度为1*/
                *data =  Tof_Data.rx_buff[framh_index+8];
              else if(Tof_Data.rx_buff[framh_index+7]==2)/*数据长度为2*/
                *data = ((u16)Tof_Data.rx_buff[framh_index+8]<<8) + Tof_Data.rx_buff[framh_index+9];  
              
              res=PACK_OK;
              
        }else 
          res=PACK_CRCERROR; /*CRC错误*/


    }else if(oper_type == 0x01)/*写操作*/
    {
        
         calchkval = CRC_Check_Sum(&Tof_Data.rx_buff[framh_index],6);/*计算CRC校验和*/
         rxchkval = ((u16)Tof_Data.rx_buff[framh_index+6]<<8) + Tof_Data.rx_buff[framh_index+7] ; /*计算帧的CRC校验和*/
        
         if(calchkval == rxchkval)/*校验和正确*/
           res=PACK_OK;
         else 
           res=PACK_CRCERROR; /*CRC错误*/  
        
        
    }else if(oper_type == 0xff)/*异常操作*/
    {
        if((Tof_Data.rx_buff[framh_index+2]==0XFF) && (Tof_Data.rx_buff[framh_index+3]==0XFF))
        {
               calchkval = CRC_Check_Sum(&Tof_Data.rx_buff[framh_index],6);/*计算CRC校验和*/
               rxchkval = ((u16)Tof_Data.rx_buff[framh_index+6]<<8) + Tof_Data.rx_buff[framh_index+7] ; /*计算帧的CRC校验和*/ 
            
               if(calchkval == rxchkval)/*校验和正确*/
               {
                   
                  res=PACK_EXCEPTION_CODE;/*非正常码*/ 
              //    printf("ERROR_CODE:%d\r\n",Tof_Data.rx_buff[framh_index+5]); /*打印错误码*/
                   
               }
                  
               else 
               {
                    res=PACK_CRCERROR; /*CRC错误*/  
               }
                                 
        }else 
          res=PACK_ERROR; /*帧异常*/
      
    }else  /*其他情况*/
    {
        res=PACK_ERROR;/*帧异常*/
        
    }
    error:

    
    Tof_Data.rx_ok=0;
    Tof_Data.rx_len=0;
    
    return res;
}

/**
  * @brief  从模块功能码读数据
  * @param  addr:设备地址
  * @param  reg: 功能码地址
  * @param  datalen: 数据长度 只能是1或2
  * @param  data: 读取数据
  * @retval uint8_t 
  */
uint8_t Ms53l1m_RData(uint16_t addr,uint8_t reg,uint8_t datalen,uint16_t *data)
{
    uint8_t res=0;
    uint16_t calchkval = 0;
    
    Tof_Data.tx_buff[0] = MFRAME_H;/*帧头*/
    Tof_Data.tx_buff[1] = SENSOR_TYPE;/*MS53L1M传感器*/
    Tof_Data.tx_buff[2] = (u8)(addr>>8);/*高位地址*/
    Tof_Data.tx_buff[3] = (u8)(addr & 0XFF); /*低位地址*/
    Tof_Data.tx_buff[4] = 0x00; /*读操作*/
    Tof_Data.tx_buff[5] = reg; /*功能码*/
    Tof_Data.tx_buff[6] = datalen; /*读取的长度*/
    
    calchkval =  CRC_Check_Sum(Tof_Data.tx_buff,7);/*CRC校验*/
    
    Tof_Data.tx_buff[7]  = (calchkval>>8) & 0xff;  /*CRC高位前*/
    Tof_Data.tx_buff[8] =  (calchkval & 0xff); /*CRC低位后*/
 
    Tof_Data.rx_ok=0;
    Tof_Data.rx_len=0;
    
    Uart_Send(Tof_Data.tx_buff,9);/*串口发送*/
    
    res = Ms53l1m_Unpack(data);/*应答包解析*/
    
    return res;
    
}

/**
  * @brief  向模块功能码写数据
  * @param  addr:设备地址
  * @param  reg: 功能码地址
  * @param  data:写入数据
  * @retval uint8_t 应答包返回状态 
  */
uint8_t Ms53l1m_WData(uint16_t addr,uint8_t reg, uint8_t data)
{
    uint8_t res=0;
    uint16_t calchkval = 0;
    
    Tof_Data.tx_buff[0] = MFRAME_H;/*帧头*/
    Tof_Data.tx_buff[1] = SENSOR_TYPE;/*MS53L1M传感器*/
    Tof_Data.tx_buff[2] = (u8)(addr>>8);/*高位地址*/
    Tof_Data.tx_buff[3] = (u8)(addr & 0XFF); /*低位地址*/
    Tof_Data.tx_buff[4] = 0x01; /*写*/
    Tof_Data.tx_buff[5] =  reg; /*功能码*/
    Tof_Data.tx_buff[6] = 0x01; /*数据长度*/
    Tof_Data.tx_buff[7] = data; /*数据*/
    
    calchkval =  CRC_Check_Sum(Tof_Data.tx_buff,8);/*CRC校验*/
    
    Tof_Data.tx_buff[8]  = (calchkval>>8) & 0xff;  /*CRC高位前*/
    Tof_Data.tx_buff[9] =  (calchkval & 0xff); /*CRC低位后*/

    Tof_Data.rx_ok=0;
    Tof_Data.rx_len=0;

    Uart_Send(Tof_Data.tx_buff,10);/*串口发送*/
    
    res = Ms53l1m_Unpack(NULL); /*应答包解析*/

    return res;
    
}

/**
  * @brief  传感器初始化
  * @param  
  * @retval  
  */
void Ms53l1m_Init(void)
{
    uint16_t addr=0;
    uint8_t i=0;
    
    Uart_Init(115200);/*串口配置115200*/
    
    delay_ms(10);/*让串口稳定*/
    
    while(Ms53l1m_RData(0xffff,IDSET_REG,2,&addr) && (i<20)) /*使用广播地址查询模块地址*/
    {
        delay_ms(200);
        i++;
    }
    Tof_Info.id = addr; /*记录设备地址*/

   
    i=0;
    while(Ms53l1m_WData(Tof_Info.id,WORKMODE_REG,MODBUS_WMODE) && (i<20)) /*设置MODBUS工作模式*/
    {
       delay_ms(200);
       i++;
    }
    
    
}

/**
  * @brief  Normal模式数据获取
  * @param  
  * @retval  
  */
u32 distance=0;
void Normal_DataGet(void)
{
    
    uint16_t data=0;
    uint8_t res=0;
    uint8_t i=0;

    res=1;
    while(res && (i<10)) 
    {
       res = Ms53l1m_WData(Tof_Info.id,BACKRATE_REG,POSTBACK_5HZ); /*设置回传速率5HZ*/
       delay_ms(100);
       i++;
    }

    res = Ms53l1m_WData(Tof_Info.id,MEAUMODE_REG,LONG_DMODE); /*设置长距离测量模式*/
    
    res = Ms53l1m_WData(Tof_Info.id,WORKMODE_REG,NORMAL_WMODE); /*设置NORMAL模式*/
    
    while(Ms53l1m_RData(Tof_Info.id,WORKMODE_REG,1,&data) && (i<10)) /*读回工作模式*/
    {
        delay_ms(100);
        i++;
    }
 
}
    
void get_tof_data(void)
{
	  char *p=0;   
    uint16_t data=0;
	        if(Tof_Data.rx_ok)/*数据接收完成*/
        {
					 display_data();
            /*获取测量距离*/
            p = strstr((char *)Tof_Data.rx_buff, "d:");
            while(*p !='m')
            {
                 if(*p >= '0' && *p <='9')
                     data = data * 10 + (*p - '0');
                 p++;
            }
            
						distance=data;
            data=0;
            
            Tof_Data.rx_ok = 0;
            Tof_Data.rx_len=0;
          
            LEDG=!LEDG;
        }
}
