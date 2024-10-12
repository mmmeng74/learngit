#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
uint8_t VOFA_RxData;
uint8_t VOFA_RxFlag;


void VOFA_Init(void)//初始化PA9(TX),PA10(RX)用于串口通信
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);

}
//printf重映射
void VOFA_SendByte(int8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}
void VOFA_SendArray(float *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		VOFA_SendByte(Array[i]);
	}
}
int fputCH(int ch, FILE *f)
{
	VOFA_SendByte(ch);
	return ch;
}
//串口接收上位机发来的速度值
uint8_t VOFA_GetRxFlag(void)
{
	if (VOFA_RxFlag == 1)
	{
		VOFA_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t VOFA_GetRxData(void)
{
	return VOFA_RxData;
}

//void USART1_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//	{
//		Serial_RxData = USART_ReceiveData(USART1);
//		Serial_RxFlag = 1;
//		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
//		Serial_RxData = USART_ReceiveData(USART1);
//		
//	}
//}

