/*********************************************************************************************************************
* CH32V307VCT6 Opensourec Library 即（CH32V307VCT6 开源库）是一个基于官方 SDK 接口的第三方开源库
* Copyright (c) 2022 SEEKFREE 逐飞科技
*
* 本文件是CH32V307VCT6 开源库的一部分
*
* CH32V307VCT6 开源库 是免费软件
* 您可以根据自由软件基金会发布的 GPL（GNU General Public License，即 GNU通用公共许可证）的条款
* 即 GPL 的第3版（即 GPL3.0）或（您选择的）任何后来的版本，重新发布和/或修改它
*
* 本开源库的发布是希望它能发挥作用，但并未对其作任何的保证
* 甚至没有隐含的适销性或适合特定用途的保证
* 更多细节请参见 GPL
*
* 您应该在收到本开源库的同时收到一份 GPL 的副本
* 如果没有，请参阅<https://www.gnu.org/licenses/>
*
* 额外注明：
* 本开源库使用 GPL3.0 开源许可证协议 以上许可申明为译文版本
* 许可申明英文版在 libraries/doc 文件夹下的 GPL3_permission_statement.txt 文件中
* 许可证副本在 libraries 文件夹下 即该文件夹下的 LICENSE 文件
* 欢迎各位使用并传播本程序 但修改内容时必须保留逐飞科技的版权声明（即本声明）
*
* 文件名称          main
* 公司名称          成都逐飞科技有限公司
* 版本信息          查看 libraries/doc 文件夹内 version 文件 版本说明
* 开发环境          MounRiver Studio V1.8.1
* 适用平台          CH32V307VCT6
* 店铺链接          https://seekfree.taobao.com/
*
* 修改记录
* 日期                                      作者                             备注
* 2022-09-15        大W            first version
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
#define PIT_CH                  (TIM3_PIT)                                      // 使用的周期中断编号 如果修改 需要同步对应修改周期中断编号与 isr.c 中的调用
#define PIT_PRIORITY            (TIM3_IRQn)                                      // 对应周期中断的中断编号
#define RING                  (0)
#define TFT                     (0)
#define WIRELESS        (0)
//#define SPI                 (0)
uint8 count = 0;
//斜率1.3188
//25行长度19
//119行长度167
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口
   /* wireless_uart_init();
    wireless_uart_send_byte('\r');
    wireless_uart_send_byte('\n');
    wireless_uart_send_string("start.\r\n");  */            // 初始化正常 输出测试信息
    // 此处编写用户代码 例如外设初始化代码等
    All_Init();
    pit_ms_init(PIT_CH, 1);                                                  // 初始化 PIT_CH0 为周期中断 1ms 周期
    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0

    //float duty=90;
    // 此处编写用户代码 例如外设初始化代码等
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
        // 此处编写需要循环执行的代码
        key_scanner();

        //set_brushless_duty(bl_duty);
#if TFT

               if(in_second_menu == 0){
                        tft180_show_uint (0,16*7,List_Number ,9);
                        Display_Initial_Menu();
                        Button_Up_Click(Menu_List,List_Number_p);
                        Button_Down_Click(Menu_List,List_Number_p);

                        //等一下，这个消除中断状态应该放在哪
                        //消一下key4中断状态
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
        GetLostTime();//获取一些辅助标志位
        Get_Edge_Point();
         //ring标志变化处，因为判断要用到左右边界
         //如果是补边线的话，直接补顺序进行即可
         //如果对图像进行补线，需要重*新*搜*线
#if RING
         if(rd.Ring_Flag==0&&Cross_Flag==0){//圆环搜索
               Ring_Search();
              if(rd.Ring_Flag!=0)
                 rd.state=1;
           }
           else if(rd.Ring_Flag==1){//左环
                // Ring_Count++;
                 Left_Ring();
            }
            else if(rd.Ring_Flag==2){//右环
                // Ring_Count++;
                 Right_Ring();//写完了，但感觉跑不起来
            }
#endif
#if 0
         if(rd.Ring_Flag==0&&Cross_Flag==0&&Ring_Count<=0){//圆环搜索
               Ring_Search();
               if(rd.Ring_Flag!=0)
                    rd.state=1;
         }
         else if(rd.Ring_Flag==1){//左环
               Ring_Count++;
               Add_line_from_right();
               if(abs(FJ_Angle)>75){
                   rd.state=9;//清除陀螺仪用
                   rd.Ring_Flag=0;
                   FJ_Angle=0;
               }
         }
         else if(rd.Ring_Flag==2){//右环
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

         //参数都在这里调

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


        // 此处编写需要循环执行的代码
    }
}

