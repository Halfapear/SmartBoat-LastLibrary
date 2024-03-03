#include "camera.h"

Round rd;
#define Search_Stop_Line 50
//加权控制
const uint8 Weight[MT9V03X_H]=
{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端00 ——09 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端10 ——19 行权重
        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,              //图像最远端20 ——29 行权重
        6, 7, 9,11,13,15,17,19,20,20,              //图像最远端30 ——39 行权重
       19,17,15,13,11, 9, 7, 5, 3, 1,              //图像最远端40 ——49 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端50 ——59 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端60 ——69 行权重
};
float Err_Sum(void)
{
    int i;
    float err=0;
    float weight_count=0;
    //常规误差
    for(i=MT9V03X_H-1;i>=MT9V03X_H-Search_Stop_Line-1;i--)//常规误差计算
    {
        err+=(MT9V03X_W/2-((l_border[i]+r_border[i])>>1))*Weight[i];//右移1位，等效除2
        weight_count+=Weight[i];
    }
    err=err/weight_count;
    return err;
}
//环岛相关参数的初始化
void Round_init()
{
    rd.Inflection_point_x=1;
    rd.Inflection_point_y=1;
    rd.Right_Lost_Time=0;
    rd.Left_Lost_Time=0;

}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     右下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_down_line=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(right_down_line==0&&//只找第一个符合条件的点
           abs(r_border[i]-r_border[i+1])<=5&&//角点的阈值可以更改
           abs(r_border[i+1]-r_border[i+2])<=5&&
           abs(r_border[i+2]-r_border[i+3])<=5&&
              (r_border[i]-r_border[i-2])<=-5&&
              (r_border[i]-r_border[i-3])<=-10&&
              (r_border[i]-r_border[i-4])<=-10)
        {
            right_down_line=i;//获取行数即可
            break;
        }
    }
    return right_down_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     右赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     continuity_change_flag=Continuity_Change_Right(int start,int end)
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Right(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没必要判断了
       return 1;
    if(start>=MT9V03X_H-5)//数组越界保护
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//都是从下往上计算的，反了就互换一下
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(r_border[i]-r_border[i-1])>=5)//连续性阈值是5，可更改
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Right(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;

    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(r_border[i]==r_border[i+5]&&r_border[i]==r_border[i-5]&&
        r_border[i]==r_border[i+4]&&r_border[i]==r_border[i-4]&&
        r_border[i]==r_border[i+3]&&r_border[i]==r_border[i-3]&&
        r_border[i]==r_border[i+2]&&r_border[i]==r_border[i-2]&&
        r_border[i]==r_border[i+1]&&r_border[i]==r_border[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(r_border[i] <r_border[i+5]&&r_border[i] <r_border[i-5]&&
        r_border[i] <r_border[i+4]&&r_border[i] <r_border[i-4]&&
        r_border[i]<=r_border[i+3]&&r_border[i]<=r_border[i-3]&&
        r_border[i]<=r_border[i+2]&&r_border[i]<=r_border[i-2]&&
        r_border[i]<=r_border[i+1]&&r_border[i]<=r_border[i-1])
        {//就很暴力，这个数据是在前5，后5中最大的，那就是单调突变点
            monotonicity_change_line=i;
            break;
        }
    }
    return monotonicity_change_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线--补左线
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Left(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)//--操作，start需要大
    {
        t=startY;
        startY=endY;
        endY=t;
    }
//这里有bug，下方循环--循环，需要start要大，只进行y的互换，但是没有进行x的互换
//建议进行判断，如果start更小，那就进行++访问
//这里修改各位自行操作
    for(i=startY;i>=endY;i--)
    {
        l_border[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
        if(l_border[i]>=MT9V03X_W-1)
        {
            l_border[i]=MT9V03X_W-1;
        }
        else if(l_border[i]<=0)
        {
            l_border[i]=0;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     通过斜率，定点补线--补右线
  @param     k       输入斜率的相反数
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      补得线直接贴在边线上
-------------------------------------------------------------------------------------------------------------------*/
void K_Add_Boundry_Right(float k,int startX,int startY,int endY)
{
    int i,t;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startY<endY)//--操作，start需要大
    {
        t=startY;
        startY=endY;
        endY=t;
    }
//这里有bug，下方循环--循环，需要start要大，只进行y的互换，但是没有进行x的互换
//建议进行判断，如果start更小，那就进行++访问
//这里修改各位自行操作
    for(i=startY;i>=endY;i--)
    {
        r_border[i]=(int)((startY-i)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
        if(r_border[i]>=MT9V03X_W-1)
        {
            r_border[i]=MT9V03X_W-1;
        }
        else if(r_border[i]<=0)
        {
            r_border[i]=0;
        }
    }
}
// 假设这些变量在某处已经定义
uint8_t Pixle[35][100]; // 实际的二维像素数组定义
uint8_t garageout_flag = 0; // 初始化标志

uint8_t region = 0;
uint8_t garage_count = 0;
uint8_t white_black = 0;
uint8_t black_white = 0;
void crosswalk(void) {

    for (uint8_t hang = 20; hang < 35; hang++) {
        garage_count = 0;
        black_white = !white_black; // 重置，以便每次新行开始时触发变化检测
        for (uint8_t lie = 10; lie < 100; lie++) {
            white_black = (bin_image[hang][lie] == white_pixel) ? 1 : 0;

            if (white_black != black_white) {
                black_white = white_black;
                garage_count++;
            }
            if (garage_count > 11) {
                region++;
                break; // 一旦在一行中检测到足够的变化，即跳出循环
            }
        }
        if (region > 2) {
            garageout_flag = 1;
            break; // 在足够多的行中检测到变化后退出
        }
    }
}
