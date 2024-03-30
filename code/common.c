#include "zf_common_headfile.h"
#include "common.h"
#include "motor.h"
#include "servo.h"
#include "camera.h"
#include "gyro.h"

#define LED1_PIN E2
#define BEEP                (C13)

//��ˢ��
#define PWM_D1          (TIM1_PWM_MAP3_CH1_E9)
#define DIR_D1             (E11) //�������ṩ�ߵ�λ
//��ˢ��
#define PWM_D2          (TIM8_PWM_MAP0_CH1_C6)
#define DIR_D2             (C7)

//ת����
#define PWM_L1           (TIM4_PWM_MAP1_CH2_D13)
#define DIR_L1              (D12)
//�ƽ���
#define PWM_L2           (TIM4_PWM_MAP1_CH3_D14)
#define DIR_L2              (D15)

//ת����
#define PWM_R1           (TIM2_PWM_MAP0_CH1_A0)
#define DIR_R1              (A1)
//�ƽ���
#define PWM_R2           (TIM2_PWM_MAP0_CH4_A3)
#define DIR_R2              (A2)

//����������
#define ENCODER_QUADDEC                 (TIM9_ENCOEDER)                                 // ������������Ӧʹ�õı������ӿ�
#define ENCODER_QUADDEC_A               (TIM9_ENCOEDER_MAP3_CH1_D9)             // A ���Ӧ������
#define ENCODER_QUADDEC_B               (TIM9_ENCOEDER_MAP3_CH2_D11)            // B ���Ӧ������
int16 ms=0;
int32 s=0;
int16 b_s;
int16 encoder_data_quaddec = 0;
int16 encoder_derdate=0;

void All_Init()
{
    system_delay_ms(50);//ϵͳ��ʱ
    gpio_init(LED1_PIN, GPO, 0, GPO_PUSH_PULL);

    gpio_init(BEEP, GPO, GPIO_LOW, GPO_PUSH_PULL);


    //��ˢ��ʼ��
    pwm_init(PWM_D1,50,0);
    pwm_init(PWM_D2,50,0);
    gpio_init(DIR_D1,GPO,1,GPO_PUSH_PULL);
    gpio_init(DIR_D2,GPO,1,GPO_PUSH_PULL);
            // ������ˢ���ת��   ��1ms - 2ms��/20ms * 10000��10000��PWM����ռ�ձ�ʱ���ֵ��
            // ��ˢ���ת�� 0%   Ϊ 500
            // ��ˢ���ת�� 20%  Ϊ 600
            // ��ˢ���ת�� 40%  Ϊ 700
            // ��ˢ���ת�� 60%  Ϊ 800
            // ��ˢ���ת�� 80%  Ϊ 900
            // ��ˢ���ת�� 100% Ϊ 1000
    //��ˢ��ʼ��
    pwm_init(PWM_L1,17000,0);
    gpio_init(DIR_L1,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_L2,17000,0);
    gpio_init(DIR_L2,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_R1,17000,0);
    gpio_init(DIR_R1,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    pwm_init(PWM_R2,17000,0);
    gpio_init(DIR_R2,GPO, GPIO_HIGH, GPO_PUSH_PULL);
    //��������ʼ��
    encoder_quad_init(ENCODER_QUADDEC, ENCODER_QUADDEC_A, ENCODER_QUADDEC_B);

    //�����ǳ�ʼ��
    imu660ra_init();
    Zero_Point_Detect();

    mt9v03x_init();
    Para_init();
    Round_init();
}
