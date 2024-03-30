#include "camera.h"


Round rd;
#define Search_Stop_Line 40
//ʮ��
uint8 Cross_Flag=0;
volatile int Left_Down_Find=0; //ʮ��ʹ�ã��ҵ�����������û�ҵ�����0
volatile int Left_Up_Find=0;   //�ĸ��յ��־
volatile int Right_Down_Find=0;
volatile int Right_Up_Find=0;

int Left_Lost_Flag[MT9V03X_H] ; //�������飬������1��û������0

int Right_Lost_Flag[MT9V03X_H] ; //�������飬������1��û������0
int16 monotonicity_change_line[2];
int16 Right_Up_Guai[2];
int16 Left_Up_Guai[2];
int16 island_state_5_down[2];
int16 island_state_3_up[2];
//��Ȩ����
const uint8 Weight[MT9V03X_H]=
{
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��00 ����09 ��Ȩ��
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,              //ͼ����Զ��10 ����19 ��Ȩ��
        1, 1, 1, 1, 1, 1, 1, 3, 4, 5,          //ͼ����Զ��20 ����29 ��Ȩ��
        6, 7, 9,11,13,15,17,19,20,20,              //ͼ����Զ��30 ����39 ��Ȩ��
        19,17,15,13,11, 9, 7, 5, 3, 1,          //ͼ����Զ��40 ����49 ��Ȩ��
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               //ͼ����Զ��50 ����59 ��Ȩ��
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,               //ͼ����Զ��60 ����69 ��Ȩ��
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                //70-79


};
float Err_Sum(void)
{
    int i;
    float err=0;
    float weight_count=0;
    //�������
    for(i=MT9V03X_H-1;i>=MT9V03X_H-Search_Stop_Line-1;i--)//����������
    {
        err+=(MT9V03X_W/2-((l_border[i]+r_border[i])>>1))*Weight[i];//����1λ����Ч��2
        weight_count+=Weight[i];
    }
    err=err/weight_count;
    return err;
}
//������ز����ĳ�ʼ��
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

