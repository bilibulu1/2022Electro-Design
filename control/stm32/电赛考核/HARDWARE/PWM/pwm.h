#ifndef __PWM_H
#define __PWM_H

#include "sys.h"


#define Moto_FrontLeft  TIM4->CCR1
#define Moto_FrontRight TIM4->CCR2
#define Moto_BackLeft   TIM4->CCR3
#define Moto_BackRight  TIM4->CCR4

#define BackRight_IN1   PBout(14)
#define BackRight_IN2   PBout(15)//yigai

#define BackLeft_IN1   PBout(13)
#define BackLeft_IN2   PBout(12)

#define FrontLeft_IN1   PFout(13)
#define FrontLeft_IN2   PFout(14)

#define FrontRight_IN1   PEout(0)
#define FrontRight_IN2   PEout(1)//yigai



extern int32_t PID_PWM_ANGLE;
extern int32_t PWM_FL;
extern int32_t PWM_FR;
extern int32_t PWM_BL;
extern int32_t PWM_BR;

void MOTO_Init(void);
void MiniBalance_PWM_Init(u16 arr,u16 psc);
void MOTO_FL_ZhengZhuan(void);
void MOTO_BL_ZhengZhuan(void);
void MOTO_FR_ZhengZhuan(void);
void MOTO_BR_ZhengZhuan(void);
void MOTO_FL_FanZhuan(void);
void MOTO_BL_FanZhuan(void);
void MOTO_FR_FanZhuan(void);
void MOTO_BR_FanZhuan(void);
void MOTO_Init_zhuanxiang(void);
void Set_Moto(int32_t PWM_FL,int32_t PWM_FR,int32_t PWM_BL,int32_t PWM_BR);
void speed_hecheng(int speedx,int speedy,int speed_delta);
int32_t XianFu_ANGLE(int32_t PWM);
void Angle_Controls(float angle_want);
void ANGLE_Control(float angle_want);



#endif


