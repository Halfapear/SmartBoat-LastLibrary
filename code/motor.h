#ifndef SRC_APPSW_TRICORE_USER_MOTOR_H_
#define SRC_APPSW_TRICORE_USER_MOTOR_H_
#include "zf_common_headfile.h"

typedef struct
{
    float Chazhi,Chazhi_old;
    float Hchazhi,Hchazhi_old;
    float PWM_Dadd,PWM_Dadd_old;
    float PWM_Dout;
    float PWM_Lout;
    float PWM_Rout;
    //----------------����ת���йر���------------------//
    float Angle_Z_Offset;         //z��ƫ���ٶ�,�ڻ���nowspeed
    float Direction_Output_old;   //��һ�ε�ת�����
    float Direction_Output_new;   //����ת�����

    float Direction_Pouter;      //�⻷p���
    float Direction_Douter;      //�⻷D���
    float Direction_P,Direction_D;
    float Huan_In_P,Ramp_Up_P;
    float Garage_P,Huan_Out_P,Ramp_Down_P;
    float Direction_OuterPut;

    float Direction_Pinner;   //�ڻ�p���
    float Direction_Dinner;   //�ڻ�D���
    float Direction_Kp,Direction_Kd;
    float Direction_InnerPut;
    //pid_param_t pd;//����ͷ��ֵ
    float error,last_error;
    float P,I,D;
    float intergrator;
    float turnP,turnD,turnI;

}Turn_ct;

typedef struct
{
    float Set_Speed,Speed_Max;
    float P,I,D,Integral;
    float Speed_Car,normal_Speed,zhidao_Speed,wandao_Speed,Speed_Now,Speed_Old,podao_up_Speed,podao_down_Speed,Huan_In_Speed,Huan_Out_Speed,Garage_Speed,Sancha_Speed,ruku_Speed;
    float Distance;
    float Enc_Right,Enc_Left,Enc_Car;
    int   Car_Start,Car_Stop;

    float derivative;
    float Error,P_Error,I_Error,D_Error,L_Error;
    float PWM_add,PWM_Ladd,Output_PWM,Output_LPWM,PWM_Limit;
}Speed_ct;

typedef struct
{
    float Left_Out,Right_Out;
}PWM_Output;
extern int16 bl_duty;
extern int16 zhuanjiaozhi;
extern int16 max_angle;
extern PWM_Output PWM;
extern Speed_ct Speed;
extern Turn_ct Turn;
extern void GetSpeed();
extern void Para_init();
extern void SpeedPID_Control();
extern void TurnPD_Control();
extern void PWM_Out();
extern void set_brushless_duty(int16 duty);
void stop();
#endif