//��ȡ���ұ߽�Ķ��ߴ���
void GetLostTime()
{
    uint16 i;
    uint16 l_lost=0;
    uint16 r_lost=0;
    uint16 both_lost=0;
    uint16 both_lost_time=0;
    for(i=MT9V03X_H-2;i>10;i--)//��������
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

//Բ��״̬�жϣ���1״̬�ж�
//ͼ��̫�����ˣ���ȷ����ͼ�񣬸���ͼ���һ������
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
//�߽���ʼ��Ļ�ȡ
void Get_Edge_Point()
{
    uint16 i;
    uint8 l_flag=0;
    uint8 r_flag=0;
    for(i=MT9V03X_H-1;i>10;i--)//��������
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
//Բ������-����������n��ͼ�����Բ���ͽ�Բ��״̬����n����ʵ�������
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
        if(rd.Ring_Start_L!=0)//����־δ����ʶ��ʱ�����ٱ�־λ����ֹ����
            rd.Ring_Start_L--;
        if(rd.Ring_Start_R!=0)
            rd.Ring_Start_R--;
     }

     if(rd.Ring_Start_L>=1)//��������ͼ�󻷱�־�����󻷴�����ֵ�ɵ�
           rd.Ring_Flag=1;
     else if(rd.Ring_Start_R>=1)//��������ͼ�һ���־�����һ�����
           rd.Ring_Flag=2;
}
//��״̬��
void Left_Ring()
{
    //rd.state=1;
    float k=0;
    if(rd.state==1)
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(30,5);//Ѱ�ҵ����Ըı��
                monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
                Left_Add_Line((int)(monotonicity_change_line[1]*0.15),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
                if((rd.state==1)&&(rd.L_Edgepoint_y<=50))//�·�������ʱ���2
                {
                    rd.state=2;
                }
            }
    else if(rd.state==2)//�·��ǵ���ʧ��2״̬ʱ�·�Ӧ���Ƕ��ߣ������ǻ���
            {
                monotonicity_change_line[0]=Monotonicity_Change_Left(70,10);//Ѱ�ҵ����Ըı��
                monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
                Left_Add_Line((int)(monotonicity_change_line[1]*0.1),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
                if(rd.state==2&&(rd.L_Edgepoint_y>=MT9V03X_H-5||monotonicity_change_line[0]>50))//��Բ������ʱ�򣬽�3
                {
                    rd.state=3;//�����Ѱ�ҷ�ΧҲҪ�ģ���camera.c
                    //Left_Island_Flag=0;
                }
            }
    else if(rd.state==3)//3״̬׼��������Ѱ���Ϲյ㣬����
            {
                if(k!=0)
                {
                    K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);//k�Ǹո�������ģ���̬��������
                    image_process();//ˢ�±߽�����
                }
                else
                {
                    Left_Up_Guai[0]=Find_Left_Up_Point(40,5);//�����Ϲյ�
                    Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                    /*if (Left_Up_Guai[0]<5)//����Ĺ���!!!!
                    {
                        rd.state=0;
                        rd.Ring_Flag=0;
                    }*/
                    if(k==0&&(15<=Left_Up_Guai[0]&&Left_Up_Guai[0]<50)&&(50<Left_Up_Guai[1]&&Left_Up_Guai[1]<110)&&Left_Up_Guai[0]>5)//�յ������һ����Χ�ڣ���Ϊ�ǹյ����
                    {
                        island_state_3_up[0]= Left_Up_Guai[0];
                        island_state_3_up[1]= Left_Up_Guai[1];
                        k=(float)((float)(MT9V03X_H-island_state_3_up[0])/(float)(MT9V03X_W-20-island_state_3_up[1]));
                        rd.add_k=k;
                        K_Draw_Line(k,MT9V03X_W-30,MT9V03X_H-1,0);//��¼�µ�һ���ϵ����ʱλ�ã�������������һ�����ߣ��뻷
                        image_process();//ˢ�±߽�����
                    }
                }
                if((rd.state==3)&&(abs(FJ_Angle)>=60))//��ʱʹ�ñ�����������ת��״̬���������ǻ�������
                {
                          k=0;//б������
                          encoder_derdate=0;
                          rd.state=4;
                          image_process();//����ɨ��
                }
            }
    else if(rd.state==4)//״̬4�Ѿ�������
     {
        if(abs(FJ_Angle)>200)//����200���Ժ��ڴ򿪳����ж�
        {
                    monotonicity_change_line[0]=Monotonicity_Change_Right(MT9V03X_H-10,10);//�����Ըı�
                    monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
                    if((rd.state==4)&&(35<=monotonicity_change_line[0]&&monotonicity_change_line[0]<=55&&monotonicity_change_line[1]>=10))//�����㿿�£���ȥ5
                    {//monotonicity_change_line[1]>=90&&
                        island_state_5_down[0]=MT9V03X_H-1;
                        island_state_5_down[1]=r_border[MT9V03X_H-1];
                        k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                        K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);//��״̬3һ������סб��
                        rd.state=5;
                    }
         }
     }
     else if(rd.state==5)//����
            {
             if(k!=0){
                K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
             }
                if((rd.state==5)&&(rd.R_Edgepoint_y<rd.L_Edgepoint_y))//�ұ��ȶ���
                {
                    rd.state=6;
                }
                if(abs(FJ_Angle)>300)
                {
                    rd.state=6;
                }
            }
            else if(rd.state==6)//���ڳ�
            {
                if(k!=0){
                K_Add_Boundry_Right(k,island_state_5_down[1],island_state_5_down[0],0);
                }
                if(((rd.state==6)&&(rd.R_Edgepoint_y>MT9V03X_H-10))||(abs(FJ_Angle)>330))//�ұ߲�����-�����Ǵ�����
                {
                    k=0;
                    rd.state=7;
                }
            }
            else if(rd.state==7)//������ȥ�ˣ���Ѱ�ҹյ㣬׼���뿪����״̬
                    {
                        Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-10,0);//��ȡ���ϵ����꣬��������ȥ8
                        Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                        if((rd.state==7)&&(Left_Up_Guai[1]<=100)&&(5<=Left_Up_Guai[0]&&Left_Up_Guai[0]<=MT9V03X_H-20))//ע������Ժ������궼��Ҫ��
                        {
                            rd.state=8;//�������ҵ��յ��ȥ8
                        }
                    }
                    else if(rd.state==8)//���ߣ��������һ��
                    {
                        Left_Up_Guai[0]=Find_Left_Up_Point(MT9V03X_H-1,10);//��ȡ���ϵ�����
                        Left_Up_Guai[1]=l_border[Left_Up_Guai[0]];
                        Lengthen_Left_Boundry(Left_Up_Guai[0]-1,MT9V03X_H-1);
                        if((rd.state==8)&&(Left_Up_Guai[0]>=MT9V03X_H-20||(Left_Up_Guai[0]<10&&rd.L_Edgepoint_y>=MT9V03X_H-10)))//���յ㿿��ʱ����Ϊ�����ˣ���������
                        {//Ҫô�յ㿿�£�Ҫô�յ㶪�ˣ����·������ߣ���Ϊ����������
                            //FJ_Angle=0;//��������
                            rd.state=9;//8ʱ�򻷵����������ˣ�Ϊ�˷�ֹ�����л���8����9����ż�ʮ�������㣬
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
//�һ�״̬��
void Right_Ring()
{
    //rd.state=1;
    float k=0;
    if(rd.state==1)
    {
        monotonicity_change_line[0]=Monotonicity_Change_Right(30,5);//�����Ըı�
        monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
        Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*0.15)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
        if(rd.R_Edgepoint_y<=50)//���½��ȶ���
        {
              rd.state=2;
        }
    }
    else if(rd.state==2)//2״̬�·����ߣ��Ϸ��������ִ���
    {
        monotonicity_change_line[0]=Monotonicity_Change_Right(70,5);//�����Ըı�
        monotonicity_change_line[1]=r_border[monotonicity_change_line[0]];
        //Draw_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*k)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
        //image_process();
        Right_Add_Line((int)(MT9V03X_W-1-(monotonicity_change_line[1]*k)),MT9V03X_H-1,monotonicity_change_line[1],monotonicity_change_line[0]);
//            if(rd.state==2&&(Boundry_Start_Right>=MT9V03X_H-10))//���½��ٲ����߽�3
        if(rd.state==2&&(rd.R_Edgepoint_y>=MT9V03X_H-5||monotonicity_change_line[0]>50))//���½��ٲ����߽�3
        {
            rd.state=3;//�·����ߣ�˵�������Ѿ�������
            k=0;
        }
    }
    else if(rd.state==3)//�����Ѿ����ִ��ߣ����Ϸ����ֽǵ�
         {
             if(k!=0)//�Ѿ��ҵ����ˣ���һ������
             {
                 K_Draw_Line(k,30,MT9V03X_H-1,0);
                 image_process();//����ɨ��
             }
             else//�����ҵ�
             {
                 Right_Up_Guai[0]=Find_Right_Up_Point(40,5);//�����Ϲյ�
                 Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
                 /*if(Right_Up_Guai[0]<10)//�ǵ�λ�ò��ԣ��˳�����
                 {
                     rd.state=0;
                     rd.Ring_Flag=0;
                 }*/
                 if(k==0&&(15<=Right_Up_Guai[0]&&Right_Up_Guai[0]<50)&&(70<Right_Up_Guai[1]&&Right_Up_Guai[1]<150)&&Right_Up_Guai[0]>5)
                 {//���ǵ�����ڸ�����Χ��
                     //island_state_3_up[0]= Right_Up_Guai[0];
                     //island_state_3_up[1]= Right_Up_Guai[1];
                     k=(float)((float)(MT9V03X_H-Right_Up_Guai[0])/(float)(20-Right_Up_Guai[1]));
                     K_Draw_Line(k,30,MT9V03X_H-1,0);
                     image_process();//ˢ����������
                 }
             }//3״̬����4���������ó��㣬�����������ǣ�����Ҳ���ĺܿ�
             if((rd.state==3)&&(abs(FJ_Angle)>=60))//��ʱʹ�ñ�����������ת��״̬���������ǻ�������
             {
                 k=0;//б������
                 encoder_derdate=0;
                 rd.state=4;
                 image_process();//����ɨ��
             }
         }
    else if(rd.state==4)//����״̬�������⴦��
    {
        if(abs(FJ_Angle)>200)//����200���Ժ��ڴ򿪳����ж�
        {
            monotonicity_change_line[0]=Monotonicity_Change_Left(110,30);//�����Ըı�
            monotonicity_change_line[1]=l_border[monotonicity_change_line[0]];
            if(rd.state==4&&Continuity_Change_Left(110,40)!=0)//�������Ǻ�����������оݣ�������220����
            {
                island_state_5_down[0]=MT9V03X_H-1;
                island_state_5_down[1]=l_border[MT9V03X_H-1]-5;
                if(k==0)
                    k=(float)((float)(MT9V03X_H-monotonicity_change_line[0])/(float)(island_state_5_down[1]-monotonicity_change_line[1]));
                K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);  //�ܸо�������߻�����⣬�п��ٿ���
                rd.state=5;
            }
        }
    }
    else if(rd.state==5)//׼��������
    {
        if(k!=0){
         K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
        }
         if(rd.state==5&&(rd.R_Edgepoint_y<rd.L_Edgepoint_y))//����ȶ���
         {
               rd.state=6;
         }
         if(abs(FJ_Angle)>270)
         {
             rd.state=6;
         }
    }
    else if(rd.state==6)//������
         {
        if(k!=0){
             K_Add_Boundry_Left(k,island_state_5_down[1],island_state_5_down[0],0);
        }
             if((rd.state==6)&&(rd.L_Edgepoint_y>MT9V03X_H-10))//����ȶ���-�˴���������������оݻ���300��
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
    else if(rd.state==7)//�������������ҽǵ�
          {
              Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-50,30);//��ȡ���ϵ����꣬�ҵ���ȥ8
              rd.right_up_guai=Right_Up_Guai[0];
              Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
              if((rd.state==7)&&((Right_Up_Guai[1]>=MT9V03X_W-58&&(5<=Right_Up_Guai[0]&&Right_Up_Guai[0]<=MT9V03X_H-20))))//ע������Ժ������궼��Ҫ����Ϊ������һ������������ֹյ�
              {//���ǵ�λ�ú���ʱ����8
                  rd.state=8;
              }
          }
    else if(rd.state==8)//����8
            {
                Right_Up_Guai[0]=Find_Right_Up_Point(MT9V03X_H-1,10);//��ȡ���ϵ�����
                rd.right_up_guai=Right_Up_Guai[0];
                Right_Up_Guai[1]=r_border[Right_Up_Guai[0]];
                //Lengthen_Right_Boundry(Right_Up_Guai[0]-1,MT9V03X_H-1);
                if(k==0)
                {
                    k=(float)(5/(r_border[Right_Up_Guai[0]-5]-Right_Up_Guai[1]));
                }
                //K_Add_Boundry_Right(k,Right_Up_Guai[1],MT9V03X_H-1,Right_Up_Guai[0]);//���ұ���
                K_Draw_Line(k, MT9V03X_W-5, MT9V03X_H-1, Right_Up_Guai[0]);
                image_process();
                if((rd.state==8)&&(Right_Up_Guai[0]>=MT9V03X_H-20||(Right_Up_Guai[0]<10&&rd.L_Edgepoint_y>=MT9V03X_H-10)))//���յ㿿��ʱ����Ϊ�����ˣ���������
                {//�ǵ㿿�£������¶˲����ߣ���Ϊ������
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
  @brief     ���½ǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Right_Down_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Down_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int right_down_line=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(right_down_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(r_border[i]-r_border[i+1])<=5&&//�ǵ����ֵ���Ը���
           abs(r_border[i+1]-r_border[i+2])<=5&&
              (r_border[i]-r_border[i-1])<=-5&&
              (r_border[i]-r_border[i-2])<=-10&&
              (r_border[i]-r_border[i-3])<=-10)
        {
            right_down_line=i;//��ȡ��������
            break;
        }
    }
    return right_down_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���Ͻǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Right_Up_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int right_up_line=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
        return right_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }

    if(end<=5)//��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(right_up_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(r_border[i]-r_border[i-1])<=6&&//��������λ�ò��
           abs(r_border[i-1]-r_border[i-2])<=6&&
              (r_border[i]-r_border[i+1])<=-6&&
              (r_border[i]-r_border[i+2])<=-8
              )
        {
            right_up_line=i;//��ȡ��������
            break;
        }
    }
    return right_up_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���Ͻǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Left_Up_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Up_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int left_up_line=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
       return left_up_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(end<=5)//��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end=5;
    if(start>=MT9V03X_H-1-5)
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(left_up_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(l_border[i]-l_border[i-1])<=5&&
           abs(l_border[i-1]-l_border[i-2])<=5&&
           abs(l_border[i-2]-l_border[i-3])<=5&&
              (l_border[i]-l_border[i+2])>=8&&
              (l_border[i]-l_border[i+3])>=15&&
              (l_border[i]-l_border[i+4])>=15)
        {
            left_up_line=i;//��ȡ��������
            break;
        }
    }
    return left_up_line;//�����MT9V03X_H-1��˵��û����ô���յ�
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ���½ǵ���
  @param     ��ʼ�㣬��ֹ��
  @return    ���ؽǵ����ڵ��������Ҳ�������0
  Sample     Find_Left_Down_Point(int start,int end);
  @note      �ǵ�����ֵ�ɸ���ʵ��ֵ����
