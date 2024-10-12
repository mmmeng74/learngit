#include "stm32f10x.h" // Device header
#include "sys.h"

float Med_Angle=-10;		//��е��ֵ
float Target_Speed=0;	//�����ٶȣ�������
float Turn_Speed=0;		//�����ٶȣ�ƫ����

float 
	Vertical_Kp=-240,//ֱ����KP��KD
	Vertical_Kd=-1.15;//1.15
float 
	Velocity_Kp=-200,//�ٶȻ�KP��KI
	Velocity_Ki=-1;//0.9
float 
	Turn_Kd=0.6,//ת��KP��KD
	Turn_Kp=-10;

#define SPEED_Y 20   //����(ǰ��)����趨�ٶ�
#define SPEED_Z 10//ƫ��(����)����趨�ٶ� 


int Vertical_out,Velocity_out,Turn_out;//ֱ����&�ٶȻ�&ת�� ���������


void EXTI9_5_IRQHandler(void)
{
	int PWM_out;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//һ���ж�
	{
		if(PBin(5)==0)//�����ж�
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//����жϱ�־λ
			
			//1���ɼ�����������&MPU6050�Ƕ���Ϣ��
			Encoder_Left=-Encoder_L_Get();//�������԰�װ����Ҫ������һ��ȡ����
			Encoder_Right=Encoder_R_Get();
			
			MPU6050_DMP_Get_Data(&Roll,&Pitch,&Yaw);			//�Ƕ�
			MPU_Get_Gyroscope(&gyroy,&gyrox,&gyroz);	//������
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//���ٶ�

			/*ǰ��*/
			if((Fore==0)&&(Back==0))Target_Speed=0;//δ���ܵ�ǰ������ָ��-->�ٶ����㣬����ԭ��
			if(Fore==1)Target_Speed++;
			
			if(Back==1){Target_Speed--;}//
			Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?(-SPEED_Y):Target_Speed);
			
			/*����*/
			if((Left==0)&&(Right==0))Turn_Speed=0;
			if(Left==1)Turn_Speed+=30;	//��ת
			if(Right==1)Turn_Speed-=30;	//��ת
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);
			
			/*ת��Լ��*/
			if((Left==0)&&(Right==0))
			{
				Turn_Kd=0.6;//��������ת��ָ�����ת��Լ��
				Turn_Kp=0;
			}
			else if((Left==1)||(Right==1))
			{
				Turn_Kd=0;//��������ת��ָ���ȥ��ת��Լ��
				Turn_Kp=-30;
			}
			/*********************************************************************************************/
			
			//2��������ѹ��ջ������У�����������������
			Velocity_out=Velocity(Target_Speed,Encoder_Left,Encoder_Right);	//�ٶȻ�
			Vertical_out=Vertical(Med_Angle,Pitch,gyroy);			//ֱ����
			Turn_out=Turn(gyroz,Turn_Speed);						//ת��
			
			PWM_out=Vertical_out+Velocity_out;//�������
			//3���ѿ�����������ص�����ϣ�������յĵĿ��ơ�
			MOTO1=PWM_out-Turn_out;//����
			MOTO2=PWM_out+Turn_out;//�ҵ��
			Limit(&MOTO1,&MOTO2);	 //PWM�޷�	
			Load(MOTO1,MOTO2);
			Stop(&Med_Angle,&Pitch);//��ȫ���
			
		}
	}
}




/*********************
ֱ����PD��������Kp*Ek+Kd*Ek_D

��ڣ������Ƕȡ���ʵ�Ƕȡ���ʵ���ٶ�
���ڣ�ֱ�������
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_Y+4);
	return PWM_out;
}


/*********************
�ٶȻ�PI��Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target,int encoder_left,int encoder_right)
{
	static int Encoder_S,EnC_Err_Lowout_last,PWM_out,Encoder_Err,EnC_Err_Lowout;
	float a=0.84;
	
	//1.�����ٶ�ƫ��
	Encoder_Err=((encoder_left+encoder_right)-Target);//��ȥ���
	//2.���ٶ�ƫ����е�ͨ�˲�
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//ʹ�ò��θ���ƽ�����˳���Ƶ���ţ���ֹ�ٶ�ͻ�䡣
	EnC_Err_Lowout_last=EnC_Err_Lowout;//��ֹ�ٶȹ����Ӱ��ֱ����������������
	//3.���ٶ�ƫ����֣����ֳ�λ��
	Encoder_S+=EnC_Err_Lowout;
	//4.�����޷�
	if(Encoder_S>=10000)
	{
		Encoder_S=10000;
	}
	if(Encoder_S<-10000)
	{
		Encoder_S=-10000;
	}
	
	if(stop==1)Encoder_S=0,stop=0;//���������
	
	//5.�ٶȻ������������
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;
	return PWM_out;
}



/*********************
ת�򻷣�ϵ��*Z����ٶ�+ϵ��*ң������
*********************/
int Turn(int gyro_Z,float Turn_Speed)
{
	int PWM_out;
	PWM_out=Turn_Kd*gyro_Z+Turn_Kp*Turn_Speed;
	return PWM_out;
}

