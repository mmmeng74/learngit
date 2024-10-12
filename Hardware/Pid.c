#include "stm32f10x.h" // Device header
#include "sys.h"

float Med_Angle=-10;		//机械中值
float Target_Speed=0;	//期望速度（俯仰）
float Turn_Speed=0;		//期望速度（偏航）

float 
	Vertical_Kp=-240,//直立环KP、KD
	Vertical_Kd=-1.15;//1.15
float 
	Velocity_Kp=-200,//速度环KP、KI
	Velocity_Ki=-1;//0.9
float 
	Turn_Kd=0.6,//转向环KP、KD
	Turn_Kp=-10;

#define SPEED_Y 20   //俯仰(前后)最大设定速度
#define SPEED_Z 10//偏航(左右)最大设定速度 


int Vertical_out,Velocity_out,Turn_out;//直立环&速度环&转向环 的输出变量


void EXTI9_5_IRQHandler(void)
{
	int PWM_out;
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//一级判定
	{
		if(PBin(5)==0)//二级判定
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位
			
			//1、采集编码器数据&MPU6050角度信息。
			Encoder_Left=-Encoder_L_Get();//电机是相对安装，需要对其中一个取反。
			Encoder_Right=Encoder_R_Get();
			
			MPU6050_DMP_Get_Data(&Roll,&Pitch,&Yaw);			//角度
			MPU_Get_Gyroscope(&gyroy,&gyrox,&gyroz);	//陀螺仪
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//加速度

			/*前后*/
			if((Fore==0)&&(Back==0))Target_Speed=0;//未接受到前进后退指令-->速度清零，稳在原地
			if(Fore==1)Target_Speed++;
			
			if(Back==1){Target_Speed--;}//
			Target_Speed=Target_Speed>SPEED_Y?SPEED_Y:(Target_Speed<-SPEED_Y?(-SPEED_Y):Target_Speed);
			
			/*左右*/
			if((Left==0)&&(Right==0))Turn_Speed=0;
			if(Left==1)Turn_Speed+=30;	//左转
			if(Right==1)Turn_Speed-=30;	//右转
			Turn_Speed=Turn_Speed>SPEED_Z?SPEED_Z:(Turn_Speed<-SPEED_Z?(-SPEED_Z):Turn_Speed);
			
			/*转向约束*/
			if((Left==0)&&(Right==0))
			{
				Turn_Kd=0.6;//若无左右转向指令，则开启转向约束
				Turn_Kp=0;
			}
			else if((Left==1)||(Right==1))
			{
				Turn_Kd=0;//若无左右转向指令，则去掉转向约束
				Turn_Kp=-30;
			}
			/*********************************************************************************************/
			
			//2、将数据压入闭环控制中，计算出控制输出量。
			Velocity_out=Velocity(Target_Speed,Encoder_Left,Encoder_Right);	//速度环
			Vertical_out=Vertical(Med_Angle,Pitch,gyroy);			//直立环
			Turn_out=Turn(gyroz,Turn_Speed);						//转向环
			
			PWM_out=Vertical_out+Velocity_out;//最终输出
			//3、把控制输出量加载到电机上，完成最终的的控制。
			MOTO1=PWM_out-Turn_out;//左电机
			MOTO2=PWM_out+Turn_out;//右电机
			Limit(&MOTO1,&MOTO2);	 //PWM限幅	
			Load(MOTO1,MOTO2);
			Stop(&Med_Angle,&Pitch);//安全检测
			
		}
	}
}




/*********************
直立环PD控制器：Kp*Ek+Kd*Ek_D

入口：期望角度、真实角度、真实角速度
出口：直立环输出
*********************/
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_out;
	
	PWM_out=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_Y+4);
	return PWM_out;
}


/*********************
速度环PI：Kp*Ek+Ki*Ek_S
*********************/
int Velocity(int Target,int encoder_left,int encoder_right)
{
	static int Encoder_S,EnC_Err_Lowout_last,PWM_out,Encoder_Err,EnC_Err_Lowout;
	float a=0.84;
	
	//1.计算速度偏差
	Encoder_Err=((encoder_left+encoder_right)-Target);//舍去误差
	//2.对速度偏差进行低通滤波
	EnC_Err_Lowout=(1-a)*Encoder_Err+a*EnC_Err_Lowout_last;//使得波形更加平滑，滤除高频干扰，防止速度突变。
	EnC_Err_Lowout_last=EnC_Err_Lowout;//防止速度过大的影响直立环的正常工作。
	//3.对速度偏差积分，积分出位移
	Encoder_S+=EnC_Err_Lowout;
	//4.积分限幅
	if(Encoder_S>=10000)
	{
		Encoder_S=10000;
	}
	if(Encoder_S<-10000)
	{
		Encoder_S=-10000;
	}
	
	if(stop==1)Encoder_S=0,stop=0;//清零积分量
	
	//5.速度环控制输出计算
	PWM_out=Velocity_Kp*EnC_Err_Lowout+Velocity_Ki*Encoder_S;
	return PWM_out;
}



/*********************
转向环：系数*Z轴角速度+系数*遥控数据
*********************/
int Turn(int gyro_Z,float Turn_Speed)
{
	int PWM_out;
	PWM_out=Turn_Kd*gyro_Z+Turn_Kp*Turn_Speed;
	return PWM_out;
}

