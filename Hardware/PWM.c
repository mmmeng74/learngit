#include "stm32f10x.h"                  // Device header
#include "sys.h"
/**
  * 函    数：PWM初始化
  * 参    数：无
  * 返 回 值：无
  */
void PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_TIM1 | RCC_APB2Periph_AFIO,ENABLE);//开启时钟
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//初始化GPIO--PA8、PA11为复用推挽输出
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_8 |GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);//初始化定时器。
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=7199;
	TIM_TimeBaseInitStruct.TIM_Prescaler=0;
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseInitStruct);
	
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//初始化输出比较
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse=0;
	TIM_OC1Init(TIM1,&TIM_OCInitStruct);
	TIM_OC4Init(TIM1,&TIM_OCInitStruct);
	
	TIM_CtrlPWMOutputs(TIM1,ENABLE);//高级定时器专属--MOE主输出使能
	
	TIM_OC1PreloadConfig(TIM1,TIM_OCPreload_Enable);/*【3】*///ENABLE//OC1预装载寄存器使能
	TIM_OC4PreloadConfig(TIM1,TIM_OCPreload_Enable);//ENABLE//OC4预装载寄存器使能
	TIM_ARRPreloadConfig(TIM1,ENABLE);//TIM1在ARR上预装载寄存器使能
	
	TIM_Cmd(TIM1,ENABLE);//开定时器。
}

