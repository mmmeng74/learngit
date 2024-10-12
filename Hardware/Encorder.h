#ifndef __ENCODER_H
#define __ENCODER_H

void Encoder_L_Init(void);
void Encoder_R_Init(void);
int16_t Encoder_L_Get(void);
int16_t Encoder_R_Get(void);
void TIM2_IRQHandler(void);
void TIM4_IRQHandler(void);



#endif
