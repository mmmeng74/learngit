#include "stm32f10x.h"                  // Device header
#include "sys.h" 

float Pitch,Roll,Yaw;								//������Ĭ�ϸ���ֵһ���������ǣ�ƫ����
short gyrox,gyroy,gyroz;				//������--���ٶ�
short aacx,aacy,aacz;						//���ٶ�
int Encoder_Left,Encoder_Right;	//���������ݣ��ٶȣ�

int PWM_MAX=10000,PWM_MIN=-10000;	//PWM�޷�����

int MOTO1,MOTO2;								//���װ�ر���

extern int Velocity_out,Vertical_out;
extern float Target_Speed;

int main(void)
{
	NVIC_Config();
	USART1_Init();
	Serial_Init();

	OLED_Init();
	OLED_Clear();
	
	MPU6050_Init();
	MPU6050_DMP_Init();
	MPU6050_EXTI_Init();
	
	Encoder_R_Init();
	Encoder_L_Init();
	Motor_Init();
	PWM_Init();
	
			
	OLED_ShowString(1,1,"jiao du:");
	OLED_ShowString(2,1,"sd:");
	
	while (1)
	{	
		
		OLED_ShowSignedNum(1,10,Pitch,5);
		OLED_ShowSignedNum(2,5,Encoder_Left,5);
		OLED_ShowSignedNum(3,5,Encoder_Right,5);
	}
}
