/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ���������
* �����Ը���������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù�������֤��������
* �� GPL �ĵ�3�棨�� GPL3.0������ѡ��ģ��κκ����İ汾�����·�����/���޸���
*
* ����Դ��ķ�����ϣ�����ܷ������ã�����δ�������κεı�֤
* ����û�������������Ի��ʺ��ض���;�ı�֤
* ����ϸ����μ� GPL
*
* ��Ӧ�����յ�����Դ���ͬʱ�յ�һ�� GPL �ĸ���
* ���û�У������<https://www.gnu.org/licenses/>
*
* ����ע����
* ����Դ��ʹ�� GPL3.0 ��Դ����֤Э�� ������������Ϊ���İ汾
* ��������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ����֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
* ��ӭ��λʹ�ò����������� ���޸�����ʱ���뱣����ɿƼ��İ�Ȩ����������������
*
* �ļ�����          main
* ��˾����          �ɶ���ɿƼ����޹�˾
* �汾��Ϣ          �鿴 libraries/doc �ļ����� version �ļ� �汾˵��
* ��������          MounRiver Studio V1.8.1
* ����ƽ̨          CH32V307VCT6
* ��������          https://seekfree.taobao.com/
*
* �޸ļ�¼
* ����                                      ����                             ��ע
* 2022-09-15        ��W            first version
********************************************************************************************************************/
#include "zf_common_headfile.h"
#include "common.h"
#include "motor.h"
#include "servo.h"
#include "image.h"
#include "camera.h"
#include "tft_show.h"

#define LED1                    (E2)
#define BEEP                (C13)
#define PIT_CH                  (TIM3_PIT)                                      // ʹ�õ������жϱ�� ����޸� ��Ҫͬ����Ӧ�޸������жϱ���� isr.c �еĵ���
#define PIT_PRIORITY            (TIM3_IRQn)                                      // ��Ӧ�����жϵ��жϱ��
uint8 count = 0;
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                       // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���

    // �˴���д�û����� ���������ʼ�������
    All_Init();
    pit_ms_init(PIT_CH, 1);                                                  // ��ʼ�� PIT_CH0 Ϊ�����ж� 1ms ����
    interrupt_set_priority(PIT_PRIORITY, 0);                                    // ���� PIT1 �������жϵ��ж����ȼ�Ϊ 0

    uint16_t data[128];
    int16_t data_index = 0;
    //float duty=90;
    // �˴���д�û����� ���������ʼ�������
    uint8_t List_Number=1;
    //    * List_Number_p=&List_Number;
    List_Number_p=&List_Number;

    tft180_set_dir(TFT180_CROSSWISE);
    tft180_set_color(RGB565_RED, RGB565_BLACK);
    tft180_init();

    uint8_t Menu_List=5;
    key_init (5);
    exti_initconfig ();
    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        key_scanner();

        set_brushless_duty(bl_duty);

        if(garageout_flag == 0){
               if(in_second_menu == 0){
                        tft180_show_uint (0,16*7,List_Number ,9);
                        Display_Initial_Menu();
                        Button_Up_Click(Menu_List,List_Number_p);
                        Button_Down_Click(Menu_List,List_Number_p);

                        //��һ�£���������ж�״̬Ӧ�÷�����
                        //��һ��key4�ж�״̬
                        exti_state[3] = 0;
                        count = 0;
                    }
                    else {
                        if(count ==0){
                        tft180_clear();
                        count++;
                        }
                        Display_Second_Menu(List_Number_p);
                    }
                }
                else {
                    stop();
                }
        key_clear_all_state();
        if(mt9v03x_finish_flag==1)
        {
            mt9v03x_finish_flag=0;
            image_process();

            Turn.Chazhi=Err_Sum();

        }


        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}
