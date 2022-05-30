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
������ߣ�
FL�� pwm���� TIM1->CCR1 PB6  AIN1 PF13 AIN2 PF14 encoder����: B1A PC6 B1B PC7
FR: pwm���� TIM1->CCR2 PB7  BIN1 PE0 BIN2 PE1 encoder����: B2A PA7 B2B PA6
BL: pwm���� TIM1->CCR3 PB8  BIN1 PB13 BIN2 PB12 encoder����: B2A PA0 B2B PA1
BR: pwm���� TIM1->CCR4 PB9  AIN1 PB14 AIN2 PB15 encoder����: B1A PB3 B1B PA15

�����Ӧģ�� ���PD1 �ұ�PD2

��·ѭ������ ������ PF0 PF1 PF2 PF3 PF4 PF4 PF5 PF6 ����Ϊ�ߵ�ƽ ��Ϊ�͵�ƽ


*/

//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С	
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ	
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *p_arg);

//�������ȼ�
#define LED0_TASK_PRIO		8
//�����ջ��С	
#define LED0_STK_SIZE 	512
//������ƿ�
OS_TCB Led0TaskTCB;
//�����ջ	
CPU_STK LED0_TASK_STK[LED0_STK_SIZE];
void led0_task(void *p_arg);

//�ǶȻ�������
//�������ȼ�
#define ANGLE_TASK_PRIO		5
//�����ջ��С	
#define ANGLE_STK_SIZE 		512
//������ƿ�
OS_TCB ANGLETaskTCB;
//�����ջ	
CPU_STK ANGLE_TASK_STK[ANGLE_STK_SIZE];
//������
void ANGLE_task(void *p_arg);


//λ�û�������
//�������ȼ�
#define POSITION_TASK_PRIO		6
//�����ջ��С	
#define POSITION_STK_SIZE 		512
//������ƿ�
OS_TCB POSITIONTaskTCB;
//�����ջ	
CPU_STK POSITION_TASK_STK[POSITION_STK_SIZE];
//������
void POSITION_task(void *p_arg);


//�ٶȻ�����
//�������ȼ�
#define SPEED_TASK_PRIO		4
//�����ջ��С	
#define SPEED_STK_SIZE 		512
//������ƿ�
OS_TCB SPEEDTaskTCB;
//�����ջ	
CPU_STK SPEED_TASK_STK[SPEED_STK_SIZE];
void SPEED_task(void *p_arg);

//DEBUG������
//�������ȼ�
#define DEBUG_TASK_PRIO		7
//�����ջ��С	
#define DEBUG_STK_SIZE 		512
//������ƿ�
OS_TCB DEBUGTaskTCB;
//�����ջ	
CPU_STK DEBUG_TASK_STK[DEBUG_STK_SIZE];
//������
void DEBUG_task(void *p_arg);

int main(void)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	
	int o;
	delay_init(168);  	//ʱ�ӳ�ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�жϷ�������
	LED_Init();         //LED��ʼ��
	uart_init(115200);
    TIM9_Int_Init(100-1,16800-1);	//��ʱ��ʱ��168M����Ƶϵ��16800������168M/16800=10Khz�ļ���Ƶ�ʣ�����10��Ϊ1ms 	
	Usart3_OV_Init(9600);
	MOTO_Init();
	MiniBalance_PWM_Init(7199,0);
	MOTO_Init_zhuanxiang();
	
	PID_Set_Init();
	OSInit(&err);		//��ʼ��UCOSIII
	OS_CRITICAL_ENTER();//�����ٽ���
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
                 (OS_TASK_PTR )start_task, 			//������
                 (void		* )0,					//���ݸ��������Ĳ���
                 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
                 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
                 (CPU_STK_SIZE)START_STK_SIZE/10,	//�����ջ�����λ
                 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
                 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
                 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void   	* )0,					//�û�����Ĵ洢��
                 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR, //����ѡ��
                 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OS_CRITICAL_EXIT();	//�˳��ٽ���	 
	OSStart(&err);  //����UCOSIII
	while(1);
}

//��ʼ������
void start_task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;

	CPU_Init();
#if OS_CFG_STAT_TASK_EN > 0u
   OSStatTaskCPUUsageInit(&err);  	//ͳ������                
#endif
	
#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();	
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	 //ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif		
	
	OS_CRITICAL_ENTER();	//�����ٽ���
    //����һ��led0�ź���
	OSSemCreate ((OS_SEM*	)&LED0_CONTROL,
                 (CPU_CHAR*	)"LED0_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//����һ���Ƕ��ź���
	OSSemCreate ((OS_SEM*	)&ANGLE_CONTROL,
                 (CPU_CHAR*	)"ANGLE_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//����һ��λ���ź���
	OSSemCreate ((OS_SEM*	)&POSITION_CONTROL,
                 (CPU_CHAR*	)"POSITION_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//����һ���ٶ��ź���
	OSSemCreate ((OS_SEM*	)&SPEED_CONTROL,
                 (CPU_CHAR*	)"SPEED_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//����һ��debug�ź���
	OSSemCreate ((OS_SEM*	)&DEBUG_CONTROL,
                 (CPU_CHAR*	)"DEBUG_CONTROL",
                 (OS_SEM_CTR)0,		
                 (OS_ERR*	)&err);
	//����LED0����
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
				 
	//����λ�û��ǶȻ�����
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
	//�����ٶȻ��ǶȻ�����
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
	//����λ�û�����
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
    //����debug����
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
				 
	OS_TaskSuspend((OS_TCB*)&StartTaskTCB,&err);		//����ʼ����			 
	OS_CRITICAL_EXIT();	//�����ٽ���
}

//led0������
void led0_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
        OSSemPend(&LED0_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
 		LED0=!LED0;
	}
}
//�ǶȻ�������
void ANGLE_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&ANGLE_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
		Angle_Controls(angle_xunji);
	}
}
//λ�û���������
void POSITION_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&POSITION_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
	}
}
int inio;
//DEBUG��������
void DEBUG_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&DEBUG_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
		printf("mode:%d\r\n",angle_xunji);
		printf("pwm_angle:%d\r\n",PID_PWM_ANGLE);
	
	}
}
//Ҫ�޸ĵĵط���������������������
#define speed_1 3000

void SPEED_task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	while(1)
	{
		OSSemPend(&SPEED_CONTROL,0,OS_OPT_PEND_BLOCKING,0,&err); //�����ź���
		Set_Moto(speed_1-PID_PWM_ANGLE,speed_1+PID_PWM_ANGLE,speed_1-PID_PWM_ANGLE,speed_1+PID_PWM_ANGLE);
    } 
}