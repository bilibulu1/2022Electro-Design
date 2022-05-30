#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 
#include "timer.h"
#include "stm32f4xx_it.h"	   


MV_Struct Boll_Pos;
u8 Point_Number;
u8 Show_flag_o;     //openmv读取完成标志

void Usart3_OV_Init(u32 bound)//串口2    PA2 PA3
{
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能串口2
 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_USART3); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_USART3); 
	//USART2端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
    
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART3, &USART_InitStructure); //初始化串口2
	
  USART_Cmd(USART3, ENABLE);    //关闭串口
		
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启相关中断
	

	//Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口2中断通道
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	         //根据指定的参数初始化VIC寄存器、
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
void USART3_IRQHandler(void)                	//串口2中断服务程序
{
	u8 rec_data;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif 	
	if (USART_GetFlagStatus(USART3, USART_FLAG_PE) != RESET)//奇偶检验错误
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_PE);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)//检测到上溢错误
	{
		USART_ReceiveData(USART3);
		USART_ClearFlag(USART3, USART_FLAG_ORE);
	}

	if (USART_GetFlagStatus(USART3, USART_FLAG_FE) != RESET)//噪声标志
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
		if(start!=0&&rec_data!=0x55)						//如果已收到包头并且当前收到的不是包尾
		{
			buf[start-1]=rec_data;							//缓存数据
			start++;
		}
		else if(start!=0&&rec_data==0x55)					//如果收到包尾
		{
			mode[0]=buf[0];									//给状态存储数组赋值
			
			angle_xunji=(int)mode[0];//向左是大于0的，向右是小于0的
			
			if(angle_xunji>90)
			{
				angle_xunji=angle_xunji-180;
			}
			start=0;
			mode1=1;										//指示主函数循环检测一次
		}
		else if(rec_data==0xaa&&start==0)					//如果收到的是包头
			start++;			
	} 
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif
	 
}














