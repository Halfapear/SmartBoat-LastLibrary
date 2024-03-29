#include "zf_common_headfile.h"
#include "common.h"
#include "motor.h"
#include "servo.h"
#include "camera.h"
#include "gyro.h"

#define LED1_PIN E2
#define BEEP                (C13)

//无刷左
#define PWM_D1          (TIM1_PWM_MAP3_CH1_E9)
#define DIR_D1             (E11) //仅拉高提供高电位
//无刷右
#define PWM_D2          (TIM8_PWM_MAP0_CH1_C6)
#define DIR_D2             (C7)

//转向左
#define PWM_L1           (TIM4_PWM_MAP1_CH2_D13)
#define DIR_L1              (D12)
//推进左
#define PWM_L2           (TIM4_PWM_MAP1_CH3_D14)
#define DIR_L2              (D15)

//转向右
#define PWM_R1           (TIM2_PWM_MAP0_CH1_A0)
#define DIR_R1              (A1)
//推进右
#define PWM_R2           (TIM2_PWM_MAP0_CH4_A3)
#define DIR_R2              (A2)

//正交编码器
#define ENCODER_QUADDEC                 (TIM9_ENCOEDER)                                 // 正交编码器对应使用的编码器接口
#define ENCODER_QUADDEC_A               (TIM9_ENCOEDER_MAP3_CH1_D9)             // A 相对应的引脚
#define ENCODER_QUADDEC_B               (TIM9_ENCOEDER_MAP3_CH2_D11)            // B 相对应的引脚
int16 ms=0;
int32 s=0;
int16 b_s;
int16 encoder_data_quaddec = 0;
int16 encoder_derdate=0;

void All_Init()
{
    system_delay_ms(50);//系统延时
    gpio_init(LED1_PIN, GPO, 0, GPO_PUSH_PULL);

    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);


    //无刷初始化
    pwm_init(PWM_D1,50,0);
    pwm_init(PWM_D2,50,0);
    gpio_init(DIR_D1,GPO,1,GPO_PUSH_PULL);
    gpio_init(DIR_D2,GPO,1,GPO_PUSH_PULL);
            // 计算无刷电调转速   （1ms - 2ms）/20ms * 10000（10000是PWM的满占空比时候的值）
            // 无刷电调转速 0%   为 500
            // 无刷电调转速 20%  为 600
            // 无刷电调转速 40%  为 700
            // 无刷电调转速 60%  为 800
            // 无刷电调转速 80%  为 900
            // 无刷电调转速 100% 为 1000
    //有刷初始化
    pwm_init(PWM_L1,17000,0);
    gpio_init(DIR_L1,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_L2,17000,0);
    gpio_init(DIR_L2,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_R1,17000,0);
    gpio_init(DIR_R1,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_R2,17000,0);
    gpio_init(DIR_R2,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    //编码器初始化
    encoder_quad_init(ENCODER_QUADDEC, ENCODER_QUADDEC_A, ENCODER_QUADDEC_B);

    //陀螺仪初始化
    imu660ra_init();
    Zero_Point_Detect();

    mt9v03x_init();
    Para_init();
    Round_init();
}