-------------------------------------------------------------------------------------------------------------------*/
int Find_Left_Down_Point(int start,int end)//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
{
    int i,t;
    int left_down_line=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�йյ��жϵ�����
        return left_down_line;
    if(start<end)
    {
        t=start;
        start=end;
        end=t;
    }
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(left_down_line==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(l_border[i]-l_border[i+1])<=6&&//�ǵ����ֵ���Ը���
           abs(l_border[i+1]-l_border[i+2])<=6&&
              (l_border[i]-l_border[i-2])>=5&&
              (l_border[i]-l_border[i-3])>=10&&
              (l_border[i]-l_border[i-4])>=10)
        {
            left_down_line=i;//��ȡ��������
            break;
        }
    }
    return left_down_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     �����������Լ��
  @param     ��ʼ�㣬��ֹ��
  @return    ��������0�����������ض��߳�����
  Sample     continuity_change_flag=Continuity_Change_Right(int start,int end)
  @note      �����Ե���ֵ����Ϊ5���ɸ���
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Right(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û��Ҫ�ж���
       return 1;
    if(start>=MT9V03X_H-5)//����Խ�籣��
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(r_border[i]-r_border[i-1])>=5)//��������ֵ��5���ɸ���
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     �����������Լ��
  @param     ��ʼ�㣬��ֹ��
  @return    ��������0�����������ض��߳�����
  Sample     continuity_change_flag=Continuity_Change_Left(int start,int end)
  @note      �����Ե���ֵ����Ϊ5���ɸ���
