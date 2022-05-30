#ifndef __TIMER__H
#define __TIMER__H

#include "sys.h"
#include "stm32f4xx_it.h"


#define Debug_COUNTER  (40)
#define Angle_COUNTER  (5)
#define POSI_COUNTER (5)
#define LED0_COUNTER (100)
#define SPEED_COUNTER (1)

void TIM9_Int_Init(u16 arr,u16 psc);
void TIM1_BRK_TIM9_IRQHandler(void);



#endif