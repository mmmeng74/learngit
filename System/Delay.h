#include "stm32f10x.h"
#ifndef __DELAY_H
#define __DELAY_H

void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);
//
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);
void delay_s(uint32_t s);

#endif
