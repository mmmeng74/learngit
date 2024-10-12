#ifndef __PID_H
#define __PID_H

int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int Target,int encoder_left,int encoder_right);
int Turn(int gyro_Z,float Turn_Speed);
void EXTI9_5_IRQHandler(void);


#endif
