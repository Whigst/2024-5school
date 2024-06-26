#ifndef _MS53L1M_H
#define _MS53L1M_H

#include "sys.h"
#include "stdbool.h"

#define Uart_Init(bps)       uart6_init(bps)       /*串口初始化API*/
#define Uart_Send(data,len)  usart6_send(data,len)  /*串口发送API*/

#define MFRAME_H       0X51 /*主机请求帧头*/
#define SFRAME_H       0X55 /*从机应答帧头*/
#define SENSOR_TYPE    0X0B /*MS53L1M传感器*/


#define RPACK_MAX_LEN  270  /*接收包最大长度*/
#define RPACK_MIN_LEN  8   /*接收包最小长度*/

#define BUFF_MAX_LEN 300 /*串口接收最大长度*/

/*用户数据处理*/
typedef struct
{
    uint8_t tx_buff[BUFF_MAX_LEN]; /*发送缓存*/
    uint8_t rx_buff[BUFF_MAX_LEN]; /*接收缓存*/
    bool rx_ok;           /*接收完成标志 1:完成 0:还没*/
    uint16_t rx_len;       /*接收缓存的长度*/

} _Tof_Data;

extern _Tof_Data Tof_Data;

/*模块参数表*/
typedef struct
{
    uint8_t backrate;/*回传速率*/
    uint8_t baudrate;/*串口波特率*/
    uint16_t id; /*设备地址*/
    uint16_t meaudata;/*测量数据*/
    uint8_t opstatus;/*测量数据状态*/
    uint8_t calimode;/*校准状态*/
    uint8_t meaumode;/*测量模式*/
    uint8_t workmode;/*工作模式*/
    uint8_t errorfarm_enable;/*异常帧输出使能*/

} _Tof_Info;


/*模块功能码*/
enum
{
    SYS_REG = 0X00,       /*系统设置*/
    BACKRATE_REG = 0X01,  /*回传速率设置*/
    BAUDRATE_REG = 0X02,  /*波特率设置*/
    IDSET_REG = 0X03,     /*地址设置*/
    MEAUDATA_REG = 0X05,  /*测量数据获取 2字节*/
    OUTPUTSTATUS_REG = 0X07, /*测量状态*/
    MEAUMODE_REG = 0X08,  /*测量模式设置*/
    CALIMODE_REG = 0X09,  /*校准模式*/
    WORKMODE_REG = 0X0A,  /*工作模式*/
    TIMEBUDGET_REG = 0X0B,/*定时预设  2字节*/
    TIMEPERIODL_REG = 0X0D,/*测量间隔 2字节*/
    ERRORFRAM_REG = 0X0F, /*错误帧信息*/
    VERSION_REG = 0X10    /*版本信息 2字节*/
};

/*系统设置参数*/
enum
{
    SYS_PARAM_RESET = 0X01, /*系统恢复设置*/
    SYS_RESET = 0X02, /*系统复位*/
};

/*回传速度参数*/
enum
{
    POSTBACK_01HZ = 0X00, /*0.1Hz*/
    POSTBACK_02HZ = 0X01, /*0.2Hz*/
    POSTBACK_05HZ = 0X02, /*0.5Hz*/
    POSTBACK_1HZ = 0X03, /*1Hz*/
    POSTBACK_2HZ = 0X04, /*2Hz*/
    POSTBACK_5HZ = 0X05, /*5Hz*/
    POSTBACK_10HZ = 0X06, /*10Hz*/
    POSTBACK_20HZ = 0X07, /*20Hz*/
    POSTBACK_50HZ = 0X08, /*50Hz*/
    POSTBACK_100HZ = 0X09 /*100Hz*/

};

/*串口波特率参数*/
enum
{
    BAUD2400 = 0X00, /*2400*/
    BAUD4800 = 0X01, /*4800*/
    BAUD9600 = 0X02, /*9600*/
    BAUD19200 = 0X03, /*19200*/
    BAUD38400 = 0X04, /*38400*/
    BAUD57600 = 0X05, /*57600*/
    BAUD115200 = 0X06, /*115200*/
    BAUD230400 = 0X07, /*230400*/
    BAUD460800 = 0X08, /*460800*/
    BAUD921600 = 0X09 /*921600*/
};

/*测量模式参数*/
enum
{
    SHORT_DMODE=0, /*短距离*/
    MID_DMODE=1,   /*中距离*/
    LONG_DMODE=2   /*长距离*/   
};

/*工作模式参数*/
enum
{
    NORMAL_WMODE = 0X00, /*Normal模式*/
    MODBUS_WMODE = 0X01, /*Modbus模式*/
    IIC_WMODE = 0X02   /*IIC模式*/
};

/*错误帧输出设置*/
enum
{
    ERROR_FRAME_OFF = 0X00, /*关闭*/
    ERROR_FRAME_ON = 0X01 /*使能*/
};


/*应答包返回状态*/
enum
{
    PACK_OK = 0X00, /*包正常*/
    PACK_RXTIMEOUT,/*接收超时*/
    PACK_RXLEN_ERROR, /*长度异常*/
    PACK_ERROR,/*帧异常*/
    PACK_CRCERROR,/*CRC错误*/
    PACK_EXCEPTION_CODE,/*异常码*/
};

void Ms53l1m_Init(void);
uint8_t Ms53l1m_WData(uint16_t addr, uint8_t reg, uint8_t data);
uint8_t Ms53l1m_RData(uint16_t addr, uint8_t reg, uint8_t datalen, uint16_t *data);
void Modbus_DataGet(void);
void Normal_DataGet(void);
extern uint32_t distance;
void get_tof_data(void);

#endif


