#include "usart2_wt901.h" 
#include "stm32f4xx_it.h"
void Wt901_Usart2_Init(u32 bound){
GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ�ܴ���2
 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_USART2); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource6,GPIO_AF_USART2); 
	//USART2�˿�����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
    
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART2, ENABLE);    
		
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	

	//Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����2�ж�ͨ��
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ����� 

}
float Now_Angle;                  //yaw��
float Last_Angle; 
float Angle_Z;

float Now_Angle_y;                  //yaw��
float Last_Angle_y; 
float Angle_Y;

float Last_rad;
float Now_rad;
float Rad_Z;

float ANGLE_Init=0.0;
int label_flag_angle=1;

int mode_xunji[1];

void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 res;
	u8 i;
	static u8 data[15]; 
	static u8 count=0;
	static u8 num=0;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif 	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //USART_FLAG_RXNE
		res =USART_ReceiveData(USART2);	
		if(count!=0&&res!=0x55)						//������յ���ͷ���ҵ�ǰ�յ��Ĳ��ǰ�β
		{
			data[count-1]=res;							//��������
			count++;
		}
		else if(count!=0&&res==0x55)					//����յ���β
		{
			mode_xunji[0]=data[0];									//��״̬�洢���鸳ֵ
			count=0;
		}
		else if(res==0xaa&&count==0)					//����յ����ǰ�ͷ
			count++;		
	}		
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif 
}
