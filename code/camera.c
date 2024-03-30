#include "camera.h"


Round rd;
#define Search_Stop_Line 40
//十字
uint8 Cross_Flag=0;
volatile int Left_Down_Find=0; //十字使用，找到被置行数，没找到就是0
volatile int Left_Up_Find=0;   //四个拐点标志
volatile int Right_Down_Find=0;
volatile int Right_Up_Find=0;

int Left_Lost_Flag[MT9V03X_H] ; //左丢线数组，丢线置1，没丢线置0

int Right_Lost_Flag[MT9V03X_H] ; //左丢线数组，丢线置1，没丢线置0
int16 monotonicity_change_line[2];
int16 Right_Up_Guai[2];
int16 Left_Up_Guai[2];
int16 island_state_5_down[2];
int16 island_state_3_up[2];
//加权控制
const uint8 Weight[MT9V03X_H]=
{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端00 ——09 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //图像最远端10 ——19 行权重
        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,          //图像最远端20 ——29 行权重
        6, 7, 9,11,13,15,17,19,20,20,              //图像最远端30 ——39 行权重
        19,17,15,13,11, 9, 7, 5, 3, 1,          //图像最远端40 ——49 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               //图像最远端50 ——59 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               //图像最远端60 ——69 行权重
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                //70-79


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
    rd.Ring_Flag=0;
    rd.state=0;
    rd.Ring_time=0;
    rd.count=0;
    rd.Ring_Start=0;
    rd.Ring_Start_L=0;
    rd.Ring_Start_R=0;
    rd.No_Ring_Flag=0;
    rd.L_Edgepoint_x=0;
    rd.L_Edgepoint_y=0;
    rd.R_Edgepoint_x=0;
    rd.R_Edgepoint_y=0;
    rd.Ring_Leave_time=0;
    rd.Both_Lost_Time=0;
    rd.add_k=0;
    rd.right_up_guai=0;


    monotonicity_change_line[0]=0;
    monotonicity_change_line[1]=0;
    Right_Up_Guai[0]=0;
    Right_Up_Guai[1]=0;
    Left_Up_Guai[0]=0;
    Left_Up_Guai[1]=0;
    island_state_5_down[0]=0;
    island_state_5_down[1]=0;
    island_state_3_up[0]=0;
    island_state_3_up[1]=0;
    for(int16 i=0;i<MT9V03X_H;i++)
    {
        Left_Lost_Flag[i]=0;
        Right_Lost_Flag[i]=0;
    }

}

//获取左右边界的丢线次数
void GetLostTime()
{
    uint16 i;
    uint16 l_lost=0;
    uint16 r_lost=0;
    uint16 both_lost=0;
    uint16 both_lost_time=0;
    for(i=MT9V03X_H-2;i>10;i--)//从下往上
    {
        both_lost=0;
        if(l_border[i]<=rd_border_min)
        {
            Left_Lost_Flag[i]=1;
            l_lost++;
            both_lost++;
        }
        else if(r_border[i]>=rd_border_max)
        {
            Right_Lost_Flag[i]=1;
            r_lost++;
            both_lost++;
        }
        if(both_lost==2)
        {
            both_lost_time++;
        }
    }
    rd.Left_Lost_Time=l_lost;
    rd.Right_Lost_Time=r_lost;
    rd.Both_Lost_Time=both_lost_time;
}

