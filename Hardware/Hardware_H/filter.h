#ifndef __FILTER_H
#define __FILTER_H

#include "sys.h"

float mid_ave_filter(float w);	//�Խ��ٶȽ����˲�(��λֵƽ���˲���)
float mf(float in);
int16_t lpf(int16_t input);


#endif 

