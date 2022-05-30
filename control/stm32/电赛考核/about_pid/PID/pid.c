#include "pid.h"
#include "arm_math.h"

int32_t pid_pwm=0;
PIDTypeDef PID_Struct_AngleLoop;
PIDTypeDef PID_Struct_WheelFL;
PIDTypeDef PID_Struct_WheelFR;
PIDTypeDef PID_Struct_WheelBL;
PIDTypeDef PID_Struct_WheelBR;

//给定pid值
void PID_Set_KP_KI_KD(PIDTypeDef *pid,float kp,float ki,float kd)
{
  pid->P=kp;
  pid->I=ki;
  pid->D=kd;
}

//给定目标值
void SetPoint(PIDTypeDef *pid,float want)
{
	pid->Setpoint=want;
}


void PID_Init(PIDTypeDef *pid)
{
	 pid->Setpoint=0.0;
	 pid->SunError=0.0;
	
	 pid->P=0.0;
	 pid->I=0.0;
	 pid->D=0.0;
	
	 pid->Last_error=0.0;
	 pid->LLast_error=0.0;
}


int32_t  PID_PosLocCalc(PIDTypeDef *pid, int32_t Now_Point)//位置式
{
  float Now_Error,d_Error;
	Now_Error=Now_Point-pid->Setpoint;
	pid->SunError+=Now_Error;//积分限幅
  if(pid->SunError>4000)
		pid->SunError=4000;
	else if(pid->SunError<-4000)
		pid->SunError=-4000;
	d_Error=Now_Error-pid->Last_error;
  pid->Last_error=Now_Error;
	pid_pwm=(int32_t)(pid->P*Now_Error+
		               pid->I*pid->SunError+
	                 pid->D*d_Error);
	return pid_pwm;
}


int32_t PID_IncCalc(PIDTypeDef *pid,float Now_Point)//增量式
{
  float p_Error,Now_Error,d_Error,i_Error;
	Now_Error=pid->Setpoint-Now_Point;
	p_Error=Now_Error-pid->Last_error;//p分量
	i_Error=Now_Error;//I分量
	d_Error=Now_Error-2*pid->Last_error+pid->LLast_error;//D分量
  pid->Last_error=Now_Error;
	pid->LLast_error=pid->Last_error;
	pid->Out_put+=(int32_t)(pid->P*p_Error+
		             pid->I*i_Error+
	               pid->D*d_Error);
	return pid->Out_put;
}


void PID_Set_Init(void)
{
	PID_Init(&PID_Struct_WheelFL);
	PID_Init(&PID_Struct_WheelFR);
    PID_Init(&PID_Struct_WheelBR);
    PID_Init(&PID_Struct_WheelBR);	

//	//速度环PID 
//	PID_Set_KP_KI_KD(&PID_Struct_WheelFL,30.0,0.09,0.0);
//	PID_Set_KP_KI_KD(&PID_Struct_WheelFR,0,0,0);//舵机小车
	
	PID_Set_KP_KI_KD(&PID_Struct_WheelFL,25.0,0.05,0.0);
	PID_Set_KP_KI_KD(&PID_Struct_WheelFR,25.0,0.05,0.0);
    PID_Set_KP_KI_KD(&PID_Struct_WheelBL,25.0,0.05,0.0);
    PID_Set_KP_KI_KD(&PID_Struct_WheelBR,25.0,0.05,0.0);
	
//	PID_Set_KP_KI_KD(&PID_Struct_WheelC,8,0.5,0);
//	PID_Set_KP_KI_KD(&PID_Struct_WheelD,8,0.5,0);
//	
	
//	//角度环PID初始化
	PID_Init(&PID_Struct_AngleLoop);
////	PID_Set_KP_KI_KD(&PID_Struct_AngleLoop,35,0.1,20); 
////	PID_Set_KP_KI_KD(&PID_Struct_AngleLoop,60,0.1,120); 
    PID_Set_KP_KI_KD(&PID_Struct_AngleLoop,120.0,0.0,0.1); 
	
//	
//	//位置环PID
//	PID_Init(&PID_Struct_PositionX);		
//	PID_Init(&PID_Struct_PositionY);	
//	PID_Set_KP_KI_KD(&PID_Struct_PositionX,10,0.01,60);	
//	PID_Set_KP_KI_KD(&PID_Struct_PositionY,10,0.01,60);	
}

