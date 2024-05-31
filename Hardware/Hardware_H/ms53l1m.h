#ifndef _MS53L1M_H
#define _MS53L1M_H

#include "sys.h"
#include "stdbool.h"

#define Uart_Init(bps)       uart6_init(bps)       /*���ڳ�ʼ��API*/
#define Uart_Send(data,len)  usart6_send(data,len)  /*���ڷ���API*/

#define MFRAME_H       0X51 /*��������֡ͷ*/
#define SFRAME_H       0X55 /*�ӻ�Ӧ��֡ͷ*/
#define SENSOR_TYPE    0X0B /*MS53L1M������*/


#define RPACK_MAX_LEN  270  /*���հ���󳤶�*/
#define RPACK_MIN_LEN  8   /*���հ���С����*/

#define BUFF_MAX_LEN 300 /*���ڽ�����󳤶�*/

/*�û����ݴ���*/
typedef struct
{
    uint8_t tx_buff[BUFF_MAX_LEN]; /*���ͻ���*/
    uint8_t rx_buff[BUFF_MAX_LEN]; /*���ջ���*/
    bool rx_ok;           /*������ɱ�־ 1:��� 0:��û*/
    uint16_t rx_len;       /*���ջ���ĳ���*/

} _Tof_Data;

extern _Tof_Data Tof_Data;

/*ģ�������*/
typedef struct
{
    uint8_t backrate;/*�ش�����*/
    uint8_t baudrate;/*���ڲ�����*/
    uint16_t id; /*�豸��ַ*/
    uint16_t meaudata;/*��������*/
    uint8_t opstatus;/*��������״̬*/
    uint8_t calimode;/*У׼״̬*/
    uint8_t meaumode;/*����ģʽ*/
    uint8_t workmode;/*����ģʽ*/
    uint8_t errorfarm_enable;/*�쳣֡���ʹ��*/

} _Tof_Info;


/*ģ�鹦����*/
enum
{
    SYS_REG = 0X00,       /*ϵͳ����*/
    BACKRATE_REG = 0X01,  /*�ش���������*/
    BAUDRATE_REG = 0X02,  /*����������*/
    IDSET_REG = 0X03,     /*��ַ����*/
    MEAUDATA_REG = 0X05,  /*�������ݻ�ȡ 2�ֽ�*/
    OUTPUTSTATUS_REG = 0X07, /*����״̬*/
    MEAUMODE_REG = 0X08,  /*����ģʽ����*/
    CALIMODE_REG = 0X09,  /*У׼ģʽ*/
    WORKMODE_REG = 0X0A,  /*����ģʽ*/
    TIMEBUDGET_REG = 0X0B,/*��ʱԤ��  2�ֽ�*/
    TIMEPERIODL_REG = 0X0D,/*������� 2�ֽ�*/
    ERRORFRAM_REG = 0X0F, /*����֡��Ϣ*/
    VERSION_REG = 0X10    /*�汾��Ϣ 2�ֽ�*/
};

/*ϵͳ���ò���*/
enum
{
    SYS_PARAM_RESET = 0X01, /*ϵͳ�ָ�����*/
    SYS_RESET = 0X02, /*ϵͳ��λ*/
};

/*�ش��ٶȲ���*/
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

/*���ڲ����ʲ���*/
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

/*����ģʽ����*/
enum
{
    SHORT_DMODE=0, /*�̾���*/
    MID_DMODE=1,   /*�о���*/
    LONG_DMODE=2   /*������*/   
};

/*����ģʽ����*/
enum
{
    NORMAL_WMODE = 0X00, /*Normalģʽ*/
    MODBUS_WMODE = 0X01, /*Modbusģʽ*/
    IIC_WMODE = 0X02   /*IICģʽ*/
};

/*����֡�������*/
enum
{
    ERROR_FRAME_OFF = 0X00, /*�ر�*/
    ERROR_FRAME_ON = 0X01 /*ʹ��*/
};


/*Ӧ�������״̬*/
enum
{
    PACK_OK = 0X00, /*������*/
    PACK_RXTIMEOUT,/*���ճ�ʱ*/
    PACK_RXLEN_ERROR, /*�����쳣*/
    PACK_ERROR,/*֡�쳣*/
    PACK_CRCERROR,/*CRC����*/
    PACK_EXCEPTION_CODE,/*�쳣��*/
};

void Ms53l1m_Init(void);
uint8_t Ms53l1m_WData(uint16_t addr, uint8_t reg, uint8_t data);
uint8_t Ms53l1m_RData(uint16_t addr, uint8_t reg, uint8_t datalen, uint16_t *data);
void Modbus_DataGet(void);
void Normal_DataGet(void);
extern uint32_t distance;
void get_tof_data(void);

#endif


