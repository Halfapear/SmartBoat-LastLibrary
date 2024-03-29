#include "zf_common_headfile.h"
#include "tft_show.h"
#include "motor.h"
#include "image.h"
#include "camera.h"
#include  "gyro.h"

#define LED1                (E2 )
uint8_t *List_Number_p;
#define image_h 120//ͼ��߶�
#define image_w 188//ͼ����
#define uesr_RED     0XF800    //��ɫ
#define uesr_GREEN   0X07E0    //��ɫ
#define uesr_BLUE    0X001F    //��ɫ

#define KEY3                    B12                                          // ʹ�õ��ⲿ�ж��������� ����޸� ��Ҫͬ����Ӧ�޸��ⲿ�жϱ���� isr.c �еĵ���
#define KEY4                    B0                                           // ʹ�õ��ⲿ�ж��������� ����޸� ��Ҫͬ����Ӧ�޸��ⲿ�жϱ���� isr.c �еĵ���



uint8 exti_state[4];
uint8 in_second_menu=0;

uint16 loop_time = 0;





void exti_initconfig (void){
exti_init(KEY3, EXTI_TRIGGER_BOTH);                                         // ��ʼ�� KEY3 Ϊ�ⲿ�ж����� ˫���ش���
exti_init(KEY4, EXTI_TRIGGER_BOTH);                                         // ��ʼ�� KEY4 Ϊ�ⲿ�ж����� ˫���ش���

interrupt_set_priority(KEY3_EXTI, (0<<5) || 1);                             // ���� KEY3 ��Ӧ�ⲿ�жϵ��ж���ռ���ȼ�0�������ȼ�1
interrupt_set_priority(KEY4_EXTI, (0<<5) || 1);                             // ���� KEY4 ��Ӧ�ⲿ�жϵ��ж���ռ���ȼ�0�������ȼ�1
}

//-------------------------------------------------------------------------------------------------------------------
// �������     KEY3 �İ����ⲿ�жϴ����� ����������� KEY3 ���Ŷ�Ӧ���ⲿ�жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     key3_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key3_exti_handler (void)
{
    if(gpio_get_level(KEY3) == 0)
    {
        in_second_menu = 1;
        exti_state[2] = 1;                                                      // �ⲿ�жϴ��� ��־λ��λ
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     KEY4 �İ����ⲿ�жϴ����� ����������� KEY4 ���Ŷ�Ӧ���ⲿ�жϵ��� ��� isr.c
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     key4_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key4_exti_handler (void)

{
    if(gpio_get_level(KEY4) == 0)
    {
        //��������˵��ж�Ӧ�÷��ڶ����˵���ѭ��
        in_second_menu = 0;                                                     // ���ñ�־Ϊ0��ָʾ�˳������˵�
        //Ӧ�ò��᲻���ף����������Ǵ��ʶ��
        exti_state[3] = 1;                                                      // �ⲿ�жϴ��� ��־λ��λ
    }
}

/*
      * @brief  ���Ϸ�ҳ��Ч������ʾ��ǰ������Ŀ����
      Button_Click_Event_1 ���� ĳһ������ �����һ��
      x y ʲô�Ķ����п�ͼ����
      * @param  Menu_List �ܹ��ж�����
      * @retval ��
*/
    //��������period����Ȼ�Ҳ�̫���ʲôʱ����init��
void Button_Down_Click(uint8_t Menu_List,uint8_t * List_Number_p)
{

    if (KEY_SHORT_PRESS == key_get_state(KEY_1))
    {
        if (* List_Number_p > 1)
            * List_Number_p = * List_Number_p - 1;
        else if (* List_Number_p == 1)
            * List_Number_p = 1;
        gpio_set_level(LED1, GPIO_LOW);
    }

}

/*
      * @brief  ���·�ҳ��Ч������ʾ��ǰ������Ŀ����
      * @param  Menu_List �ܹ��ж�����
      * @retval ��
 */
void Button_Up_Click(uint8_t Menu_List,uint8_t* List_Number_p)
{
//    key_scanner();
    if (KEY_SHORT_PRESS == key_get_state(KEY_2))
    {
        if (* List_Number_p < Menu_List)
            * List_Number_p = * List_Number_p + 1;
        else if (* List_Number_p == Menu_List)
            * List_Number_p = 5;
        gpio_set_level(LED1, GPIO_LOW);
    }

}


/*
      * @brief  ��ʾ�Ҷ�ͼ�������о�Ȼ�У�̫ʡ���ˣ�
      * @param  ��
      * @retval ��
 */

void Display_Grayscale_Image(void)
{
    tft180_set_dir(TFT180_CROSSWISE);                                           // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����

            //tft180_show_int(0,16*4,encoder_data_quaddec ,5);

    //tft180_show_gray_image(0,0,mt9v03x_image[0],image_w,image_h,160,128,0);
    tft180_show_gray_image(0,0,bin_image[0],image_w,image_h,160,128,0);
    tft180_show_int(0,0,rd.state,4);
                tft180_show_int(0,16,FJ_Angle,5);
                tft180_show_int(0,16*1,Turn.PWM_Lout,5);
                tft180_show_int(0,16*2,Turn.PWM_Rout,5);
                //tft180_show_float(0,16*2,rd.add_k,2,3);
                tft180_show_int(0,16*3,rd.right_up_guai,5);
    for (int16 i = 0; i <image_h-1; i++)
       {
        tft180_draw_point(94,i,RGB565_YELLOW);
        if(center_line[i]<160){
                tft180_draw_point(center_line[i], i, uesr_RED);//��ʾ��� ��ʾ����
                }
                if(l_border[i]<160)
                {
                    tft180_draw_point(l_border[i], i, uesr_GREEN);//��ʾ��� ��ʾ�����
                }
                if(r_border[i]<160)
                {
                    tft180_draw_point(r_border[i], i, uesr_BLUE);//��ʾ��� ��ʾ�ұ���
                }
       }

    }

//�����̶��ķ�����



/*
      * @brief  ��ʾ��ʼ�˵����Ѿ����ú�
      * @param  ��
      * @retval ��
*/

void Display_Initial_Menu()
{

    tft180_show_string ( 0,  16*0, "1.Grayscale Image");

    tft180_show_string ( 0,  16*1, "2.Speed.P");
    tft180_show_string ( 0,  16*2, "3.Speed.I");
    tft180_show_string ( 0,  16*3, "4.Speed.D");
    tft180_show_string ( 0,  16*4, "5.Turn.P");
    tft180_show_string ( 0,  16*5, "6.Turn.D");
    tft180_show_int (0, 16*6,  encoder_data_quaddec,4);
//���� ��ʾ���ֺ���ʾ�ַ�����ʲô��������������ܲ�֧��ֱ����ʾ��ASCII�ַ������纺�ֵ�
}

/*
ps�����ʽ��������������أ���������������* List_Number_p���ɰ�
���磺if(* List_Number_p = 2){
    Display_Color Image();
}
*/


/*
      * @brief  ���ڵ��1��PWMֵ
      * @param  Pwm��Compare��ַ
      * @retval ָ��ֱ�Ӵ���ȥ��
 */




void Adjust_SpeedP(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.P += 0.1; // ���Ӳ���Ϊ0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.P -= 0.1; // ���ٲ���Ϊ0.1
        if (Speed.P < 0) Speed.P = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "2.Speed.P");
    tft180_show_float(0, 16*2, Speed.P, 1, 6);
}

void Adjust_SpeedI(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.I += 0.01; // ���Ӳ���Ϊ0.01
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.I -= 0.01; // ���ٲ���Ϊ0.01
        if (Speed.I < 0) Speed.I = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "3.Speed.I");
    tft180_show_float(0, 16*2, Speed.I, 1, 6);
}

