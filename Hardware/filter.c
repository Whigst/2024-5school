#include "filter.h"

/********************************************
							中位值平均滤波

 说明：采一组队列去掉最大值和最小值
 优点：融合了两种滤波的优点。对于偶然出现的脉冲性干扰，可消
       除有其引起的采样值偏差。对周期干扰有良好的抑制作用，
       平滑度高，适于高频振荡的系统。
 缺点：测量速度慢
*********************************************/

#define maN 7
float mid_ave_filter(float w)	//对角速度进行滤波(中位值平均滤波法)
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
  //采样值从小到大排列（冒泡法）
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
一阶低通滤波
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
									递推均值滤波

说明：把连续N个采样值看成一个队列，队列长度固定为N。
      每次采样到一个新数据放入队尾，并扔掉队首的一
      次数据。把队列中的N各数据进行平均运算，既获得
      新的滤波结果。
优点：对周期性干扰有良好的抑制作用，平滑度高；适用于高频振荡的系统
缺点：灵敏度低；对偶然出现的脉冲性干扰的抑制作用较差，不适于脉冲干
      扰较严重的场合 不适合用于开关电源电路
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
