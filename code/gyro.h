#include "common.h"
/* ÁéÃô¶È */
#define   GYRO_SENS             1/16.4
#define   ACCE_SENS             90.0/4096
#define   DT                    0.01

/**********ÉêÃ÷±äÁ¿*************/
extern volatile float FJ_Pitch;//Æ«º½½Ç
extern volatile float FJ_Angle;//¸©Ñö½Ç

void Gyroscope_Init(void);
void Gyroscope_GetData(void);
void Zero_Point_Detect(void);
void Get_Gyroscope_Angle(void);
void Clear_Gyroscope_Angle(void);
void Get_Gyroscope_Pitch(void);
void Clear_Gyroscope_Pitch(void);


