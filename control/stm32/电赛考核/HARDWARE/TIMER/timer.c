#include "timer.h"
#include "stm32f4xx_it.h"

extern OS_SEM  DEBUG_CONTROL;
extern OS_SEM  ANGLE_CONTROL;
extern OS_SEM  POSITION_CONTROL;
extern OS_SEM  LED0_CONTROL;
extern OS_SEM  SPEED_CONTROL;

void TIM9_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  ///使能TIM9时钟

	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseInitStructure);//初始化TIM9
	
	TIM_ITConfig(TIM9,TIM_IT_Update,ENABLE); //允许定时器9更新中断
	TIM_Cmd(TIM9,ENABLE); //使能定时器9
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_BRK_TIM9_IRQn; //定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}

void TIM1_BRK_TIM9_IRQHandler(void)
{

	static uint16_t DebugPeriod = Debug_COUNTER;
	static uint16_t AnglePeriod = Angle_COUNTER;
	static uint16_t PosiPeriod = POSI_COUNTER;
	static uint16_t LED0Period = LED0_COUNTER;
	static uint16_t SPEEDPeriod =SPEED_COUNTER;
	OS_ERR err;
	
	CPU_SR_ALLOC();
	OSIntEnter();	
	
	if(TIM_GetITStatus(TIM9,TIM_IT_Update)==SET) //溢出中断
	{	
		 DebugPeriod --;
		 AnglePeriod--;
		 PosiPeriod--;
		LED0Period--;
         SPEEDPeriod--;
		
		 if(DebugPeriod==0)
		 {
		 	 OSSemPost(&DEBUG_CONTROL,OS_OPT_POST_1,&err);//发送速度信号量
			 DebugPeriod=Debug_COUNTER;
			 
		 }
		 if(AnglePeriod==0)
		 {
		 	 OSSemPost(&ANGLE_CONTROL,OS_OPT_POST_1,&err);//发送速度信号量
			 AnglePeriod=Angle_COUNTER;
		 }
		 if(PosiPeriod==0)
		 {
		 	 OSSemPost(&POSITION_CONTROL,OS_OPT_POST_1,&err);//发送速度信号量
			 PosiPeriod=POSI_COUNTER;
		 }
		 if(LED0Period==0)
		 {
			 OSSemPost(&LED0_CONTROL,OS_OPT_POST_1,&err);//发送LED0信号量
			 LED0Period=LED0_COUNTER;
		 }
		  if(SPEEDPeriod==0)
		 {
			 OSSemPost(&SPEED_CONTROL,OS_OPT_POST_1,&err);//发送LED0信号量
			 SPEEDPeriod=SPEED_COUNTER;
			 
		 }
	}
	TIM_ClearITPendingBit(TIM9,TIM_IT_Update);  //清除中断标志位
	
	OSIntExit();	
}

