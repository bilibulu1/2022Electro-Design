#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
  
 #include "stm32f4xx_it.h"
typedef struct{
u8 Buf[50];
u8 Cuspos_R;
u8 Cuspos_G;
u8 Cuspos_B;
} MV_Struct;
extern u8 Show_flag_o;
extern u8 Point_Number;
extern MV_Struct Boll_Pos;


extern u8 mode[4];
extern u8 mode1;
extern int angle_xunji;

void Usart3_OV_Init(u32 bound);
void Get_Pos_RGB(void);
void USART3_IRQHandler(void);                	//串口2中断服务程序



#endif











