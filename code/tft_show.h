#ifndef __TFT_SHOW_H__
#define __TFT_SHOW_H__
#include "common.h"

#define KEY1                    (C9 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用
#define KEY2                    (D8 )                                           // 使用的外部中断输入引脚 如果修改 需要同步对应修改外部中断编号与 isr.c 中的调用

#define KEY3_EXTI               (EXTI15_10_IRQn)                                // 对应外部中断的中断编号
#define KEY4_EXTI               (EXTI0_IRQn)                                    // 对应外部中断的中断编号
#define image_h 120//图像高度
#define image_w 188//图像宽度

void Display_Initial_Menu(void);
void Button_Up_Click(uint8_t Menu_List,uint8_t* List_Number_p);
void Button_Down_Click(uint8_t Menu_List,uint8_t* List_Number_p);
void Display_Grayscale_Image(void);
void Display_Second_Menu(uint8_t* List_Number_p);

void Adjust_SpeedP(void);
void Adjust_SpeedI(void);
void Adjust_SpeedD(void);
void Adjust_TurnP(void);
void Adjust_TurnD(void);
void Adjust_max_angle(void);
void Adjust_Set_Speed(void);



extern uint8 in_second_menu;
extern uint8 exti_state[4];
void EXTI0_Config(void);
void EXTI1_Config(void);

void exti_initconfig (void);

extern uint8_t *List_Number_p;
#endif
