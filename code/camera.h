#include "common.h"
#include "image.h"
#include "motor.h"

typedef struct
{
    uint8 start_round_flag;
    uint8 Inflection_point_x,Inflection_point_y;
    uint16 Left_Lost_Time,Right_Lost_Time;//左右边界的丢线数量
    uint8 Rd_Flag_1,Rd_Flag_2,Rd_Flag_3,Rd_Flag_4;
}Round;

extern Round rd;
extern uint8_t Pixle[35][100]; // 假设这是你的像素数组的大小
extern uint8_t garageout_flag; // 用于指示是否检测到斑马线的标志

extern uint8_t region ;
extern uint8_t garage_count ;
extern uint8_t white_black ;
extern void Angle_calculate();
extern float Err_Sum(void);
// 函数声明
void crosswalk(void);
