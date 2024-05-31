#include "filter.h"

/********************************************
							��λֵƽ���˲�

 ˵������һ�����ȥ�����ֵ����Сֵ
 �ŵ㣺�ں��������˲����ŵ㡣����żȻ���ֵ������Ը��ţ�����
       ����������Ĳ���ֵƫ������ڸ��������õ��������ã�
       ƽ���ȸߣ����ڸ�Ƶ�񵴵�ϵͳ��
 ȱ�㣺�����ٶ���
*********************************************/

#define maN 7
float mid_ave_filter(float w)	//�Խ��ٶȽ����˲�(��λֵƽ���˲���)
{
  unsigned char i,j;
  float temp;
	float sum=0,value;
  static float value_buf[maN+1];
	value_buf[maN]=w;
  for(i=0;i<maN;i++)
  {
    value_buf[i] = value_buf[i+1];
  }
  //����ֵ��С�������У�ð�ݷ���
  for(j=0;j<maN;j++)
  {
    for(i=0;i<maN+1-j;i++)
    {
      if(value_buf[i]>value_buf[i+1])
      {
        temp = value_buf[i];
        value_buf[i] = value_buf[i+1];
        value_buf[i+1] = temp;
      }
    }
  }
	//
  for(i=1;i<maN-1;i++)
  sum += value_buf[i];

  value = sum/(maN-2);
  return(value);
}

/*
һ�׵�ͨ�˲�
*/
int16_t lpf(int16_t input)																		
{		
	static int16_t err;
	static int16_t out;
	float k=0.7f;
	err=input;
	out=k*out+(1-k)*err;
	return out;
}


/***************************************************
									���ƾ�ֵ�˲�

˵����������N������ֵ����һ�����У����г��ȹ̶�ΪN��
      ÿ�β�����һ�������ݷ����β�����ӵ����׵�һ
      �����ݡ��Ѷ����е�N�����ݽ���ƽ�����㣬�Ȼ��
      �µ��˲������
�ŵ㣺�������Ը��������õ��������ã�ƽ���ȸߣ������ڸ�Ƶ�񵴵�ϵͳ
ȱ�㣺�����ȵͣ���żȻ���ֵ������Ը��ŵ��������ýϲ�����������
      �Ž����صĳ��� ���ʺ����ڿ��ص�Դ��·
****************************************************/
#define filter_max 5
float mf(float in)
{
	static int dat[filter_max+1];//filter_max=5
	int i=0;
	float dat_sum=0;
	
	dat[filter_max]=in;
	
	for(;i<filter_max;i++)
	{
		dat[i]=dat[i+1];
		dat_sum+=dat[i];
	}
	return dat_sum/(filter_max+1);
}
