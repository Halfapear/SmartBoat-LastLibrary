#include "common.h"
#include "image.h"
#include "motor.h"

//宏定义
#define image_h 120//图像高度
#define image_w 188//图像宽度
#define border_max  image_w-2 //边界最大值
#define border_min  1   //边界最小值

typedef struct
{
    uint8 state;//0表示未识别到圆环，1开始为各个状态，每次切换状态加一即可
    uint8 Inflection_point_x,Inflection_point_y;//记录拐点的坐标
    uint16 Left_Lost_Time,Right_Lost_Time;//左右边界的丢线数量
    uint8 Rd_Flag_1,Rd_Flag_2,Rd_Flag_3,Rd_Flag_4;//备用
    uint8 Ring_Flag;//区分左右环的标志位，1位左，2为右，0为非环。
    uint16 Ring_time;//用于记录在圆环内个状态的时间，辅助判断
    uint16 count;//辅助计数
    uint8 Ring_Start;
    uint8 Ring_Start_L;//圆环检测左
    uint8 Ring_Start_R;//圆环检测右
    uint16 No_Ring_Flag;//圆环误判计数
    uint8 L_Edgepoint_x,L_Edgepoint_y,R_Edgepoint_x,R_Edgepoint_y;
    uint16 Ring_Leave_time;
}Round;

extern Round rd;
extern uint8_t Pixle[35][100]; // 假设这是你的像素数组的大小
extern uint8_t garageout_flag; // 用于指示是否检测到斑马线的标志

extern uint8_t region ;
extern uint8_t garage_count ;
extern uint8_t white_black ;
extern void Angle_calculate();
extern float Err_Sum(void);
void Round_init();
void GetLostTime();
void Get_Edge_Point();

void Ring_Search();
void Right_Ring();
void Left_Ring();
int Ring_Start_Test();//圆环开始的标志判断
//以下为标志位判断函数
int Find_Right_Down_Point(int start,int end);
int Find_Right_Up_Point(int start,int end);
int Find_Left_Up_Point(int start,int end);
int Find_Left_Down_Point(int start,int end);//找四个角点，返回值是角点所在的行数
int Continuity_Change_Right(int start,int end);
int Continuity_Change_Left(int start,int end);//连续性检验
int Monotonicity_Change_Right(int start,int end);
int Monotonicity_Change_Left(int start,int end);//单调性改变，返回值是单调性改变点所在的行数
//以下为补线函数
void Left_Add_Line(int x1,int y1,int x2,int y2);
void Right_Add_Line(int x1,int y1,int x2,int y2);//补边线
void Lengthen_Left_Boundry(int start,int end);
void Lengthen_Right_Boundry(int start,int end);//边界延长
void K_Add_Boundry_Left(float k,int startX,int startY,int endY);
void K_Add_Boundry_Right(float k,int startX,int startY,int endY);//斜率补边线
void K_Draw_Line(float k, int startX, int startY,int endY);//图像内划线
void Draw_Line(int startX, int startY, int endX, int endY);//输入起始点，终点坐标，补一条宽度为2的黑线

// 函数声明
void crosswalk(void);
