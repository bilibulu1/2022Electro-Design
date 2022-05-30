#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "includes.h"
#include "os_app_hooks.h"
#include "stm32f4xx_it.h"
OS_SEM  DEBUG_CONTROL;
OS_SEM  ANGLE_CONTROL;
OS_SEM  POSITION_CONTROL;
OS_SEM  LED0_CONTROL;
OS_SEM  SPEED_CONTROL;

/*
电机接线：
FL： pwm引脚 TIM1->CCR1 PB6  AIN1 PF13 AIN2 PF14 encoder引脚: B1A PC6 B1B PC7
FR: pwm引脚 TIM1->CCR2 PB7  BIN1 PE0 BIN2 PE1 encoder引脚: B2A PA7 B2B PA6
BL: pwm引脚 TIM1->CCR3 PB8  BIN1 PB13 BIN2 PB12 encoder引脚: B2A PA0 B2B PA1
BR: pwm引脚 TIM1->CCR4 PB9  AIN1 PB14 AIN2 PB15 encoder引脚: B1A PB3 B1B PA15

红外感应模块 左边PD1 右边PD2

七路循迹引脚 从左到右 PF0 PF1 PF2 PF3 PF4 PF4 PF5 PF6 不亮为高电平 亮为低电平


*/

//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小	
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈	
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *p_arg);

//任务优先级
#define LED0_TASK_PRIO		8
//任务堆栈大小	
#define LED0_STK_SIZE 	512
//任务控制块
OS_TCB Led0TaskTCB;
//任务堆栈	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//角度环的任务
//任务优先级
#define ANGLE_TASK_PRIO		5
//任务堆栈大小	
#define ANGLE_STK_SIZE 		512
//任务控制块
OS_TCB ANGLETaskTCB;
//任务堆栈	
CPU_STK ANGLE_TASK_STK[ANGLE_STK_SIZE];
//任务函数
void ANGLE_task(void *p_arg);


//位置环的任务
//任务优先级
#define POSITION_TASK_PRIO		6
//任务堆栈大小	
#define POSITION_STK_SIZE 		512
//任务控制块
OS_TCB POSITIONTaskTCB;
//任务堆栈	
CPU_STK POSITION_TASK_STK[POSITION_STK_SIZE];
//任务函数
void POSITION_task(void *p_arg);


//速度环任务
//任务优先级
#define SPEED_TASK_PRIO		4
//任务堆栈大小	
#define SPEED_STK_SIZE 		512
//任务控制块
OS_TCB SPEEDTaskTCB;
//任务堆栈	
CPU_STK SPEED_TASK_STK[SPEED_STK_SIZE];
void SPEED_task(void *p_arg);

//DEBUG的任务
//任务优先级
#define DEBUG_TASK_PRIO		7
//任务堆栈大小	
#define DEBUG_STK_SIZE 		512
//任务控制块
OS_TCB DEBUGTaskTCB;
//任务堆栈	
CPU_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];
//任务函数
void DEBUG_task(void *p_arg);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	int o;
	delay_init(168);  	//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//中断分组配置
	LED_Init();         //LED初始化
	uart_init(115200);
    TIM9_Int_Init(100-1,16800-1);	//定时器时钟168M，分频系数16800，所以168M/16800=10Khz的计数频率，计数10次为1ms 	
	Usart3_OV_Init(9600);
	MOTO_Init();
	MiniBalance_PWM_Init(7199,0);
	MOTO_Init_zhuanxiang();
	
	PID_Set_Init();
	OSInit(&err);		//初始化UCOSIII
	OS_CRITICAL_ENTER();//进入临界区
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
                 (OS_TASK_PTR )start_task, 			//任务函数
                 (void		* )0,					//传递给任务函数的参数
                 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
                 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//任务堆栈深度限位
                 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
                 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
                 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void   	* )0,					//用户补充的存储区
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //任务选项
                 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OS_CRITICAL_EXIT();	//退出临界区	 
	OSStart(&err);  //开启UCOSIII
	while(1);
}

