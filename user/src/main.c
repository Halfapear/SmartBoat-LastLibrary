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
uint8 count = 0;
int main (void)
{
    clock_init(SYSTEM_CLOCK_120M);      // 初始化芯片时钟 工作频率为 120MHz
    debug_init();                       // 务必保留，本函数用于初始化MPU 时钟 调试串口

    // 此处编写用户代码 例如外设初始化代码等
    All_Init();
    pit_ms_init(PIT_CH, 1);                                                  // 初始化 PIT_CH0 为周期中断 1ms 周期
    interrupt_set_priority(PIT_PRIORITY, 0);                                    // 设置 PIT1 对周期中断的中断优先级为 0

    //float duty=90;
    // 此处编写用户代码 例如外设初始化代码等
    uint8_t List_Number=1;
    //    * List_Number_p=&List_Number;
    List_Number_p=&List_Number;

    tft180_set_dir(TFT180_CROSSWISE);
    tft180_set_color(RGB565_RED, RGB565_BLACK);
    tft180_init();

    uint8_t Menu_List=10;
    key_init (5);
    exti_initconfig ();
    while(1)
    {
        // 此处编写需要循环执行的代码
        key_scanner();

        //set_brushless_duty(bl_duty);

        if(garageout_flag == 0){
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
                }
                else {
                    stop();
                }
        key_clear_all_state();

        if(mt9v03x_finish_flag==1)
        {
            mt9v03x_finish_flag=0;
            image_process();

            GetLostTime();//获取一些辅助标志位
            Get_Edge_Point();
            //ring标志变化处，因为判断要用到左右边界
            //如果是补边线的话，直接补顺序进行即可
            //如果对图像进行补线，需要重*新*搜*线
           /* if(rd.Ring_Flag==0){//圆环搜索
                Ring_Search();
                if(rd.Ring_Flag!=0)
                    rd.state=1;
            }
            else if(rd.Ring_Flag==1){//左环
                Left_Ring();
            }
            else if(rd.Ring_Flag==2){//右环
                Right_Ring();//写完了，但感觉跑不起来
            }*/

            GetCenterline();
            Turn.Chazhi=Err_Sum();

        }


        // 此处编写需要循环执行的代码
    }
}

