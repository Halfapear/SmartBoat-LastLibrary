/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library ����CH32V307VCT6 ��Դ�⣩��һ�����ڹٷ� SDK �ӿڵĵ�������Դ��
* Copyright (c) 2022 SEEKFREE ��ɿƼ�
*
* ���ļ���CH32V307VCT6 ��Դ���һ����
*
* CH32V307VCT6 ��Դ�� ��������
* �����Ը��������������ᷢ���� GPL��GNU General Public License���� GNUͨ�ù������֤��������
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
* ����Դ��ʹ�� GPL3.0 ��Դ���֤Э�� �����������Ϊ���İ汾
* �������Ӣ�İ��� libraries/doc �ļ����µ� GPL3_permission_statement.txt �ļ���
* ���֤������ libraries �ļ����� �����ļ����µ� LICENSE �ļ�
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
#define RING                  (0)
#define TFT                     (0)
#define WIRELESS        (0)
//#define SPI                 (0)
uint8 count = 0;
//б��1.3188
//25�г���19
//119�г���167
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // ��ʼ��оƬʱ�� ����Ƶ��Ϊ 120MHz
    debug_init();                       // ��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
   /* wireless_uart_init();
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("start.\r\n");  */            // ��ʼ������ ���������Ϣ
    // �˴���д�û����� ���������ʼ�������
    All_Init();
    pit_ms_init(PIT_CH, 1);                                                  // ��ʼ�� PIT_CH0 Ϊ�����ж� 1ms ����
    interrupt_set_priority(PIT_PRIORITY, 0);                                    // ���� PIT1 �������жϵ��ж����ȼ�Ϊ 0

    //float duty=90;
    // �˴���д�û����� ���������ʼ�������
    uint8_t List_Number=1;
    //    * List_Number_p=&List_Number;
    List_Number_p=&List_Number;
//#if TFT
//    tft180_set_dir(TFT180_CROSSWISE);
//    tft180_set_color(RGB565_RED, RGB565_BLACK);
//    tft180_init();
//#endif
//    uint8_t Menu_List=10;
//    key_init (5);
//    exti_initconfig ();

    while(1)
    {
        // �˴���д��Ҫѭ��ִ�еĴ���
        key_scanner();

        //set_brushless_duty(bl_duty);
#if TFT

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
#endif
        key_clear_all_state();

        if(mt9v03x_finish_flag==1)
        {
            turn_to_bin();
            mt9v03x_finish_flag=0;

        }
        image_process();
        //Get_Four_jiao();
        GetLostTime();//��ȡһЩ������־λ
        Get_Edge_Point();
         //ring��־�仯������Ϊ�ж�Ҫ�õ����ұ߽�
         //����ǲ����ߵĻ���ֱ�Ӳ�˳����м���
         //�����ͼ����в��ߣ���Ҫ��*��*��*��
#if RING
         if(rd.Ring_Flag==0&&Cross_Flag==0){//Բ������
               Ring_Search();
              if(rd.Ring_Flag!=0)
                 rd.state=1;
           }
           else if(rd.Ring_Flag==1){//��
                // Ring_Count++;
                 Left_Ring();
            }
            else if(rd.Ring_Flag==2){//�һ�
                // Ring_Count++;
                 Right_Ring();//д���ˣ����о��ܲ�����
            }
#endif
#if 0
         if(rd.Ring_Flag==0&&Cross_Flag==0&&Ring_Count<=0){//Բ������
               Ring_Search();
               if(rd.Ring_Flag!=0)
                    rd.state=1;
         }
         else if(rd.Ring_Flag==1){//��
               Ring_Count++;
               Add_line_from_right();
               if(abs(FJ_Angle)>75){
                   rd.state=9;//�����������
                   rd.Ring_Flag=0;
                   FJ_Angle=0;
               }
         }
         else if(rd.Ring_Flag==2){//�һ�
               Ring_Count++;
               Add_line_from_left();
               if(abs(FJ_Angle)>75){
                   rd.state=9;
                   rd.Ring_Flag=0;
                   FJ_Angle=0;
               }
         }
#endif
         //Find_Up_Point( MT9V03X_H-1, 10 );
         //Find_Down_Point(MT9V03X_H-5,40);
         Cross_Detect();
         if(s>20)
             crosswalk();
         if(s>14&&s<30&&r_border[80]>100)
             Add_line_from_right();

         //�������������

         max_row=findMaxTransitionFromWhiteToBlack(40, 120);
           if(max_row>=90&&rd.Ring_Flag==0&&Cross_Flag==0){
                          Speed.Set_Speed=Speed.zhidao_Speed;
             }
             else  {
                          Speed.Set_Speed=Speed.wandao_Speed;
             }
//         if(s<0){
//             Turn.P=3.5;
//             //Turn.I=0.35;
//             Speed.Set_Speed=2800;
//             Speed.wandao_Speed=2800;
//             Speed.zhidao_Speed=2800;
//             max_angle=110;
//             //Turn.D=3;
//         }
//         else
//         {
//             //Speed.Set_Speed=3500;
//             Speed.Set_Speed=2800;
//             Speed.wandao_Speed=3500;
//             Speed.zhidao_Speed=3500;
//             max_angle=200;
//             Turn.P=1.2;
//                 Turn.I=0.2;
//                 Turn.D=2;
//         }
         GetCenterline();
         Turn.Chazhi=Err_Sum();
         /*if(Turn.Chazhi>18||Turn.Chazhi<-18)
           {
                 Speed.Set_Speed=Speed.wandao_Speed;
            }
           else
           {
                 Speed.Set_Speed=Speed.zhidao_Speed;
           }*/
//         if(straight==0)
//           {
//                 Speed.Set_Speed=Speed.wandao_Speed;
//            }
//           else
//           {
//                 Speed.Set_Speed=Speed.zhidao_Speed;
//           }

         if(garageout_flag==1)
             stop();

/*#if WIRELESS
        func_float_to_str(TP,Turn.turnP,2);
        func_float_to_str(TD,Turn.turnD,2);
        func_float_to_str(TI,Turn.turnI,2);
        wireless_uart_send_string("\r\nTurnP:");
        wireless_uart_send_string(TP);
        wireless_uart_send_byte('\r');
        wireless_uart_send_byte('\n');
        wireless_uart_send_string("\r\nTurnI:");
        wireless_uart_send_string(TI);
        wireless_uart_send_byte('\r');
        wireless_uart_send_byte('\n');
        wireless_uart_send_string("\r\nTurnD:");
        wireless_uart_send_string(TI);
        wireless_uart_send_byte('\r');
        wireless_uart_send_byte('\n');
#endif*/


        // �˴���д��Ҫѭ��ִ�еĴ���
    }
}

