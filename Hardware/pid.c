#include "pid.h"

#define SPEED_PID_MAX 950   //PID限幅值
#define integral1_max 300
#define v_max 3000

/*
以下为两类pid模板
int pid_del(int16_t v,int16_t speed) :增量式
int pid_location(int16_t v,int16_t speed):位置式 
*/
#define MOBAN 1
#if MOBAN
	int pid_del(int16_t goal,int16_t current)   //ok
	{
		static float pid_delta;//
		static int16_t err[3]={0};//PID差值
		float Kp = 0.0;
		float	Ki = 0.0;
		float Kd = 0.0;
		err[2] = err[1];//上上次偏差
		err[1] = err[0];//上次误差
		err[0] = goal-current;  //设定值 - 反馈值//当前误差
		pid_delta += (int16_t)(Kp*(float)(err[0]-err[1]) +Ki*err[0]+ Kd*(err[0]-2*err[1]+err[2]));//
		if(pid_delta>SPEED_PID_MAX)pid_delta=SPEED_PID_MAX;
		else if(pid_delta<-SPEED_PID_MAX)pid_delta=-SPEED_PID_MAX;
		return pid_delta;
	}
	//
	int pid_location(int16_t goal,int16_t current)   //
	{
		int out;
		float ki_integral1;
		static int integral1;
		static int16_t err[3]={0};//四个PID差值
		float Kp=0.0;
		float	Ki=0.0;
		float Kd=0.0;
		err[1] = err[0];//上次误差
		err[0] = goal-current;  //设定值 - 反馈值//当前误差
		integral1 +=  err[0];
		ki_integral1=Ki*integral1;
		if(ki_integral1>=30) ki_integral1=30;
		else if(ki_integral1<=-30) ki_integral1=-30;
		out = (int16_t)(Kp*(float)err[0] + Ki*integral1 + Kd*(err[0]-err[1]));
		if(out>SPEED_PID_MAX)out=SPEED_PID_MAX;
		else if(out<-SPEED_PID_MAX)out=-SPEED_PID_MAX;
		return out;
	}
#endif
//












