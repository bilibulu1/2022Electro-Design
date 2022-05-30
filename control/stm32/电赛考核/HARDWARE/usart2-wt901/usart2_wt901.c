#include "usart2_wt901.h" 
#include "stm32f4xx_it.h"
void Wt901_Usart2_Init(u32 bound){
GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能串口2
 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 
	//USART2端口配置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
    
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口2
	
  USART_Cmd(USART2, ENABLE);    
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启相关中断
	

	//Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//子优先级1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、 

}
float Now_Angle;                  //yaw角
float Last_Angle; 
float Angle_Z;

float Now_Angle_y;                  //yaw角
float Last_Angle_y; 
float Angle_Y;

float Last_rad;
float Now_rad;
float Rad_Z;

float ANGLE_Init=0.0;
int label_flag_angle=1;

int mode_xunji[1];

void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	u8 res;
	u8 i;
	static u8 data[15]; 
	static u8 count=0;
	static u8 num=0;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif 	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //USART_FLAG_RXNE
		res =USART_ReceiveData(USART2);	
		if(count!=0&&res!=0x55)						//如果已收到包头并且当前收到的不是包尾
		{
			data[count-1]=res;							//缓存数据
			count++;
		}
		else if(count!=0&&res==0x55)					//如果收到包尾
		{
			mode_xunji[0]=data[0];									//给状态存储数组赋值
			count=0;
		}
		else if(res==0xaa&&count==0)					//如果收到的是包头
			count++;		
	}		
#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntExit();  											 
#endif 
}
