#include "common.h"
#include "image.h"
#include "motor.h"
#include "gyro.h"

//�궨��
#define image_h 120//ͼ��߶�
#define image_w 188//ͼ����
#define rd_border_max  image_w-3 //�߽����ֵ
#define rd_border_min  2   //�߽���Сֵ

typedef struct
{
    uint8 state;//0��ʾδʶ��Բ����1��ʼΪ����״̬��ÿ���л�״̬��һ����
    uint8 Inflection_point_x,Inflection_point_y;//��¼�յ������
    uint16 Left_Lost_Time,Right_Lost_Time;//���ұ߽�Ķ�������
    uint8 Rd_Flag_1,Rd_Flag_2,Rd_Flag_3,Rd_Flag_4;//����
    uint8 Ring_Flag;//�������һ��ı�־λ��1λ��2Ϊ�ң�0Ϊ�ǻ���
    uint16 Ring_time;//���ڼ�¼��Բ���ڸ�״̬��ʱ�䣬�����ж�
    uint16 count;//��������
    uint8 Ring_Start;
    uint8 Ring_Start_L;//Բ�������
    uint8 Ring_Start_R;//Բ�������
    uint16 No_Ring_Flag;//Բ�����м���
    uint8 L_Edgepoint_x,L_Edgepoint_y,R_Edgepoint_x,R_Edgepoint_y;
    uint16 Ring_Leave_time;
    uint16 Both_Lost_Time;//����ͬʱ������
    float add_k;
    uint16 right_up_guai;//���Ϲյ�����
    uint16 state2_time;
}Round;


extern uint8 Cross_Flag;
extern int16 LUJ;
extern int16 RUJ;
extern int16 LDJ;
extern int16 RDJ;
extern int16 Left_Down_Find; //ʮ��ʹ�ã��ҵ�����������û�ҵ�����0
extern int16 Left_Up_Find;   //�ĸ��յ��־
extern int16 Right_Down_Find;
extern int16 Right_Up_Find;

extern Round rd;
//extern uint8_t image[35][100]; // �������������������Ĵ�С
extern uint8_t garageout_flag; // ����ָʾ�Ƿ��⵽�����ߵı�־

extern uint8_t region ;
extern uint8_t garage_count ;
extern uint8_t white_black ;
extern uint8_t black_white;

extern uint8_t region ;
extern uint8_t garage_count ;
extern uint8_t white_black ;
extern void Angle_calculate();
extern float Err_Sum(void);
void Round_init();
void GetLostTime();
void Get_Edge_Point();
void Get_Four_jiao();

void Ring_Search();
void Right_Ring();
void Left_Ring();
int Ring_Start_Test();//Բ����ʼ�ı�־�ж�
void Cross_Detect();//ʮ�ֲ���
//����Ϊ��־λ�жϺ���
int Find_Right_Down_Point(int start,int end);
int Find_Right_Up_Point(int start,int end);
int Find_Left_Up_Point(int start,int end);
int Find_Left_Down_Point(int start,int end);//���ĸ��ǵ㣬����ֵ�ǽǵ����ڵ�����
int Continuity_Change_Right(int start,int end);
int Continuity_Change_Left(int start,int end);//�����Լ���
int Monotonicity_Change_Right(int start,int end);
int Monotonicity_Change_Left(int start,int end);//�����Ըı䣬����ֵ�ǵ����Ըı�����ڵ�����
//����Ϊ���ߺ���
void Add_line_from_left();//��׼������
void Add_line_from_right();
void Left_Add_Line(int x1,int y1,int x2,int y2);
void Right_Add_Line(int x1,int y1,int x2,int y2);//������
void Lengthen_Left_Boundry(int start,int end);
void Lengthen_Right_Boundry(int start,int end);//�߽��ӳ�
void K_Add_Boundry_Left(float k,int startX,int startY,int endY);
void K_Add_Boundry_Right(float k,int startX,int startY,int endY);//б�ʲ�����
void K_Draw_Line(float k, int startX, int startY,int endY);//ͼ���ڻ���
void Draw_Line(int startX, int startY, int endX, int endY);//������ʼ�㣬�յ����꣬��һ�����Ϊ2�ĺ���

// ��������
void crosswalk(void);