//开始任务函数
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//统计任务                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	 //使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//进入临界区
    //创建一个led0信号量
	OSSemCreate ((OS_SEM*	)&LED0_CONTROL,
                 (CPU_CHAR*	)"LED0_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建一个角度信号量
	OSSemCreate ((OS_SEM*	)&ANGLE_CONTROL,
                 (CPU_CHAR*	)"ANGLE_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建一个位置信号量
	OSSemCreate ((OS_SEM*	)&POSITION_CONTROL,
                 (CPU_CHAR*	)"POSITION_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建一个速度信号量
	OSSemCreate ((OS_SEM*	)&SPEED_CONTROL,
                 (CPU_CHAR*	)"SPEED_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建一个debug信号量
	OSSemCreate ((OS_SEM*	)&DEBUG_CONTROL,
                 (CPU_CHAR*	)"DEBUG_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//创建LED0任务
	OSTaskCreate((OS_TCB 	* )&Led0TaskTCB,		
				 (CPU_CHAR	* )"led0 task", 		
                 (OS_TASK_PTR )led0_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )LED0_TASK_PRIO,     
                 (CPU_STK   * )&LED0_TASK_STK[0],	
                 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
                 (CPU_STK_SIZE)LED0_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,					
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
                 (OS_ERR 	* )&err);				
				 
	//创建位置环角度环任务
	OSTaskCreate((OS_TCB 	* )&ANGLETaskTCB,		
				 (CPU_CHAR	* )"ANGLE task", 		
                 (OS_TASK_PTR )ANGLE_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )ANGLE_TASK_PRIO,     	
                 (CPU_STK   * )&ANGLE_TASK_STK[0],	
                 (CPU_STK_SIZE)ANGLE_STK_SIZE/10,	
                 (CPU_STK_SIZE)ANGLE_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
	//创建速度环角度环任务
	OSTaskCreate((OS_TCB 	* )&SPEEDTaskTCB,		
				 (CPU_CHAR	* )"SPEED task", 		
                 (OS_TASK_PTR )SPEED_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )SPEED_TASK_PRIO,     	
                 (CPU_STK   * )&SPEED_TASK_STK[0],	
                 (CPU_STK_SIZE)SPEED_STK_SIZE/10,	
                 (CPU_STK_SIZE)SPEED_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);		 
	//创建位置环任务
	OSTaskCreate((OS_TCB 	* )&POSITIONTaskTCB,		
				 (CPU_CHAR	* )"POSITION task", 		
                 (OS_TASK_PTR )POSITION_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )POSITION_TASK_PRIO,     	
                 (CPU_STK   * )&POSITION_TASK_STK[0],	
                 (CPU_STK_SIZE)POSITION_STK_SIZE/10,	
                 (CPU_STK_SIZE)POSITION_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
    //创建debug任务
	OSTaskCreate((OS_TCB 	* )&DEBUGTaskTCB,		
				 (CPU_CHAR	* )"DEBUG task", 		
                 (OS_TASK_PTR )DEBUG_task, 			
                 (void		* )0,					
                 (OS_PRIO	  )DEBUG_TASK_PRIO,     	
                 (CPU_STK   * )&DEBUG_TASK_STK[0],	
                 (CPU_STK_SIZE)DEBUG_STK_SIZE/10,	
                 (CPU_STK_SIZE)DEBUG_STK_SIZE,		
                 (OS_MSG_QTY  )0,					
                 (OS_TICK	  )0,					
                 (void   	* )0,				
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, 
                 (OS_ERR 	* )&err);
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//挂起开始任务			 
	OS_CRITICAL_EXIT();	//进入临界区
}

//led0任务函数
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
        OSSemPend(&LED0_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
 		LED0=!LED0;
	}
}
//角度环任务函数
void ANGLE_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&ANGLE_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
		Angle_Controls(angle_xunji);
	}
}
//位置环的任务函数
void POSITION_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&POSITION_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
	}
}
int inio;
//DEBUG的任务函数
void DEBUG_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&DEBUG_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
		printf("mode:%d\r\n",angle_xunji);
		printf("pwm_angle:%d\r\n",PID_PWM_ANGLE);
	
	}
}
//要修改的地方！！！！！！！！！！
#define speed_1 3000

void SPEED_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&SPEED_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //请求信号量
		Set_Moto(speed_1-PID_PWM_ANGLE,speed_1+PID_PWM_ANGLE,speed_1-PID_PWM_ANGLE,speed_1+PID_PWM_ANGLE);
    } 
}