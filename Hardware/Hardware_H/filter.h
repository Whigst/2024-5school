#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"

float mid_ave_filter(float w);	//对角速度进行滤波(中位值平均滤波法)
float mf(float in);
int16_t lpf(int16_t input);


#endif 

