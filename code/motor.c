#include "motor.h"
#include "common.h"
#include  "camera.h"
#define ENCODER_QUADDEC                 (TIM9_ENCOEDER)

//��ˢ��
#define PWM_D1          (TIM1_PWM_MAP3_CH1_E9)
#define DIR_D1             (TIM1_PWM_MAP3_CH2_E11) //�������ṩ�ߵ�λ
//��ˢ��
#define PWM_D2          (TIM8_PWM_MAP0_CH1_C6)
#define DIR_D2             (TIM8_PWM_MAP0_CH2_C7)

//ת����
#define PWM_L1           (TIM4_PWM_MAP1_CH3_D14)
#define DIR_L1              (D15)
//�ƽ���
#define PWM_L2           (TIM4_PWM_MAP1_CH2_D13)
#define DIR_L2              (D12)

//ת����
#define PWM_R1           (TIM2_PWM_MAP0_CH4_A3)
#define DIR_R1              (A2)
//�ƽ���
#define PWM_R2           (TIM2_PWM_MAP0_CH1_A0)
#define DIR_R2              (A1)

Speed_ct Speed;
PWM_Output PWM;
Turn_ct Turn;
int16 bl_duty=0;

int16 maxspeed=6000;
int16 max_angle=200;


int16 zhuanjiaozhi=0;
int16 base_speed=1000;
int16 turnmax=2000;
int16 turnmin=-2000;

void Para_init()
{
    bl_duty=500;//��ˢ�������
    PWM.Left_Out=0;
    PWM.Right_Out=0;
    Speed.Set_Speed=3300;
    //Speed.Speed_Max=4000;

    Speed.Speed_Now=0;
    Speed.Speed_Old=0;
    Speed.Speed_Car=0;

    Speed.Error=0;
    Speed.P_Error=0;
    Speed.I_Error=0;
    Speed.D_Error=0;
    Speed.L_Error=0;
    Speed.derivative=0;

    Speed.P=1.2;
    Speed.I=0.2;
    Speed.D=1;

    Speed.Output_PWM=0;

    Turn.Chazhi=0;
    Turn.Chazhi_old=0;

    Turn.PWM_Lout=0;
    Turn.PWM_Rout=0;
    Turn.PWM_Dout=0;
    Turn.intergrator=0;
    Turn.error=0;
    Turn.last_error=0;

    Turn.turnP=0;
    Turn.turnI=0;
    Turn.turnD=0;

    Turn.P=1.3;
    Turn.I=0.8;
    Turn.D=5;
}
//�������ٶȻ�ȡ�봦��
//������ֱ��-3.3cm
void GetSpeed()
{
    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC);                  // ��ȡ����������
    if(rd.state==2)
    {
        rd.state2_time+=encoder_data_quaddec;
    }
    Speed.Speed_Now=encoder_data_quaddec*40;
    encoder_clear_count(ENCODER_QUADDEC);                                       // ��ձ���������
    Speed.Speed_Car=0.9*Speed.Speed_Now+0.1*Speed.Speed_Old;        //��С����
    Speed.Speed_Old=Speed.Speed_Now;

}
//�����޶�
float constrain_float(float amt, float low, float high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}
//�ٶȻ�pid
void SpeedPID_Control()
{
    Speed.Error=(Speed.Set_Speed-Speed.Speed_Car);
    Speed.Integral+=Speed.Error;
    Speed.Integral=constrain_float(Speed.Integral,-300,300);
    Speed.P_Error=Speed.Error;                                          //��������
    Speed.I_Error=Speed.Integral;                    //���ֻ���
    Speed.D_Error=Speed.Error-Speed.L_Error;                            //΢�ֻ���

    Speed.Output_PWM=Speed.P*Speed.P_Error+Speed.I*Speed.I_Error+Speed.D*Speed.D_Error;
    if(Speed.Output_PWM>4000)
        Speed.Output_PWM=4000;
    else if(Speed.Output_PWM<-4000)
        Speed.Output_PWM=-4000;

}
//ת��pd
void TurnPD_Control()
{

    Turn.error=0.9*Turn.Chazhi+0.1*Turn.last_error;
    if(Turn.error<2&&Turn.error>-2)//��Ӧ����
    {
        Turn.error=0;
    }
    Turn.intergrator+=Turn.error;
    Turn.intergrator=constrain_float(Turn.intergrator,-40,40);
    Turn.turnP=Turn.P*Turn.error;
    Turn.turnD=Turn.D*(Turn.error-Turn.last_error);
    Turn.turnI=Turn.intergrator*Turn.I;
    Turn.PWM_Dout=Turn.P*Turn.error+Turn.intergrator*Turn.I+Turn.D*(Turn.error-Turn.last_error);
    Turn.last_error = Turn.error;
    if(Turn.PWM_Dout>80)
        Turn.PWM_Dout=80;
    else if(Turn.PWM_Dout<-80)
        Turn.PWM_Dout=-80;


}
//������ˢ�����ת�٣�duty��Χ500-1000
void set_brushless_duty(int16 duty)
{
    if(duty>1000)
        duty=980;
    pwm_set_duty(PWM_D1, duty);
    pwm_set_duty(PWM_D2, duty);
}
/*
void PWM_Out()
{
    //�ٶȻ����
    Turn.PWM_Lout=Speed.Output_PWM*(1-Turn.PWM_Dout/max_angle);
    Turn.PWM_Rout=Speed.Output_PWM*(1+Turn.PWM_Dout/max_angle);

    if(Turn.PWM_Lout>maxspeed)
        Turn.PWM_Lout=maxspeed;
    else if(Turn.PWM_Lout<-maxspeed)
        Turn.PWM_Lout=-maxspeed;

    if(Turn.PWM_Rout>maxspeed)
            Turn.PWM_Rout=maxspeed;
    else if(Turn.PWM_Rout<-maxspeed)
            Turn.PWM_Rout=-maxspeed;

    if(Turn.PWM_Lout>0)
    {
        gpio_set_level(DIR_L2, GPIO_LOW);
        pwm_set_duty(PWM_L2,Turn.PWM_Lout);
        gpio_set_level(DIR_R1, GPIO_LOW);
        pwm_set_duty(PWM_R1,Turn.PWM_Lout*0.4);

    }
    else {
        gpio_set_level(DIR_L2, GPIO_HIGH);
        pwm_set_duty(PWM_L2,-Turn.PWM_Lout);
        gpio_set_level(DIR_R1, GPIO_HIGH);
        pwm_set_duty(PWM_R1,Turn.PWM_Lout*0.4);
    }

    if(Turn.PWM_Rout>0)
        {
            gpio_set_level(DIR_R2, GPIO_LOW);
            pwm_set_duty(PWM_R2,Turn.PWM_Rout);
            gpio_set_level(DIR_L1, GPIO_LOW);
            pwm_set_duty(PWM_L1,Turn.PWM_Rout);
        }
        else {
            gpio_set_level(DIR_R2, GPIO_HIGH);
            pwm_set_duty(PWM_R2,-Turn.PWM_Rout);
            gpio_set_level(DIR_L1, GPIO_HIGH);
            pwm_set_duty(PWM_L1,Turn.PWM_Rout);
        }


}*/

