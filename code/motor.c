#include "motor.h"
#include "common.h"
#define ENCODER_QUADDEC                 (TIM9_ENCOEDER)

//无刷左
#define PWM_D1          (TIM1_PWM_MAP3_CH1_E9)
#define DIR_D1             (TIM1_PWM_MAP3_CH2_E11) //仅拉高提供高电位
//无刷右
#define PWM_D2          (TIM8_PWM_MAP0_CH1_C6)
#define DIR_D2             (TIM8_PWM_MAP0_CH2_C7)

//转向左
#define PWM_L1           (TIM4_PWM_MAP1_CH3_D14)
#define DIR_L1              (D15)
//推进左
#define PWM_L2           (TIM4_PWM_MAP1_CH2_D13)
#define DIR_L2              (D12)

//转向右
#define PWM_R1           (TIM2_PWM_MAP0_CH4_A3)
#define DIR_R1              (A2)
//推进右
#define PWM_R2           (TIM2_PWM_MAP0_CH1_A0)
#define DIR_R2              (A1)

Speed_ct Speed;
PWM_Output PWM;
Turn_ct Turn;
int16 bl_duty=0;

int16 maxspeed=6000;
int16 max_angle=150;


int16 zhuanjiaozhi=0;
int16 base_speed=1000;
int16 turnmax=2000;
int16 turnmin=-2000;

void Para_init()
{
    bl_duty=880;//无刷电机调速
    PWM.Left_Out=0;
    PWM.Right_Out=0;
    Speed.Set_Speed=3500;
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

    Speed.P=1.1;
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

    Turn.P=1.5;
    Turn.I=0.5;
    Turn.D=2;
}
//编码器速度获取与处理
//测速轮直径-3.3cm
void GetSpeed()
{
    encoder_data_quaddec = encoder_get_count(ENCODER_QUADDEC);                  // 获取编码器计数
    Speed.Speed_Now=encoder_data_quaddec*100;
    encoder_clear_count(ENCODER_QUADDEC);                                       // 清空编码器计数
    Speed.Speed_Car=0.9*Speed.Speed_Now+0.1*Speed.Speed_Old;        //减小抖动
    Speed.Speed_Old=Speed.Speed_Now;

}
//积分限度
float constrain_float(float amt, float low, float high)
{
    return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}
//速度环pid
void SpeedPID_Control()
{
    Speed.Error=(Speed.Set_Speed-Speed.Speed_Car);
    Speed.Integral+=Speed.Error;
    Speed.Integral=constrain_float(Speed.Integral,-300,300);
    Speed.P_Error=Speed.Error;                                          //比例环节
    Speed.I_Error=Speed.Integral;                    //积分环节
    Speed.D_Error=Speed.Error-Speed.L_Error;                            //微分环节

    Speed.Output_PWM=Speed.P*Speed.P_Error+Speed.I*Speed.I_Error+Speed.D*Speed.D_Error;
    if(Speed.Output_PWM>4000)
        Speed.Output_PWM=4000;
    else if(Speed.Output_PWM<-4000)
        Speed.Output_PWM=-4000;

}
//转向环pd
void TurnPD_Control()
{

    Turn.error=0.9*Turn.Chazhi+0.1*Turn.last_error;
    if(Turn.error<2&&Turn.error>-2)//响应下限
    {
        Turn.error=0;
    }
    Turn.intergrator+=Turn.error;
    Turn.intergrator=constrain_float(Turn.intergrator,-40,40);
    Turn.PWM_Dout=Turn.P*Turn.error+Turn.intergrator*Turn.I+Turn.D*(Turn.error-Turn.last_error);
    Turn.last_error = Turn.error;
    if(Turn.PWM_Dout>100)
        Turn.PWM_Dout=100;
    else if(Turn.PWM_Dout<-100)
        Turn.PWM_Dout=-100;


}
//设置无刷电机的转速，duty范围500-1000
void set_brushless_duty(int16 duty)
{
    pwm_set_duty(PWM_D1, duty);
    pwm_set_duty(PWM_D2, duty);
}
/*
void PWM_Out()
{
    //速度环输出
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
    //速度环输出
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

    }
    else {
        gpio_set_level(DIR_L2, GPIO_HIGH);
        pwm_set_duty(PWM_L2,-Turn.PWM_Lout);
    }

    if(Turn.PWM_Rout>0)
        {
            gpio_set_level(DIR_R2, GPIO_LOW);
            pwm_set_duty(PWM_R2,Turn.PWM_Rout);
        }
        else {
            gpio_set_level(DIR_R2, GPIO_HIGH);
            pwm_set_duty(PWM_R2,-Turn.PWM_Rout);
        }


}
void stop(){
    Speed.Set_Speed = 0;
}
