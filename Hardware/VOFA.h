#ifndef __SERIAL__H
#define __SERIAL__H
#include <stdio.h>
void VOFA_Init(void);
void VOFA_SendByte(int8_t Byte);
void VOFA_SendArray(float *Array, uint16_t Length);
uint8_t VOFA_GetRxFlag(void);
uint8_t VOFA_GetRxData(void);
void USART1_IRQHandler(void);
#endif
