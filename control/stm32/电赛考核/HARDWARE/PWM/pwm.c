#include "pwm.h"
#include "stm32f4xx_it.h"

int32_t PID_PWM_ANGLE=0;

#define PWM_xian_ANGLE 3000 //角度限幅值


void MOTO_Init(void)//初始化控制电机所需的IO
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	//PORTB12 13 14 15 11 10 9 8推挽输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1;
	GPIO_Init(GPIOE,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin =GPIO_Pin_13|GPIO_Pin_14;
	GPIO_Init(GPIOF,&GPIO_InitStruct);
}
void MiniBalance_PWM_Init(u16 arr,u16 psc)
{		 		
	GPIO_InitTypeDef GPIO_InitStructure;  
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;  
	TIM_OCInitTypeDef  TIM_OCInitStructure;  
		
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);         
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);    
		
 
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);
				
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOB,&GPIO_InitStructure);   

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOB,&GPIO_InitStructure);     

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;      
	GPIO_Init(GPIOB,&GPIO_InitStructure); 
		
	TIM_TimeBaseStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period=arr;  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;   
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);		
		
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); 
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); 
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); 
		
	TIM_ARRPreloadConfig(TIM4,ENABLE);
	TIM_Cmd(TIM4, ENABLE); 
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	
	Moto_FrontLeft=0;
	Moto_FrontRight=0; 
    Moto_BackLeft=0;   
    Moto_BackRight=0;
} 
void MOTO_FL_ZhengZhuan(void)
{
	FrontLeft_IN1=1;
	FrontLeft_IN2=0;
}
void MOTO_FR_ZhengZhuan(void)
{
	FrontRight_IN1=0;
	FrontRight_IN2=1;
}
void MOTO_BR_ZhengZhuan(void)
{
	BackRight_IN1=1;
	BackRight_IN2=0;
}
void MOTO_BL_ZhengZhuan(void)
{
	BackLeft_IN1=1;
	BackLeft_IN2=0;
}
void MOTO_FL_FanZhuan(void)
{
	FrontLeft_IN1=0;
	FrontLeft_IN2=1;
}
void MOTO_FR_FanZhuan(void)
{
	FrontRight_IN1=1;
	FrontRight_IN2=0;
}
void MOTO_BR_FanZhuan(void)
{
	BackRight_IN1=0;
	BackRight_IN2=1;
}
void MOTO_BL_FanZhuan(void)
{
	BackLeft_IN1=0;
	BackLeft_IN2=1;
}
void MOTO_Init_zhuanxiang(void)
{
	MOTO_FL_ZhengZhuan();
	MOTO_BL_ZhengZhuan();
	MOTO_FR_ZhengZhuan();
	MOTO_BR_ZhengZhuan();
}
void Set_Moto(int32_t PWM_FL,int32_t PWM_FR,int32_t PWM_BL,int32_t PWM_BR)
{
    if(PWM_FL>=0)    
	{
		MOTO_FL_ZhengZhuan();
		Moto_FrontLeft=PWM_FL;
	}
	 if(PWM_FL<0)    
	{
		PWM_FL=-PWM_FL;
		MOTO_FL_FanZhuan();
		Moto_FrontLeft=PWM_FL;
	}


	 if(PWM_FR>=0)    
	{
		MOTO_FR_ZhengZhuan();
		Moto_FrontRight=PWM_FR;
	}
	 if(PWM_FR<0)    
	{
		PWM_FR=-PWM_FR;
		MOTO_FR_FanZhuan();
		Moto_FrontRight=PWM_FR;
	}
	
	
	 if(PWM_BL>=0)    
	{
		MOTO_BL_ZhengZhuan();
		Moto_BackLeft=PWM_BL;
	}
	 if(PWM_BL<0)    
	{
		PWM_BL=-PWM_BL;
		MOTO_BL_FanZhuan();
		Moto_BackLeft=PWM_BL;
	}
      if(PWM_BR>=0)    
	{
		MOTO_BR_ZhengZhuan();
		Moto_BackRight=PWM_BR;
	}
	if(PWM_BR<0)    
	{
		PWM_BR=-PWM_BR;
		MOTO_BR_FanZhuan();
		Moto_BackRight=PWM_BR;
	}
}
int32_t PWM_FL=0;
int32_t PWM_FR=0;
int32_t PWM_BL=0;
int32_t PWM_BR=0;

//void speed_hecheng(int speedx,int speedy,int speed_delta)
//{	 
//	PWM_FL=(int32_t)((-speedx)-(speedy)+speed_delta);
//	PWM_FR=(int32_t)((-speedx)+(speedy)+speed_delta);
//	PWM_BL=(int32_t)((speedx)-(speedy)+speed_delta);
//	PWM_BR=(int32_t)((speedx)+(speedy)+speed_delta);
//	
//	Set_Moto(PWM_FL,PWM_FR,PWM_BL,PWM_BR);
//	
//}
void Angle_Controls(float angle_want)
{
	SetPoint(&PID_Struct_AngleLoop,angle_want);	
	PID_PWM_ANGLE = PID_IncCalc(&PID_Struct_AngleLoop,0.0f);
	PID_PWM_ANGLE=XianFu_ANGLE(PID_PWM_ANGLE);
}
int32_t XianFu_ANGLE(int32_t PWM)
{
	if(PWM>PWM_xian_ANGLE) PWM=PWM_xian_ANGLE;
	if(PWM<-PWM_xian_ANGLE) PWM=-PWM_xian_ANGLE;
	return PWM;
}
//void ANGLE_Control(float angle_want)
//{
//	 if((ANGLE_Init<=360.0)&&(ANGLE_Init>=270.0)&&(angle_want=0.0))
//	 {
//		if(Now_Angle<180.0) 	
//		{
//			SetPoint(&PID_Struct_AngleLoop,ANGLE_Init-360.0);	
//			PID_PWM_ANGLE=PID_IncCalc(&PID_Struct_AngleLoop,Now_Angle);
//		}
//        else
//		{
//			SetPoint(&PID_Struct_AngleLoop,ANGLE_Init+angle_want);	
//			PID_PWM_ANGLE=PID_IncCalc(&PID_Struct_AngleLoop,Now_Angle);			
//		}			
//	 }
//	 else if((ANGLE_Init>=0.0)&&(ANGLE_Init<=90.0)&&(angle_want==0.0))
//	 {
//		 if(Now_Angle>270.0) 	
//		{
//			SetPoint(&PID_Struct_AngleLoop,ANGLE_Init+360.0);	
//			PID_PWM_ANGLE=PID_IncCalc(&PID_Struct_AngleLoop,Now_Angle);
//		}
//        else
//		{
//			SetPoint(&PID_Struct_AngleLoop,ANGLE_Init+angle_want);	
//			PID_PWM_ANGLE=PID_IncCalc(&PID_Struct_AngleLoop,Now_Angle);			
//		}	
//	 }
//	 else
//	 {
//	    SetPoint(&PID_Struct_AngleLoop,ANGLE_Init+angle_want);
//	
//	    PID_PWM_ANGLE=PID_IncCalc(&PID_Struct_AngleLoop,Now_Angle);
//	 }	
//	PID_PWM_ANGLE=XianFu_ANGLE(PID_PWM_ANGLE);
//}