void Adjust_SpeedD(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.D += 0.05; // ���Ӳ���Ϊ0.05
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.D -= 0.05; // ���ٲ���Ϊ0.05
        if (Speed.D < 0) Speed.D = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "4.Speed.D");
    tft180_show_float(0, 16*2, Speed.D, 1, 6);
}

void Adjust_TurnP(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Turn.P += 0.1; // ���Ӳ���Ϊ0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Turn.P -= 0.1; // ���ٲ���Ϊ0.1
        if (Turn.P < 0) Turn.P = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "5.Turn_P");
    tft180_show_float(0, 16*2, Turn.P, 1, 6);
}

void Adjust_TurnD(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Turn.D += 0.1; // ���Ӳ���Ϊ0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Turn.D -= 0.1; // ���ٲ���Ϊ0.1
        if (Turn.D < 0) Turn.D = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "6.Turn_D");
    tft180_show_float(0, 16*2, Turn.D, 1, 6);
}

void Adjust_max_angle(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        max_angle += 10;
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        max_angle -= 10;
        if (max_angle < 0) Turn.D = 0; // ��ֹ����
    }
    tft180_show_string ( 0,  16*0, "7.max_angle");
    tft180_show_int(0, 16*2, max_angle, 4);
}

void Adjust_Set_Speed(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.Set_Speed += 500;
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.Set_Speed -= 500;
        if (Speed.Set_Speed < 0) Speed.Set_Speed = 0; // ��ֹ����
    }
    //现在还有bug：黑屏
    tft180_show_string ( 0,  16*0, "8.Set_Speed");
//    tft180_show_float(0, 16*2, Speed.Set_Speed, 1, 6);
}


/*
//      * @brief    ����3���������͵���* List_Number_p��Ӧ�ĺ���
//      * @param  ��
//      * @retval ��
*/
void Display_Second_Menu(uint8_t *List_Number_p){
            if(*List_Number_p ==1)
            {
                //��ʵ�ֵĹ���
                Display_Grayscale_Image();

                // ����ⲿ�жϴ�����־λ


            }
            else if(*List_Number_p==2)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedP();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==3)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedI();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==4)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedD();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==5)
            {
                //��ʵ�ֵĹ���
                Adjust_TurnP();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==6)
            {
                //��ʵ�ֵĹ���
                Adjust_TurnD();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==7)
            {
                //��ʵ�ֵĹ���
                Adjust_max_angle();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            //��һ��key3�ж�״̬
            exti_state[2] = 0;
}


