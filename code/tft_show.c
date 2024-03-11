#include "zf_common_headfile.h"
#include "tft_show.h"
#include "motor.h"
#include "image.h"
#include "camera.h"

#define LED1                (E2 )
uint8_t *List_Number_p;
#define image_h 120//图像高度
#define image_w 188//图像宽度
#define uesr_RED     0XF800    //红色
#define uesr_GREEN   0X07E0    //绿色
#define uesr_BLUE    0X001F    //蓝色

#define KEY3                    B12                                          // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用
#define KEY4                    B0                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用



uint8 exti_state[4];
uint8 in_second_menu=0;

uint16 loop_time = 0;





void exti_initconfig (void){
exti_init(KEY3, EXTI_TRIGGER_BOTH);                                         // 初始化 KEY3 为外部中断输入 双边沿触发
exti_init(KEY4, EXTI_TRIGGER_BOTH);                                         // 初始化 KEY4 为外部中断输入 双边沿触发

interrupt_set_priority(KEY3_EXTI, (0<<5) || 1);                             // 设置 KEY3 对应外部中断的中断抢占优先级0，子优先级1
interrupt_set_priority(KEY4_EXTI, (0<<5) || 1);                             // 设置 KEY4 对应外部中断的中断抢占优先级0，子优先级1
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     KEY3 的按键外部中断处理函数 这个函数将在 KEY3 引脚对应的外部中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     key3_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key3_exti_handler (void)
{
    if(gpio_get_level(KEY3) == 0)
    {
        in_second_menu = 1;
        exti_state[2] = 1;                                                      // 外部中断触发 标志位置位
    }
}

//-------------------------------------------------------------------------------------------------------------------
// 函数简介     KEY4 的按键外部中断处理函数 这个函数将在 KEY4 引脚对应的外部中断调用 详见 isr.c
// 参数说明     void
// 返回参数     void
// 使用示例     key4_exti_handler();
//-------------------------------------------------------------------------------------------------------------------
void key4_exti_handler (void)

{
    if(gpio_get_level(KEY4) == 0)
    {
        //这个二级菜单判断应该放在二级菜单的循环
        in_second_menu = 0;                                                     // 设置标志为0，指示退出二级菜单
        //应该不会不明白，这个数组就是存标识的
        exti_state[3] = 1;                                                      // 外部中断触发 标志位置位
    }
}

/*
      * @brief  向上翻页，效果是显示当前所在项目数字
      Button_Click_Event_1 就是 某一个按键 比如第一个
      x y 什么的都还有看图配置
      * @param  Menu_List 总共有多少项
      * @retval 无
*/
    //括号内是period（虽然我不太清楚什么时候用init）
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
      * @brief  向下翻页，效果是显示当前所在项目数字
      * @param  Menu_List 总共有多少项
      * @retval 无
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
      * @brief  显示灰度图像（例程中竟然有，太省事了）
      * @param  无
      * @retval 无
 */

void Display_Grayscale_Image(void)
{
    tft180_set_dir(TFT180_CROSSWISE);                                           // 需要先横屏 不然显示不下

            //tft180_show_int(0,16*4,encoder_data_quaddec ,5);

    //tft180_show_gray_image(0,0,mt9v03x_image[0],image_w,image_h,160,128,0);
    tft180_show_gray_image(0,0,bin_image[0],image_w,image_h,160,128,0);
    tft180_show_int(0,0,Speed.Speed_Car,4);
                tft180_show_int(0,16,Speed.P_Error,5);

                tft180_show_int(0,16*2,Turn.PWM_Lout,5);
                tft180_show_int(0,16*3,Turn.PWM_Rout,5);
    for (int16 i = 0; i <image_h-1; i++)
       {
        tft180_draw_point(94,i,RGB565_YELLOW);
        if(center_line[i]<160){
                tft180_draw_point(center_line[i], i, uesr_RED);//显示起点 显示中线
                }
                if(l_border[i]<160)
                {
                    tft180_draw_point(l_border[i], i, uesr_GREEN);//显示起点 显示左边线
                }
                if(r_border[i]<160)
                {
                    tft180_draw_point(r_border[i], i, uesr_BLUE);//显示起点 显示右边线
                }
       }

    }

//几个固定的放上面



/*
      * @brief  显示初始菜单，已经配置好
      * @param  无
      * @retval 无
*/

void Display_Initial_Menu()
{
    tft180_show_string ( 0,  16*0, "1.This is a menu");
    tft180_show_string ( 0,  16*1, "2.Grayscale Image");
    tft180_show_string ( 0,  16*2, "3.Speed.P");
    tft180_show_string ( 0,  16*3, "4.Speed.I");
    tft180_show_string ( 0,  16*4, "5.Speed.D");
    tft180_show_string ( 0,  16*5, "6.Turn.P");
    //tft180_show_string ( 0,  16*6, "7.Turn.D");
    tft180_show_int (0, 16*6,  encoder_data_quaddec,4);
//不是 显示汉字和显示字符串有什么区别：这个函数可能不支持直接显示非ASCII字符，比如汉字等
}

/*
ps：你问接下来二级界面呢？————估计由* List_Number_p过渡吧
比如：if(* List_Number_p = 2){
    Display_Color Image();
}
*/


/*
      * @brief  调节电机1的PWM值
      * @param  Pwm的Compare地址
      * @retval 指针直接传回去了
 */




void Adjust_SpeedP(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.P += 0.1; // 增加步长为0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.P -= 0.1; // 减少步长为0.1
        if (Speed.P < 0) Speed.P = 0; // 防止下溢
    }
    tft180_show_string ( 0,  16*0, "3.Speed.P");
    tft180_show_float(0, 16*2, Speed.P, 1, 6);
}