//圆环状态判断，即1状态判断
//图像太抽象了，再确认下图像，根据图像改一下条件
int Ring_Start_Test()
{
    if(rd.Left_Lost_Time>rd.Right_Lost_Time&&
        abs(rd.Left_Lost_Time-rd.Right_Lost_Time)>20&&
        (rd.Left_Lost_Time+rd.Right_Lost_Time)<190&&
        Continuity_Change_Right(119,41)==0&&Continuity_Change_Left(119,41)!=0&&
        Monotonicity_Change_Left(100,20)!=0&&Monotonicity_Change_Right(100,20)==0&&
        Find_Left_Down_Point(115,50)
        )
    {
        return 1;
    }
    else if(rd.Left_Lost_Time<rd.Right_Lost_Time&&
            abs(rd.Left_Lost_Time-rd.Right_Lost_Time)>25&&
            (rd.Left_Lost_Time+rd.Right_Lost_Time)<190&&
            Continuity_Change_Right(119,41)!=0&&Continuity_Change_Left(119,41)==0&&
            Monotonicity_Change_Left(100,20)==0&&Monotonicity_Change_Right(100,20)!=0&&
            Find_Right_Down_Point(115,50)
            )
        {
            return 2;
        }
    else
        return 0;

}
//边界起始点的获取
void Get_Edge_Point()
{
    uint16 i;
    uint8 l_flag=0;
    uint8 r_flag=0;
    for(i=MT9V03X_H-1;i>10;i--)//从下往上
    {
        if (l_border[i]>3&&l_flag==0) {
           l_flag=1;
           rd.L_Edgepoint_x=l_border[i];
           rd.L_Edgepoint_y=i;
        }
        if (r_border[i]<184&&r_flag==0) {
                   r_flag=1;
                   rd.R_Edgepoint_x=r_border[i];
                   rd.R_Edgepoint_y=i;
                }
        if(l_flag==1&&r_flag==1)
            return;

    }
}
//圆环搜索-理论是连续n幅图像符合圆环就进圆环状态机，n根据实际情况改
void Ring_Search()
{
    rd.Ring_Start=Ring_Start_Test();
    if(rd.Ring_Start==1&&rd.Ring_Start_R==0)
    {
        rd.Ring_Start_L++;
    }
    else if(rd.Ring_Start==2&&rd.Ring_Start_L==0)
    {
        rd.Ring_Start_R++;
    }
    else
    {
        if(rd.Ring_Start_L!=0)//当标志未连续识别到时，减少标志位，防止误判
            rd.Ring_Start_L--;
        if(rd.Ring_Start_R!=0)
            rd.Ring_Start_R--;
     }

     if(rd.Ring_Start_L>=1)//连续三幅图左环标志，进左环处理，阈值可调
           rd.Ring_Flag=1;
     else if(rd.Ring_Start_R>=1)//连续三幅图右环标志，进右环处理
           rd.Ring_Flag=2;
}
//左环状态机
void Left_Ring()
{
    //rd.state=1;
    float k=0;
    if(rd.state==1)
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(30,5);//寻找单调性改变点
                monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
                Left_Add_Line((int)(monotonicity_change_line[1]*0.15),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
                if((rd.state==1)&&(rd.L_Edgepoint_y<=50))//下方当丢线时候进2
                {
                    rd.state=2;
                }
            }
    else if(rd.state==2)//下方角点消失，2状态时下方应该是丢线，上面是弧线
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(70,10);//寻找单调性改变点
                monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
                Left_Add_Line((int)(monotonicity_change_line[1]*0.1),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
                if(rd.state==2&&(rd.L_Edgepoint_y>=MT9V03X_H-5||monotonicity_change_line[0]>50))//当圆弧靠下时候，进3
                {
                    rd.state=3;//最长白列寻找范围也要改，见camera.c
                    //Left_Island_Flag=0;
                }
            }
    else if(rd.state==3)//3状态准备进环，寻找上拐点，连线
            {
                if(k!=0)
                {
                    K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);//k是刚刚算出来的，静态变量存着
                    image_process();//刷新边界数据
                }
                else
                {
                    Left_Up_Guai[0]=Find_Left_Up_Point(40,5);//找左上拐点
                    Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                    /*if (Left_Up_Guai[0]<5)//这里改过啊!!!!
                    {
                        rd.state=0;
                        rd.Ring_Flag=0;
                    }*/
                    if(k==0&&(15<=Left_Up_Guai[0]&&Left_Up_Guai[0]<50)&&(50<Left_Up_Guai[1]&&Left_Up_Guai[1]<110)&&Left_Up_Guai[0]>5)//拐点出现在一定范围内，认为是拐点出现
                    {
                        island_state_3_up[0]= Left_Up_Guai[0];
                        island_state_3_up[1]= Left_Up_Guai[1];
                        k=(float)((float)(MT9V03X_H-island_state_3_up[0])/(float)(MT9V03X_W-20-island_state_3_up[1]));
                        rd.add_k=k;
                        K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);//记录下第一次上点出现时位置，针对这个环岛拉一条死线，入环
                        image_process();//刷新边界数据
                    }
                }
                if((rd.state==3)&&(abs(FJ_Angle)>=60))//暂时使用编码器计数来转换状态，后续考虑换陀螺仪
                {
                          k=0;//斜率清零
                          encoder_derdate=0;
                          rd.state=4;
                          image_process();//重新扫线
                }
            }
    else if(rd.state==4)//状态4已经在里面
     {
        if(abs(FJ_Angle)>200)//积分200度以后在打开出环判断
        {
                    monotonicity_change_line[0]=Monotonicity_Change_Right(MT9V03X_H-10,10);//单调性改变
                    monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
                    if((rd.state==4)&&(35<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=55&&monotonicity_change_line[1]>=10))//单调点靠下，进去5
                    {//monotonicity_change_line[1]>=90&&
                        island_state_5_down[0]=MT9V03X_H-1;
                        island_state_5_down[1]=r_border[MT9V03X_H-1];
                        k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                        K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);//和状态3一样，记住斜率
                        rd.state=5;
                    }
         }
     }
     else if(rd.state==5)//出环
            {
             if(k!=0){
                K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
             }
                if((rd.state==5)&&(rd.R_Edgepoint_y<rd.L_Edgepoint_y))//右边先丢线
                {
                    rd.state=6;
                }
                if(abs(FJ_Angle)>300)
                {
                    rd.state=6;
                }
            }
            else if(rd.state==6)//还在出
            {
                if(k!=0){
                K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
                }
                if(((rd.state==6)&&(rd.R_Edgepoint_y>MT9V03X_H-10))||(abs(FJ_Angle)>330))//右边不丢线-陀螺仪待补充
                {
                    k=0;
                    rd.state=7;
                }
            }
            else if(rd.state==7)//基本出去了，在寻找拐点，准备离开环岛状态
                    {
                        Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-10,0);//获取左上点坐标，坐标点合理去8
                        Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                        if((rd.state==7)&&(Left_Up_Guai[1]<=100)&&(5<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=MT9V03X_H-20))//注意这里，对横纵坐标都有要求
                        {
                            rd.state=8;//基本上找到拐点就去8
                        }
                    }
                    else if(rd.state==8)//连线，出环最后一步
                    {
                        Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-1,10);//获取左上点坐标
                        Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                        Lengthen_Left_Boundry(Left_Up_Guai[0]-1,MT9V03X_H-1);
                        if((rd.state==8)&&(Left_Up_Guai[0]>=MT9V03X_H-20||(Left_Up_Guai[0]<10&&rd.L_Edgepoint_y>=MT9V03X_H-10)))//当拐点靠下时候，认为出环了，环岛结束
                        {//要么拐点靠下，要么拐点丢了，切下方不丢线，认为环岛结束了
                            //FJ_Angle=0;//数据清零
                            rd.state=9;//8时候环岛基本结束了，为了防止连续判环，8后会进9，大概几十毫秒后归零，
                            //rd.Ring_Flag=0;
                        }
                    }
                    else if(rd.state==9)
                        {
                            if(rd.Ring_Leave_time>=1000)
                            {
                                rd.state=0;
                                rd.Ring_Flag=0;
                                k=0;
                                rd.Ring_Leave_time=0;
                            }
                        }

}
//右环状态机
void Right_Ring()
{
    //rd.state=1;
    float k=0;
    if(rd.state==1)
    {
        monotonicity_change_line[0]=Monotonicity_Change_Right(30,5);//单调性改变
        monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
        Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
        if(rd.R_Edgepoint_y<=50)//右下角先丢线
        {
              rd.state=2;
        }
    }
    else if(rd.state==2)//2状态下方丢线，上方即将出现大弧线
    {
        monotonicity_change_line[0]=Monotonicity_Change_Right(70,5);//单调性改变
        monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
        //Draw_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*k)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
        //image_process();
        Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*k)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
//            if(rd.state==2&&(Boundry_Start_Right>=MT9V03X_H-10))//右下角再不丢线进3
        if(rd.state==2&&(rd.R_Edgepoint_y>=MT9V03X_H-5||monotonicity_change_line[0]>50))//右下角再不丢线进3
        {
            rd.state=3;//下方丢线，说明大弧线已经下来了
            k=0;
        }
    }
    else if(rd.state==3)//下面已经出现大弧线，且上方出现角点
         {
             if(k!=0)//已经找到点了，画一条死线
             {
                 K_Draw_Line(k,30,MT9V03X_H-1,0);
                 image_process();//重新扫线
             }
             else//还在找点
             {
                 Right_Up_Guai[0]=Find_Right_Up_Point(40,5);//找右上拐点
                 Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
                 /*if(Right_Up_Guai[0]<10)//角点位置不对，退出环岛
                 {
                     rd.state=0;
                     rd.Ring_Flag=0;
                 }*/
                 if(k==0&&(15<=Right_Up_Guai[0]&&Right_Up_Guai[0]<50)&&(70<Right_Up_Guai[1]&&Right_Up_Guai[1]<150)&&Right_Up_Guai[0]>5)
                 {//当角点出现在给定范围内
                     //island_state_3_up[0]= Right_Up_Guai[0];
                     //island_state_3_up[1]= Right_Up_Guai[1];
                     k=(float)((float)(MT9V03X_H-Right_Up_Guai[0])/(float)(20-Right_Up_Guai[1]));
                     K_Draw_Line(k,30,MT9V03X_H-1,0);
                     image_process();//刷新赛道数据
                 }
             }//3状态秒跳4，编码器得长点，或者上陀螺仪，后面也跳的很快
             if((rd.state==3)&&(abs(FJ_Angle)>=60))//暂时使用编码器计数来转换状态，后续考虑换陀螺仪
             {
                 k=0;//斜率清零
                 encoder_derdate=0;
                 rd.state=4;
                 image_process();//重新扫线
             }
         }
    else if(rd.state==4)//环内状态，无特殊处理
    {
        if(abs(FJ_Angle)>200)//积分200度以后在打开出环判断
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(110,30);//单调性改变
            monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
            if(rd.state==4&&Continuity_Change_Left(110,40)!=0)//有陀螺仪后添加陀螺仪判据，积分满220以上
            {
                island_state_5_down[0]=MT9V03X_H-1;
                island_state_5_down[1]=l_border[MT9V03X_H-1]-5;
                if(k==0)
                    k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);  //总感觉这个补线会出问题，有空再看看
                rd.state=5;
            }
        }
    }
    else if(rd.state==5)//准备出环岛
    {
        if(k!=0){
         K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
        }
         if(rd.state==5&&(rd.R_Edgepoint_y<rd.L_Edgepoint_y))//左边先丢线
         {
               rd.state=6;
         }
         if(abs(FJ_Angle)>270)
         {
             rd.state=6;
         }
    }
    else if(rd.state==6)//继续出
         {
        if(k!=0){
             K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
        }
             if((rd.state==6)&&(rd.L_Edgepoint_y>MT9V03X_H-10))//左边先丢线-此处可以添加陀螺仪判据积分300度
             {//
                 k=0;
                 rd.state=7;
             }
             if(abs(FJ_Angle)>300)
             {
                 k=0;
                 rd.state=7;
             }
         }
    else if(rd.state==7)//基本出环岛，找角点
          {
              Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-50,30);//获取左上点坐标，找到了去8
              rd.right_up_guai=Right_Up_Guai[0];
              Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
              if((rd.state==7)&&((Right_Up_Guai[1]>=MT9V03X_W-58&&(5<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=MT9V03X_H-20))))//注意这里，对横纵坐标都有要求，因为赛道不一样，会意外出现拐点
              {//当角点位置合理时，进8
                  rd.state=8;
              }
          }
    else if(rd.state==8)//环岛8
            {
                Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-1,10);//获取右上点坐标
                rd.right_up_guai=Right_Up_Guai[0];
                Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
                //Lengthen_Right_Boundry(Right_Up_Guai[0]-1,MT9V03X_H-1);
                if(k==0)
                {
                    k=(float)(5/(r_border[Right_Up_Guai[0]-5]-Right_Up_Guai[1]));
                }
                //K_Add_Boundry_Right(k,Right_Up_Guai[1],MT9V03X_H-1,Right_Up_Guai[0]);//补右边线
                K_Draw_Line(k, MT9V03X_W-5, MT9V03X_H-1, Right_Up_Guai[0]);
                image_process();
                if((rd.state==8)&&(Right_Up_Guai[0]>=MT9V03X_H-20||(Right_Up_Guai[0]<10&&rd.L_Edgepoint_y>=MT9V03X_H-10)))//当拐点靠下时候，认为出环了，环岛结束
                {//角点靠下，或者下端不丢线，认为出环了
                    //FJ_Angle=0;
                    rd.state=9;
                    rd.Ring_Flag=0;
                }
            }
    else if(rd.state==9)
    {
        if(rd.Ring_Leave_time>=3000)
        {
            rd.state=0;
            rd.Ring_Flag=0;
            k=0;
            rd.Ring_Leave_time=0;
        }
    }
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
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(right_down_line==0&&//只找第一个符合条件的点
           abs(r_border[i]-r_border[i+1])<=5&&//角点的阈值可以更改
           abs(r_border[i+1]-r_border[i+2])<=5&&
              (r_border[i]-r_border[i-1])<=-5&&
              (r_border[i]-r_border[i-2])<=-10&&
              (r_border[i]-r_border[i-3])<=-10)
        {
            right_down_line=i;//获取行数即可
            break;
        }
    }
    return right_down_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     右上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int right_up_line=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return right_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }

    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(right_up_line==0&&//只找第一个符合条件的点
           abs(r_border[i]-r_border[i-1])<=6&&//下面两行位置差不多
           abs(r_border[i-1]-r_border[i-2])<=6&&
              (r_border[i]-r_border[i+1])<=-6&&
              (r_border[i]-r_border[i+2])<=-8
              )
        {
            right_up_line=i;//获取行数即可
            break;
        }
    }
    return right_up_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     左上角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Up_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Up_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_up_line=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
       return left_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(left_up_line==0&&//只找第一个符合条件的点
           abs(l_border[i]-l_border[i-1])<=5&&
           abs(l_border[i-1]-l_border[i-2])<=5&&
           abs(l_border[i-2]-l_border[i-3])<=5&&
              (l_border[i]-l_border[i+2])>=8&&
              (l_border[i]-l_border[i+3])>=15&&
              (l_border[i]-l_border[i+4])>=15)
        {
            left_up_line=i;//获取行数即可
            break;
        }
    }
    return left_up_line;//如果是MT9V03X_H-1，说明没有这么个拐点
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     左下角点检测
  @param     起始点，终止点
  @return    返回角点所在的行数，找不到返回0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      角点检测阈值可根据实际值更改
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start,int end)//找四个角点，返回值是角点所在的行数
{
    int i,t;
    int left_down_line=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有拐点判断的意义
        return left_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(left_down_line==0&&//只找第一个符合条件的点
           abs(l_border[i]-l_border[i+1])<=6&&//角点的阈值可以更改
           abs(l_border[i+1]-l_border[i+2])<=6&&
              (l_border[i]-l_border[i-2])>=5&&
              (l_border[i]-l_border[i-3])>=10&&
              (l_border[i]-l_border[i-4])>=10)
        {
            left_down_line=i;//获取行数即可
            break;
        }
    }
    return left_down_line;
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
  @brief     左赛道连续性检测
  @param     起始点，终止点
  @return    连续返回0，不连续返回断线出行数
  Sample     continuity_change_flag=Continuity_Change_Left(int start,int end)
  @note      连续性的阈值设置为5，可更改
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没必要判断了
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
        if(abs(l_border[i]-l_border[i-1])>=5)//连续性阈值是5，可更改
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     单调性突变检测-右边
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
  @brief     单调性突变检测-左边
  @param     起始点，终止行
  @return    点所在的行数，找不到返回0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      前5后5它最大（最小），那他就是角点
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Left(int start,int end)//单调性改变，返回值是单调性改变点所在的行数
{
    int i;
    int monotonicity_change_line=0;

    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//大部分都丢线，没有单调性判断的意义
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//数组越界保护
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//会读取前5后5数据，所以前面对输入范围有要求
    {
        if(l_border[i]==l_border[i+5]&&l_border[i]==l_border[i-5]&&
        l_border[i]==l_border[i+4]&&l_border[i]==l_border[i-4]&&
        l_border[i]==l_border[i+3]&&l_border[i]==l_border[i-3]&&
        l_border[i]==l_border[i+2]&&l_border[i]==l_border[i-2]&&
        l_border[i]==l_border[i+1]&&l_border[i]==l_border[i-1])
        {//一堆数据一样，显然不能作为单调转折点
            continue;
        }
        else if(l_border[i] >l_border[i+5]&&l_border[i] >l_border[i-5]&&
        l_border[i] >l_border[i+4]&&l_border[i] >l_border[i-4]&&
        l_border[i]>=l_border[i+3]&&l_border[i]>=l_border[i-3]&&
        l_border[i]>=l_border[i+2]&&l_border[i]>=l_border[i-2]&&
        l_border[i]>=l_border[i+1]&&l_border[i]>=l_border[i-1])
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
  @brief     通过斜率，定点补线
  @param     k       输入斜率
             startY  输入起始点纵坐标
             endY    结束点纵坐标
  @return    null    直接补边线
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
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
    if(startY<endY)
    {
        t=startY;
        startY=endY;
        endY=t;
    }
    for(i=startY;i>=endY;i--)
    {
        r_border[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
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
/*-------------------------------------------------------------------------------------------------------------------
  @brief     左补线
  @param     补线的起点，终点
  @return    null
  Sample     Left_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的,不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Left_Add_Line(int x1,int y1,int x2,int y2)//左补线,补的是边界
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//起始点位置校正，排除数组越界的可能
       x1=MT9V03X_W-1;
    else if(x1<=0)
        x1=0;
     if(y1>=MT9V03X_H-1)
        y1=MT9V03X_H-1;
     else if(y1<=0)
        y1=0;
     if(x2>=MT9V03X_W-1)
        x2=MT9V03X_W-1;
     else if(x2<=0)
             x2=0;
     if(y2>=MT9V03X_H-1)
        y2=MT9V03X_H-1;
     else if(y2<=0)
             y2=0;
    a1=y1;
    a2=y2;
    if(a1>a2)//坐标互换
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//根据斜率补线即可
    {
        hx=(i-y1)*(x2-x1)/(y2-y1)+x1;
        if(hx>=MT9V03X_W)
            hx=MT9V03X_W;
        else if(hx<=0)
            hx=0;
        l_border[i]=hx;
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     右补线
  @param     补线的起点，终点
  @return    null
  Sample     Right_Add_Line(int x1,int y1,int x2,int y2);
  @note      补的直接是边界，点最好是可信度高的，不要乱补
-------------------------------------------------------------------------------------------------------------------*/
void Right_Add_Line(int x1,int y1,int x2,int y2)//右补线,补的是边界
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//起始点位置校正，排除数组越界的可能
       x1=MT9V03X_W-1;
    else if(x1<=0)
        x1=0;
    if(y1>=MT9V03X_H-1)
        y1=MT9V03X_H-1;
    else if(y1<=0)
        y1=0;
    if(x2>=MT9V03X_W-1)
        x2=MT9V03X_W-1;
    else if(x2<=0)
        x2=0;
    if(y2>=MT9V03X_H-1)
        y2=MT9V03X_H-1;
    else if(y2<=0)
         y2=0;
    a1=y1;
    a2=y2;
    if(a1>a2)//坐标互换
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//根据斜率补线即可
    {
        hx=(i-y1)*(x2-x1)/(y2-y1)+x1;
        if(hx>=MT9V03X_W)
            hx=MT9V03X_W;
        else if(hx<=0)
            hx=0;
        r_border[i]=hx;
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找5个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Left_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
         Left_Add_Line(l_border[start],start,l_border[end],end);
    }

    else
    {
        k=(float)(l_border[start]-l_border[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            l_border[i]=(int)(i-start)*k+l_border[start];//(x=(y-y1)*k+x1),点斜式变形
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
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     右左边界延长
  @param     延长起始行数，延长到某行
  @return    null
  Sample     Stop_Detect(void)
  @note      从起始点向上找3个点，算出斜率，向下延长，直至结束点
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Right_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//起始点位置校正，排除数组越界的可能
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++访问，坐标互换
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//因为需要在开始点向上找3个点，对于起始点过于靠上，不能做延长，只能直接连线
    {
        Right_Add_Line(r_border[start],start,r_border[end],end);
    }
    else
    {
        k=(float)(r_border[start]-r_border[start-4])/5.0;//这里的k是1/斜率
        for(i=start;i<=end;i++)
        {
            r_border[i]=(int)(i-start)*k+r_border[start];//(x=(y-y1)*k+x1),点斜式变形
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
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     根据斜率划线
  @param     输入斜率，定点，画一条黑线
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY,int endY)
{
    int endX=0;

    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    endX=(int)((endY-startY)/k+startX);//(y-y1)=k(x-x1)变形，x=(y-y1)/k+x1
    Draw_Line(startX,startY,endX,endY);
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     画线
  @param     输入起始点，终点坐标，补一条宽度为2的黑线
  @return    null
  Sample     Draw_Line(0, 0,MT9V03X_W-1,MT9V03X_H-1);
             Draw_Line(MT9V03X_W-1, 0,0,MT9V03X_H-1);
                                    画一个大×
  @note     补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i,x,y;
    int start=0,end=0;
    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endX>=MT9V03X_W-1)
        endX=MT9V03X_W-1;
    else if(endX<=0)
        endX=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startX==endX)//一条竖线
    {
        if (startY > endY)//互换
        {
            start=endY;
            end=startY;
        }
        for (i = start; i <= end; i++)
        {
            if(i<=1)
                i=1;
            bin_image[i][startX]=black_pixel;
            bin_image[i-1][startX]=black_pixel;
        }
    }
    else if(startY == endY)//补一条横线
    {
        if (startX > endX)//互换
        {
            start=endX;
            end=startX;
        }
        for (i = start; i <= end; i++)
        {
            if(startY<=1)
                startY=1;
            bin_image[startY][i]=black_pixel;
            bin_image[startY-1][i]=black_pixel;
        }
    }
    else //上面两个是水平，竖直特殊情况，下面是常见情况
    {
        if(startY>endY)//起始点矫正
        {
            start=endY;
            end=startY;
        }
        else
        {
            start=startY;
            end=endY;
        }
        for (i = start; i <= end; i++)//纵向补线，保证每一行都有黑点
        {
            x =(int)(startX+(endX-startX)*(i-startY)/(endY-startY));//两点式变形
            if(x>=MT9V03X_W-1)
                x=MT9V03X_W-1;
            else if (x<=1)
                x=1;
            bin_image[i][x] = black_pixel;
            bin_image[i][x-1] = black_pixel;
        }
        if(startX>endX)
        {
            start=endX;
            end=startX;
        }
        else
        {
            start=startX;
            end=endX;
        }
        for (i = start; i <= end; i++)//横向补线，保证每一列都有黑点
        {

            y =(int)(startY+(endY-startY)*(i-startX)/(endX-startX));//两点式变形
            if(y>=MT9V03X_H-1)
                y=MT9V03X_H-1;
            else if (y<=0)
                y=0;
            bin_image[y][i] = black_pixel;
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     找下面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Right_Down_Find=0;
             Left_Down_Find=0;
  Sample     Find_Down_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Down_Point(int start,int end)
{
    int i,t;
    Right_Down_Find=0;
    Left_Down_Find=0;
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
        if(Left_Down_Find==0&&//只找第一个符合条件的点
           abs(l_border[i]-l_border[i+1])<=5&&//角点的阈值可以更改
           abs(l_border[i+1]-l_border[i+2])<=5&&
           abs(l_border[i+2]-l_border[i+3])<=5&&
              (l_border[i]-l_border[i-2])>=8&&
              (l_border[i]-l_border[i-3])>=15&&
              (l_border[i]-l_border[i-4])>=15)
        {
            Left_Down_Find=i;//获取行数即可
        }
        if(Right_Down_Find==0&&//只找第一个符合条件的点
           abs(r_border[i]-r_border[i+1])<=5&&//角点的阈值可以更改
           abs(r_border[i+1]-r_border[i+2])<=5&&
           abs(r_border[i+2]-r_border[i+3])<=5&&
              (r_border[i]-r_border[i-2])<=-8&&
              (r_border[i]-r_border[i-3])<=-15&&
              (r_border[i]-r_border[i-4])<=-15)
        {
            Right_Down_Find=i;
        }
        if(Left_Down_Find!=0&&Right_Down_Find!=0)//两个找到就退出
        {
            break;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     找上面的两个拐点，供十字使用
  @param     搜索的范围起点，终点
  @return    修改两个全局变量
             Left_Up_Find=0;
             Right_Up_Find=0;
  Sample     Find_Up_Point(int start,int end)
  @note      运行完之后查看对应的变量，注意，没找到时对应变量将是0
-------------------------------------------------------------------------------------------------------------------*/
void Find_Up_Point(int start,int end)
{
    int i,t;
    Left_Up_Find=0;
    Right_Up_Find=0;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)//及时最长白列非常长，也要舍弃部分点，防止数组越界
        end=5;
    if(start>=MT9V03X_H-1-5)//下面5行数据不稳定，不能作为边界点来判断，舍弃
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(Left_Up_Find==0&&//只找第一个符合条件的点
           abs(l_border[i]-l_border[i-1])<=5&&
           abs(l_border[i-1]-l_border[i-2])<=5&&
           abs(l_border[i-2]-l_border[i-3])<=5&&
              (l_border[i]-l_border[i+2])>=8&&
              (l_border[i]-l_border[i+3])>=15&&
              (l_border[i]-l_border[i+4])>=15)
        {
            Left_Up_Find=i;//获取行数即可
        }
        if(Right_Up_Find==0&&//只找第一个符合条件的点
           abs(r_border[i]-r_border[i-1])<=5&&//下面两行位置差不多
           abs(r_border[i-1]-r_border[i-2])<=5&&
           abs(r_border[i-2]-r_border[i-3])<=5&&
              (r_border[i]-r_border[i+2])<=-8&&
              (r_border[i]-r_border[i+3])<=-15&&
              (r_border[i]-r_border[i+4])<=-15)
        {
            Right_Up_Find=i;//获取行数即可
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//下面两个找到就出去
        {
            break;
        }
    }
    if(abs(Right_Up_Find-Left_Up_Find)>=30)//纵向撕裂过大，视为误判
    {
        Right_Up_Find=0;
        Left_Up_Find=0;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     十字检测
  @param     null
  @return    null
  Sample     Cross_Detect(void);
  @note      利用四个拐点判别函数，查找四个角点，根据找到拐点的个数决定是否补线
-------------------------------------------------------------------------------------------------------------------*/
void Cross_Detect()
{
    int down_search_start=0;//下点搜索开始行
    Cross_Flag=0;
    if(rd.state==0&&rd.Ring_Flag==0)//与环岛互斥开
    {
        Left_Up_Find=0;
        Right_Up_Find=0;
        if(rd.Both_Lost_Time>=10)//十字必定有双边丢线，在有双边丢线的情况下再开始找角点
        {
            Find_Up_Point( MT9V03X_H-1, 0 );
            if(Left_Up_Find==0&&Right_Up_Find==0)//只要没有同时找到两个上点，直接结束
            {
                return;
            }
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//找到两个上点，就找到十字了
        {
            Cross_Flag=1;//对应标志位，便于各元素互斥掉
            down_search_start=Left_Up_Find>Right_Up_Find?Left_Up_Find:Right_Up_Find;//用两个上拐点坐标靠下者作为下点的搜索上限
            Find_Down_Point(MT9V03X_H-5,down_search_start+2);//在上拐点下2行作为下点的截止行
            if(Left_Down_Find<=Left_Up_Find)
            {
                Left_Down_Find=0;//下点不可能比上点还靠上
            }
            if(Right_Down_Find<=Right_Up_Find)
            {
                Right_Down_Find=0;//下点不可能比上点还靠上
            }
            if(Left_Down_Find!=0&&Right_Down_Find!=0)
            {//四个点都在，无脑连线，这种情况显然很少
                Left_Add_Line (l_border [Left_Up_Find ],Left_Up_Find ,l_border [Left_Down_Find ] ,Left_Down_Find);
                Right_Add_Line(r_border[Right_Up_Find],Right_Up_Find,r_border[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find==0&&Right_Down_Find!=0)//11//这里使用的都是斜率补线
            {//三个点                                     //01
                Lengthen_Left_Boundry(Left_Up_Find-1,MT9V03X_H-1);
                Right_Add_Line(r_border[Right_Up_Find],Right_Up_Find,r_border[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find!=0&&Right_Down_Find==0)//11
            {//三个点                                     //10
                Left_Add_Line (l_border [Left_Up_Find ],Left_Up_Find ,l_border [Left_Down_Find ] ,Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
            else if(Left_Down_Find==0&&Right_Down_Find==0)//11
            {//就俩上点                                   //00
                Lengthen_Left_Boundry (Left_Up_Find-1,MT9V03X_H-1);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
        }
        else
        {
            Cross_Flag=0;
        }
    }
    //角点相关变量，debug使用
    //ips200_showuint8(0,12,Cross_Flag);
//    ips200_showuint8(0,13,Island_State);
//    ips200_showuint8(50,12,Left_Up_Find);
//    ips200_showuint8(100,12,Right_Up_Find);
//    ips200_showuint8(50,13,Left_Down_Find);
//    ips200_showuint8(100,13,Right_Down_Find);
}
