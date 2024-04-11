void Display_Initial_Menu()
{

    tft180_show_string ( 0,  16*0, "1.Grayscale Image");

    tft180_show_string ( 0,  16*1, "2.Turn.P");
    tft180_show_string ( 0,  16*2, "3.Turn.I");
    tft180_show_string ( 0,  16*3, "4.Turn.D");
    tft180_show_string ( 0,  16*4, "5.Speed.P");
//    tft180_show_string ( 0,  16*5, "6.Speed.I");
//没位置显示I了，但还在的
//这个乱码什么玩意，只能晚点总结的时候一点点粘了
    tft180_show_string ( 0,  16*5, "7.Speed.D");
    tft180_show_int (0, 16*6,  encoder_data_quaddec,4);
//���� ��ʾ���ֺ���ʾ�ַ�����ʲô��������������ܲ�֧��ֱ����ʾ��ASCII�ַ������纺�ֵ�
}

void Adjust_TurnI(void) {
    if (KEY_SHORT_PRESS == key_get_state(KEY_1)) {
        Turn.I += 0.1; 
    }
    else if (KEY_SHORT_PRESS == key_get_state(KEY_2)) {
        Turn.I -= 0.1; 
        if (Turn.I < 0) Turn.I = 0; 
    }
    tft180_show_string ( 0,  16*0, "5.Turn_I");
    tft180_show_float(0, 16*2, Turn.I, 1, 6);
}

void Display_Second_Menu(uint8_t *List_Number_p){
            if(*List_Number_p ==1)
            {
                //��ʵ�ֵĹ���
                Display_Grayscale_Image();

                // ����ⲿ�жϴ�����־λ


            }
            else if(*List_Number_p==2)
            {
                //��ʵ�ֵĹ���
                Adjust_TurnP();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==3)
            {
                //��ʵ�ֵĹ���
                Adjust_TurnI();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==4)
            {
                //��ʵ�ֵĹ���
                Adjust_TurnD();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==5)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedP();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==6)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedI();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==7)
            {
                //��ʵ�ֵĹ���
                Adjust_SpeedD();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            else if(*List_Number_p==8)
            {
                //��ʵ�ֵĹ���
                Adjust_max_angle();
                // ����ⲿ�жϴ�����־λ
                exti_state[3] = 0;
            }
            //��һ��key3�ж�״̬
            exti_state[2] = 0;
}


