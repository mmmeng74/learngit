#include "usart1.h"

u8 USART_RX_STA=0;       //接收状态标记
u8 USART_RX_BUF[64];     //接收缓冲,最大64个字节.

void USART1_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	

	USART_ClearFlag(USART1, USART_FLAG_RXNE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
		

}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		
		if((USART_RX_STA&0x80)==0)//接收未完成
			{
			if(USART_RX_STA&0x40)//接收到了0x0d
				{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else USART_RX_STA|=0x80;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART_RX_STA|=0x40;
				else
					{
					USART_RX_BUF[USART_RX_STA&0X3F]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>63)USART_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
} 




 int fputc1(int ch,FILE *p) //函数默认的，在使用 printf 函数时自动调用 
 { 
	 USART_SendData(USART1,(u8)ch); 
	 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	 return ch;
 } 
