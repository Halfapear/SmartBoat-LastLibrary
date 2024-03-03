#include "servo.h"
#define SERVO_DUTY(x)         ((float)PWM_DUTY_MAX / (1000.0 / (float)SERVO_FREQ) * (0.5 + (float)(x) / 90.0))
#define SERVO_PWM               (TIM2_PWM_MAP1_CH1_A15)                         // 定义主板上舵机对应引脚
#define SERVO_FREQ              (50 )                                           // 定义主板上舵机频率  请务必注意范围 50-300

#define SERVO_L_MAX             (80 )                                           // 定义主板上舵机活动范围 角度
#define SERVO_R_MAX             (100)                                           // 定义主板上舵机活动范围 角度
#if ((SERVO_FREQ < 50) || (SERVO_FREQ > 300))
    #error "SERVO_MOTOR_FREQ ERROE!"
#endif
float servo_motor_duty = 90.0;                                                  // 舵机动作角度
float servo_motor_dir = 1;                                                      // 舵机动作状态


void servo_Init()
{
    pwm_init(SERVO_PWM, SERVO_FREQ, 0);
    //pwm_set_duty(SERVO_PWM, (uint32)SERVO_DUTY(servo_motor_duty));
}
void servo_Set_duty(float duty)
{
    servo_motor_duty=duty;
    pwm_set_duty(SERVO_PWM, (uint32)SERVO_DUTY(servo_motor_duty));
}
