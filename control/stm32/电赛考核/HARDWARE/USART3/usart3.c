#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "stm32f4xx_it.h"	   


MV_Struct Boll_Pos;
u8 Point_Number;
u8 Show_flag_o;     //openmv��ȡ��ɱ�־

void Usart3_OV_Init(u32 bound)//����2    PA2 PA3
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ�ܴ���2
 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_USART3); 
	//USART2�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
    
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART3, ENABLE);    //�رմ���
		
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
	

	//Usart2 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����2�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	         //����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}


void Get_Pos_RGB(void)
{
	u8 i;
	for(i=0;i<49;i++)
	{
		if(Boll_Pos.Buf[i]=='/' && Boll_Pos.Buf[i+1]=='/')
		{
			Boll_Pos.Cuspos_R=Boll_Pos.Buf[i+2];
			Boll_Pos.Cuspos_G=Boll_Pos.Buf[i+3];
			Boll_Pos.Cuspos_B=Boll_Pos.Buf[i+4];
			USART_Cmd(USART3, DISABLE);
		}
	}
}

u8 start=0;
u8 buf[200];
u8 mode[4];
u8 mode1=1;
int angle_xunji;
void USART3_IRQHandler(void)                	//����2�жϷ������
{
	u8 rec_data;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif 	
	if (USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)//��ż�������
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_PE);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)//��⵽�������
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_ORE);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_FE) != RESET)//������־
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_FE);
	}
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //
	{
     rec_data=USART_ReceiveData(USART3);
	USART_ClearITPendingBit(USART3, USART_IT_RXNE);
		/* Read one byte from the receive data register */
		rec_data = USART_ReceiveData(USART3);
		if(start!=0&&rec_data!=0x55)						//������յ���ͷ���ҵ�ǰ�յ��Ĳ��ǰ�β
		{
			buf[start-1]=rec_data;							//��������
			start++;
		}
		else if(start!=0&&rec_data==0x55)					//����յ���β
		{
			mode[0]=buf[0];									//��״̬�洢���鸳ֵ
			
			angle_xunji=(int)mode[0];//�����Ǵ���0�ģ�������С��0��
			
			if(angle_xunji>90)
			{
				angle_xunji=angle_xunji-180;
			}
			start=0;
			mode1=1;										//ָʾ������ѭ�����һ��
		}
		else if(rec_data==0xaa&&start==0)					//����յ����ǰ�ͷ
			start++;			
	} 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
	 
}