void Adjust_SpeedI(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.I += 0.01; // 增加步长为0.01
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.I -= 0.01; // 减少步长为0.01
        if (Speed.I < 0) Speed.I = 0; // 防止下溢
    }
}

void Adjust_SpeedD(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Speed.D += 0.05; // 增加步长为0.05
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Speed.D -= 0.05; // 减少步长为0.05
        if (Speed.D < 0) Speed.D = 0; // 防止下溢
    }
}

void Adjust_TurnP(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Turn.P += 0.1; // 增加步长为0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Turn.P -= 0.1; // 减少步长为0.1
        if (Turn.P < 0) Turn.P = 0; // 防止下溢
    }
}

void Adjust_TurnD(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Turn.D += 0.1; // 增加步长为0.1
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Turn.D -= 0.1; // 减少步长为0.1
        if (Turn.D < 0) Turn.D = 0; // 防止下溢
    }
}


/*
//      * @brief    按键3，按下它就调用* List_Number_p对应的函数
//      * @param  无
//      * @retval 无
*/
void Display_Second_Menu(uint8_t *List_Number_p){
            if(*List_Number_p ==2)
            {
                //想实现的功能
                Display_Grayscale_Image();

                // 清空外部中断触发标志位


            }
            else if(*List_Number_p==3)
            {
                //想实现的功能
                Adjust_SpeedP();
                // 清空外部中断触发标志位
                exti_state[3] = 0;
            }
            else if(*List_Number_p==4)
            {
                //想实现的功能
                Adjust_SpeedI();
                // 清空外部中断触发标志位
                exti_state[3] = 0;
            }
            else if(*List_Number_p==5)
            {
                //想实现的功能
                Adjust_SpeedD();
                // 清空外部中断触发标志位
                exti_state[3] = 0;
            }
            else if(*List_Number_p==6)
            {
                //想实现的功能
                Adjust_TurnP();
                // 清空外部中断触发标志位
                exti_state[3] = 0;
            }
            else if(*List_Number_p==7)
            {
                //想实现的功能
                Adjust_TurnD();
                // 清空外部中断触发标志位
                exti_state[3] = 0;
            }
            //消一下key3中断状态
            exti_state[2] = 0;
}