void PWM_Out()
{
    //�ٶȻ����
    Turn.PWM_Lout=Speed.Output_PWM*(1-Turn.PWM_Dout/max_angle);
    Turn.PWM_Rout=Speed.Output_PWM*(1+Turn.PWM_Dout/max_angle)+900;

    if(Turn.PWM_Lout>maxspeed)
        Turn.PWM_Lout=maxspeed;
    else if(Turn.PWM_Lout<-maxspeed)
        Turn.PWM_Lout=-maxspeed;

    if(Turn.PWM_Rout>maxspeed)
            Turn.PWM_Rout=maxspeed;
    else if(Turn.PWM_Rout<-maxspeed)
            Turn.PWM_Rout=-maxspeed;

    if(Turn.PWM_Lout>0)
    {
            gpio_set_level(DIR_L2, GPIO_LOW);
            pwm_set_duty(PWM_L2,Turn.PWM_Lout);
    }
    else
    {
            gpio_set_level(DIR_L2, GPIO_HIGH);
            pwm_set_duty(PWM_L2,-Turn.PWM_Lout);
     }

    if(Turn.PWM_Rout>0)
    {
            gpio_set_level(DIR_R2, GPIO_LOW);
            pwm_set_duty(PWM_R2,Turn.PWM_Rout);
     }
     else
     {
            gpio_set_level(DIR_R2, GPIO_HIGH);
            pwm_set_duty(PWM_R2,-Turn.PWM_Rout);
      }


}
void stop(){
    if(EndTime==0)
        EndTime=1;
}