-------------------------------------------------------------------------------------------------------------------*/
int Continuity_Change_Left(int start,int end)
{
    int i;
    int t;
    int continuity_change_flag=0;
    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û��Ҫ�ж���
       return 1;
    if(start>=MT9V03X_H-5)//����Խ�籣��
        start=MT9V03X_H-5;
    if(end<=5)
       end=5;
    if(start<end)//���Ǵ������ϼ���ģ����˾ͻ���һ��
    {
       t=start;
       start=end;
       end=t;
    }

    for(i=start;i>=end;i--)
    {
        if(abs(l_border[i]-l_border[i-1])>=5)//��������ֵ��5���ɸ���
       {
            continuity_change_flag=i;
            break;
       }
    }
    return continuity_change_flag;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ͻ����-�ұ�
  @param     ��ʼ�㣬��ֹ��
  @return    �����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      ǰ5��5�������С�����������ǽǵ�
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Right(int start,int end)//�����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line=0;

    if(rd.Right_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�е������жϵ�����
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//����Խ�籣��
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if(r_border[i]==r_border[i+5]&&r_border[i]==r_border[i-5]&&
        r_border[i]==r_border[i+4]&&r_border[i]==r_border[i-4]&&
        r_border[i]==r_border[i+3]&&r_border[i]==r_border[i-3]&&
        r_border[i]==r_border[i+2]&&r_border[i]==r_border[i-2]&&
        r_border[i]==r_border[i+1]&&r_border[i]==r_border[i-1])
        {//һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if(r_border[i] <r_border[i+5]&&r_border[i] <r_border[i-5]&&
        r_border[i] <r_border[i+4]&&r_border[i] <r_border[i-4]&&
        r_border[i]<=r_border[i+3]&&r_border[i]<=r_border[i-3]&&
        r_border[i]<=r_border[i+2]&&r_border[i]<=r_border[i-2]&&
        r_border[i]<=r_border[i+1]&&r_border[i]<=r_border[i-1])
        {//�ͺܱ����������������ǰ5����5�����ģ��Ǿ��ǵ���ͻ���
            monotonicity_change_line=i;
            break;
        }
    }
    return monotonicity_change_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������ͻ����-���
  @param     ��ʼ�㣬��ֹ��
  @return    �����ڵ��������Ҳ�������0
  Sample     Find_Right_Up_Point(int start,int end);
  @note      ǰ5��5�������С�����������ǽǵ�
-------------------------------------------------------------------------------------------------------------------*/
int Monotonicity_Change_Left(int start,int end)//�����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
{
    int i;
    int monotonicity_change_line=0;

    if(rd.Left_Lost_Time>=0.9*MT9V03X_H)//�󲿷ֶ����ߣ�û�е������жϵ�����
        return monotonicity_change_line;
    if(start>=MT9V03X_H-1-5)//����Խ�籣��
        start=MT9V03X_H-1-5;
     if(end<=5)
        end=5;
    if(start<=end)
        return monotonicity_change_line;
    for(i=start;i>=end;i--)//���ȡǰ5��5���ݣ�����ǰ������뷶Χ��Ҫ��
    {
        if(l_border[i]==l_border[i+5]&&l_border[i]==l_border[i-5]&&
        l_border[i]==l_border[i+4]&&l_border[i]==l_border[i-4]&&
        l_border[i]==l_border[i+3]&&l_border[i]==l_border[i-3]&&
        l_border[i]==l_border[i+2]&&l_border[i]==l_border[i-2]&&
        l_border[i]==l_border[i+1]&&l_border[i]==l_border[i-1])
        {//һ������һ������Ȼ������Ϊ����ת�۵�
            continue;
        }
        else if(l_border[i] >l_border[i+5]&&l_border[i] >l_border[i-5]&&
        l_border[i] >l_border[i+4]&&l_border[i] >l_border[i-4]&&
        l_border[i]>=l_border[i+3]&&l_border[i]>=l_border[i-3]&&
        l_border[i]>=l_border[i+2]&&l_border[i]>=l_border[i-2]&&
        l_border[i]>=l_border[i+1]&&l_border[i]>=l_border[i-1])
        {//�ͺܱ����������������ǰ5����5�����ģ��Ǿ��ǵ���ͻ���
            monotonicity_change_line=i;
            break;
        }
    }
    return monotonicity_change_line;
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ͨ��б�ʣ����㲹��--������
  @param     k       ����б��
             startY  ������ʼ��������
             endY    ������������
  @return    null
  Sample     K_Add_Boundry_Left(float k,int startY,int endY);
  @note      ������ֱ�����ڱ�����
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
    if(startY<endY)//--������start��Ҫ��
    {
        t=startY;
        startY=endY;
        endY=t;
    }
//������bug���·�ѭ��--ѭ������ҪstartҪ��ֻ����y�Ļ���������û�н���x�Ļ���
//��������жϣ����start��С���Ǿͽ���++����
//�����޸ĸ�λ���в���
    for(i=startY;i>=endY;i--)
    {
        l_border[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
  @brief     ͨ��б�ʣ����㲹��
  @param     k       ����б��
             startY  ������ʼ��������
             endY    ������������
  @return    null    ֱ�Ӳ�����
  Sample     K_Add_Boundry_Right(float k,int startY,int endY);
  @note      ������ֱ�����ڱ�����
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
        r_border[i]=(int)((i-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
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
// ������Щ������ĳ���Ѿ�����
uint8_t Pixle[35][100]; // ʵ�ʵĶ�ά�������鶨��
uint8_t garageout_flag = 0; // ��ʼ����־

uint8_t region = 0;
uint8_t garage_count = 0;
uint8_t white_black = 0;
uint8_t black_white = 0;
void crosswalk(void) {

    for (uint8_t hang = 20; hang < 35; hang++) {
        garage_count = 0;
        black_white = !white_black; // ���ã��Ա�ÿ�����п�ʼʱ�����仯���
        for (uint8_t lie = 10; lie < 100; lie++) {
            white_black = (bin_image[hang][lie] == white_pixel) ? 1 : 0;

            if (white_black != black_white) {
                black_white = white_black;
                garage_count++;
            }
            if (garage_count > 11) {
                region++;
                break; // һ����һ���м�⵽�㹻�ı仯��������ѭ��
            }
        }
        if (region > 2) {
            garageout_flag = 1;
            break; // ���㹻������м�⵽�仯���˳�
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ����
  @param     ���ߵ���㣬�յ�
  @return    null
  Sample     Left_Add_Line(int x1,int y1,int x2,int y2);
  @note      ����ֱ���Ǳ߽磬������ǿ��Ŷȸߵ�,��Ҫ�Ҳ�
-------------------------------------------------------------------------------------------------------------------*/
void Left_Add_Line(int x1,int y1,int x2,int y2)//����,�����Ǳ߽�
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//��ʼ��λ��У�����ų�����Խ��Ŀ���
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
    if(a1>a2)//���껥��
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//����б�ʲ��߼���
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
  @brief     �Ҳ���
  @param     ���ߵ���㣬�յ�
  @return    null
  Sample     Right_Add_Line(int x1,int y1,int x2,int y2);
  @note      ����ֱ���Ǳ߽磬������ǿ��Ŷȸߵģ���Ҫ�Ҳ�
-------------------------------------------------------------------------------------------------------------------*/
void Right_Add_Line(int x1,int y1,int x2,int y2)//�Ҳ���,�����Ǳ߽�
{
    int i,max,a1,a2;
    int hx;
    if(x1>=MT9V03X_W-1)//��ʼ��λ��У�����ų�����Խ��Ŀ���
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
    if(a1>a2)//���껥��
    {
        max=a1;
        a1=a2;
        a2=max;
    }
    for(i=a1;i<=a2;i++)//����б�ʲ��߼���
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
  @brief     ��߽��ӳ�
  @param     �ӳ���ʼ�������ӳ���ĳ��
  @return    null
  Sample     Stop_Detect(void)
  @note      ����ʼ��������5���㣬���б�ʣ������ӳ���ֱ��������
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Left_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//��ʼ��λ��У�����ų�����Խ��Ŀ���
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++���ʣ����껥��
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//��Ϊ��Ҫ�ڿ�ʼ��������3���㣬������ʼ����ڿ��ϣ��������ӳ���ֻ��ֱ������
    {
         Left_Add_Line(l_border[start],start,l_border[end],end);
    }

    else
    {
        k=(float)(l_border[start]-l_border[start-4])/5.0;//�����k��1/б��
        for(i=start;i<=end;i++)
        {
            l_border[i]=(int)(i-start)*k+l_border[start];//(x=(y-y1)*k+x1),��бʽ����
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
  @brief     ����߽��ӳ�
  @param     �ӳ���ʼ�������ӳ���ĳ��
  @return    null
  Sample     Stop_Detect(void)
  @note      ����ʼ��������3���㣬���б�ʣ������ӳ���ֱ��������
-------------------------------------------------------------------------------------------------------------------*/
void Lengthen_Right_Boundry(int start,int end)
{
    int i,t;
    float k=0;
    if(start>=MT9V03X_H-1)//��ʼ��λ��У�����ų�����Խ��Ŀ���
        start=MT9V03X_H-1;
    else if(start<=0)
        start=0;
    if(end>=MT9V03X_H-1)
        end=MT9V03X_H-1;
    else if(end<=0)
        end=0;
    if(end<start)//++���ʣ����껥��
    {
        t=end;
        end=start;
        start=t;
    }

    if(start<=5)//��Ϊ��Ҫ�ڿ�ʼ��������3���㣬������ʼ����ڿ��ϣ��������ӳ���ֻ��ֱ������
    {
        Right_Add_Line(r_border[start],start,r_border[end],end);
    }
    else
    {
        k=(float)(r_border[start]-r_border[start-4])/5.0;//�����k��1/б��
        for(i=start;i<=end;i++)
        {
            r_border[i]=(int)(i-start)*k+r_border[start];//(x=(y-y1)*k+x1),��бʽ����
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
  @brief     ����б�ʻ���
  @param     ����б�ʣ����㣬��һ������
  @return    null
  Sample     K_Draw_Line(k, 20,MT9V03X_H-1 ,0)
  @note      ���ľ���һ���ߣ���Ҫ����ɨ��
-------------------------------------------------------------------------------------------------------------------*/
void K_Draw_Line(float k, int startX, int startY,int endY)
{
    int endX=0;

    if(startX>=MT9V03X_W-1)//�޷�����
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
    endX=(int)((endY-startY)/k+startX);//(y-y1)=k(x-x1)���Σ�x=(y-y1)/k+x1
    Draw_Line(startX,startY,endX,endY);
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ����
  @param     ������ʼ�㣬�յ����꣬��һ�����Ϊ2�ĺ���
  @return    null
  Sample     Draw_Line(0, 0,MT9V03X_W-1,MT9V03X_H-1);
             Draw_Line(MT9V03X_W-1, 0,0,MT9V03X_H-1);
                                    ��һ�����
  @note     ���ľ���һ���ߣ���Ҫ����ɨ��
-------------------------------------------------------------------------------------------------------------------*/
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i,x,y;
    int start=0,end=0;
    if(startX>=MT9V03X_W-1)//�޷�����
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
    if(startX==endX)//һ������
    {
        if (startY > endY)//����
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
    else if(startY == endY)//��һ������
    {
        if (startX > endX)//����
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
    else //����������ˮƽ����ֱ��������������ǳ������
    {
        if(startY>endY)//��ʼ�����
        {
            start=endY;
            end=startY;
        }
        else
        {
            start=startY;
            end=endY;
        }
        for (i = start; i <= end; i++)//�����ߣ���֤ÿһ�ж��кڵ�
        {
            x =(int)(startX+(endX-startX)*(i-startY)/(endY-startY));//����ʽ����
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
        for (i = start; i <= end; i++)//�����ߣ���֤ÿһ�ж��кڵ�
        {

            y =(int)(startY+(endY-startY)*(i-startX)/(endX-startX));//����ʽ����
            if(y>=MT9V03X_H-1)
                y=MT9V03X_H-1;
            else if (y<=0)
                y=0;
            bin_image[y][i] = black_pixel;
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������������յ㣬��ʮ��ʹ��
  @param     �����ķ�Χ��㣬�յ�
  @return    �޸�����ȫ�ֱ���
             Right_Down_Find=0;
             Left_Down_Find=0;
  Sample     Find_Down_Point(int start,int end)
  @note      ������֮��鿴��Ӧ�ı�����ע�⣬û�ҵ�ʱ��Ӧ��������0
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
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    if(end<=MT9V03X_H-Search_Stop_Line)
        end=MT9V03X_H-Search_Stop_Line;
    if(end<=5)
       end=5;
    for(i=start;i>=end;i--)
    {
        if(Left_Down_Find==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(l_border[i]-l_border[i+1])<=5&&//�ǵ����ֵ���Ը���
           abs(l_border[i+1]-l_border[i+2])<=5&&
           abs(l_border[i+2]-l_border[i+3])<=5&&
              (l_border[i]-l_border[i-2])>=8&&
              (l_border[i]-l_border[i-3])>=15&&
              (l_border[i]-l_border[i-4])>=15)
        {
            Left_Down_Find=i;//��ȡ��������
        }
        if(Right_Down_Find==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(r_border[i]-r_border[i+1])<=5&&//�ǵ����ֵ���Ը���
           abs(r_border[i+1]-r_border[i+2])<=5&&
           abs(r_border[i+2]-r_border[i+3])<=5&&
              (r_border[i]-r_border[i-2])<=-8&&
              (r_border[i]-r_border[i-3])<=-15&&
              (r_border[i]-r_border[i-4])<=-15)
        {
            Right_Down_Find=i;
        }
        if(Left_Down_Find!=0&&Right_Down_Find!=0)//�����ҵ����˳�
        {
            break;
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ������������յ㣬��ʮ��ʹ��
  @param     �����ķ�Χ��㣬�յ�
  @return    �޸�����ȫ�ֱ���
             Left_Up_Find=0;
             Right_Up_Find=0;
  Sample     Find_Up_Point(int start,int end)
  @note      ������֮��鿴��Ӧ�ı�����ע�⣬û�ҵ�ʱ��Ӧ��������0
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
    if(end<=5)//��ʱ����зǳ�����ҲҪ�������ֵ㣬��ֹ����Խ��
        end=5;
    if(start>=MT9V03X_H-1-5)//����5�����ݲ��ȶ���������Ϊ�߽�����жϣ�����
        start=MT9V03X_H-1-5;
    for(i=start;i>=end;i--)
    {
        if(Left_Up_Find==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(l_border[i]-l_border[i-1])<=5&&
           abs(l_border[i-1]-l_border[i-2])<=5&&
           abs(l_border[i-2]-l_border[i-3])<=5&&
              (l_border[i]-l_border[i+2])>=8&&
              (l_border[i]-l_border[i+3])>=15&&
              (l_border[i]-l_border[i+4])>=15)
        {
            Left_Up_Find=i;//��ȡ��������
        }
        if(Right_Up_Find==0&&//ֻ�ҵ�һ�����������ĵ�
           abs(r_border[i]-r_border[i-1])<=5&&//��������λ�ò��
           abs(r_border[i-1]-r_border[i-2])<=5&&
           abs(r_border[i-2]-r_border[i-3])<=5&&
              (r_border[i]-r_border[i+2])<=-8&&
              (r_border[i]-r_border[i+3])<=-15&&
              (r_border[i]-r_border[i+4])<=-15)
        {
            Right_Up_Find=i;//��ȡ��������
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//���������ҵ��ͳ�ȥ
        {
            break;
        }
    }
    if(abs(Right_Up_Find-Left_Up_Find)>=30)//����˺�ѹ�����Ϊ����
    {
        Right_Up_Find=0;
        Left_Up_Find=0;
    }
}

/*-------------------------------------------------------------------------------------------------------------------
  @brief     ʮ�ּ��
  @param     null
  @return    null
  Sample     Cross_Detect(void);
  @note      �����ĸ��յ��б����������ĸ��ǵ㣬�����ҵ��յ�ĸ��������Ƿ���
-------------------------------------------------------------------------------------------------------------------*/
void Cross_Detect()
{
    int down_search_start=0;//�µ�������ʼ��
    Cross_Flag=0;
    if(rd.state==0&&rd.Ring_Flag==0)//�뻷�����⿪
    {
        Left_Up_Find=0;
        Right_Up_Find=0;
        if(rd.Both_Lost_Time>=10)//ʮ�ֱض���˫�߶��ߣ�����˫�߶��ߵ�������ٿ�ʼ�ҽǵ�
        {
            Find_Up_Point( MT9V03X_H-1, 0 );
            if(Left_Up_Find==0&&Right_Up_Find==0)//ֻҪû��ͬʱ�ҵ������ϵ㣬ֱ�ӽ���
            {
                return;
            }
        }
        if(Left_Up_Find!=0&&Right_Up_Find!=0)//�ҵ������ϵ㣬���ҵ�ʮ����
        {
            Cross_Flag=1;//��Ӧ��־λ�����ڸ�Ԫ�ػ����
            down_search_start=Left_Up_Find>Right_Up_Find?Left_Up_Find:Right_Up_Find;//�������Ϲյ����꿿������Ϊ�µ����������
            Find_Down_Point(MT9V03X_H-5,down_search_start+2);//���Ϲյ���2����Ϊ�µ�Ľ�ֹ��
            if(Left_Down_Find<=Left_Up_Find)
            {
                Left_Down_Find=0;//�µ㲻���ܱ��ϵ㻹����
            }
            if(Right_Down_Find<=Right_Up_Find)
            {
                Right_Down_Find=0;//�µ㲻���ܱ��ϵ㻹����
            }
            if(Left_Down_Find!=0&&Right_Down_Find!=0)
            {//�ĸ��㶼�ڣ��������ߣ����������Ȼ����
                Left_Add_Line (l_border [Left_Up_Find ],Left_Up_Find ,l_border [Left_Down_Find ] ,Left_Down_Find);
                Right_Add_Line(r_border[Right_Up_Find],Right_Up_Find,r_border[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find==0&&Right_Down_Find!=0)//11//����ʹ�õĶ���б�ʲ���
            {//������                                     //01
                Lengthen_Left_Boundry(Left_Up_Find-1,MT9V03X_H-1);
                Right_Add_Line(r_border[Right_Up_Find],Right_Up_Find,r_border[Right_Down_Find],Right_Down_Find);
            }
            else if(Left_Down_Find!=0&&Right_Down_Find==0)//11
            {//������                                     //10
                Left_Add_Line (l_border [Left_Up_Find ],Left_Up_Find ,l_border [Left_Down_Find ] ,Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
            else if(Left_Down_Find==0&&Right_Down_Find==0)//11
            {//�����ϵ�                                   //00
                Lengthen_Left_Boundry (Left_Up_Find-1,MT9V03X_H-1);
                Lengthen_Right_Boundry(Right_Up_Find-1,MT9V03X_H-1);
            }
        }
        else
        {
            Cross_Flag=0;
        }
    }
    //�ǵ���ر�����debugʹ��
    //ips200_showuint8(0,12,Cross_Flag);
//    ips200_showuint8(0,13,Island_State);
//    ips200_showuint8(50,12,Left_Up_Find);
//    ips200_showuint8(100,12,Right_Up_Find);
//    ips200_showuint8(50,13,Left_Down_Find);
//    ips200_showuint8(100,13,Right_Down_Find);
}
