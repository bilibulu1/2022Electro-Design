#include "my_lib.h"
#include <stdlib.h>
#include <arm_math.h>
#include "stdint.h"

int constrain_int(int x, int a, int b)
{
	if ((x >= a) && (x <= b))
	{
		return x;
	}
	else
	{
		return (x < a) ? a : b;
	}
}
float constrain_float(float x, float a, float b)
{
	if ((x >= a) && (x <= b))
	{
		return x;
	}
	else
	{
		return (x < a) ? a : b;
	}
}

#define LongToBin(n)      \
	(((n >> 21) & 0x80) | \
	 ((n >> 18) & 0x40) | \
	 ((n >> 15) & 0x20) | \
	 ((n >> 12) & 0x10) | \
	 ((n >> 9) & 0x08) |  \
	 ((n >> 6) & 0x04) |  \
	 ((n >> 3) & 0x02) |  \
	 ((n)&0x01))

#define Bin(n) LongToBin(0x##n##l)

// BCD转HEX
uint8_t BCD2HEX(uint8_t bcd_data)
{
	uint8_t temp;
	temp = ((bcd_data >> 8) * 100) | ((bcd_data >> 4) * 10) | (bcd_data & 0x0f);
	return temp;
}

// HEX转BCD
uint8_t HEX2BCD(uint8_t hex_data)
{
	uint8_t bcd_data;
	uint8_t temp;
	temp = hex_data % 100;
	bcd_data = ((uint8_t)hex_data) / 100 << 8;
	bcd_data = bcd_data | temp / 10 << 4;
	bcd_data = bcd_data | temp % 10;
	return bcd_data;
}

// 冒泡排序
void bubble_sort(uint16_t arr[], uint16_t len)
{
	uint16_t i, j, temp;
	for (i = 0; i < len - 1; i++)
		for (j = 0; j < len - 1 - i; j++)
			if (arr[j] > arr[j + 1])
			{
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
}

// 无限循环
void infinity_loop(void)
{
	for (;;)
	{
	}
}

float map(float x, float in_min, float in_max, float out_min, float out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
/**************************************************************************
函数功能：绝对值函数
入口参数：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a) //取绝对值
{ 		   
	 int temp;
	 if(a<0)  temp=-a;  
	 else temp=a;
	 return temp;
}