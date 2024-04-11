//4.5晚加了点东西
//应用版的主函数在image.c(我喜欢放camera)；然后我的判断函数没用上，正式版在isr.c;在这里改三个参数；
//Speed.zhidao_Speed什么的在motor那边，如果想改pid就在isr.c那里改然后给其他

int16 straight=1;
//记得改h文件

int16 findMaxTransitionFromWhiteToBlack(int cols, int colf) {
    int maxY = -1; // 用于记录最大的y值，初始化为-1表示还没找到
    // 遍历指定的列范围
    for (int col = cols; col <= colf; col++) {
        for (int row = 0; row < image_h - 1; row++) {
            // 检查当前像素是不是白色，并且下一个像素是黑色
            if (bin_image[row][col] == 255 && bin_image[row - 1][col] == 0) {
                // 发现白到黑的变化，更新maxY
                if (row > maxY) {
                    maxY = row; // 更新最大的y值
                }
                break; // 找到第一个变化后就停止搜索当前列
            }
        }
    }
    return maxY; // 返回找到的最大y值
}

int findMaxTransitionFromWhiteToBlack(int cols, int colf) {
    int maxY = 117; // 用于记录最大的y值，初始化为-1表示还没找到
    // 遍历指定的列范围
    for (int col = cols; col <= colf; col+=3) {
        for (int row = image_h-1; row >3; row--) {
            // 检查当前像素是不是白色，并且下一个像素是黑色
            if (bin_image[row][col] == white_pixel &&bin_image[row+1][col]==white_pixel&&
                bin_image[row - 1][col] == black_pixel&&bin_image[row-2][col]==black_pixel) {
                // 发现白到黑的变化，更新maxY
                if (row < maxY) {
                    maxY = row; // 更新最大的y值
                }
                break; // 找到第一个变化后就停止搜索当前列
            }
        }
    }
    return image_h-maxY; // 返回找到的最大y值
}

//记好了 是120*188
//可以修改的是cols, colf和直道白阵列阙值（还没写出来，加在判断里就行）
//this is judge(and the parameter changed here )
void judgeStraight(){
    if (findMaxTransitionFromWhiteToBlack(30, 150)<90){
        straight = 0;
    }
    else{
        straight = 1;
    }
}

//测试的时候可以在显示屏上放一个（放图像那里吧）
//这个可以直接在我这测
void Display_Grayscale_Image(void)
{
    tft180_set_dir(TFT180_CROSSWISE);                                           // ��Ҫ�Ⱥ��� ��Ȼ��ʾ����

            //tft180_show_int(0,16*4,encoder_data_quaddec ,5);

   //tft180_show_gray_image(0,0,mt9v03x_image[0],image_w,image_h,160,128,0);


        //tft180_show_gray_image(0,0,mt9v03x_image[0],image_w,image_h,160,128,0);
        tft180_show_gray_image(0,0,bin_image[0],image_w,image_h,160,128,0);
//        tft180_show_int(0,0,Speed.Speed_Car,4);
        tft180_show_int(0,0,Turn.Chazhi,4);
        tft180_show_int(0,16*2,Turn.PWM_Lout,5);
        tft180_show_int(0,16*3,Turn.PWM_Rout,5);
        tft180_show_int(0,16*4,encoder_data_quaddec ,5);
        
        tft180_show_int(0,16*5,straight ,5);

//下面好像是圆环的示数
//   tft180_show_gray_image(0,0,bin_image[0],image_w,image_h,160,128,0);
//    tft180_show_int(0,0,rd.state,4);
//                tft180_show_int(0,16,rd.state2_time,5);
//                tft180_show_int(0,16*2,rd.L_Edgepoint_y,4);
//                //tft180_show_int(0,16*3,rd.add_k,4);
//                tft180_show_float(0,16*3,rd.add_k,2,3);
//                tft180_show_int(0,16*4,FJ_Angle,4);
    for (int16 i = 0; i <image_h-1; i++)
       {
        tft180_draw_point(94,i,RGB565_YELLOW);
        if(center_line[i]<160){
                tft180_draw_point(center_line[i], i, uesr_RED);//��ʾ��� ��ʾ����
                }
                if(l_border[i]<160)
                {
                    tft180_draw_point(l_border[i], i, uesr_GREEN);//��ʾ��� ��ʾ�����
                }
                if(r_border[i]<160)
                {
                    tft180_draw_point(r_border[i], i, uesr_BLUE);//��ʾ��� ��ʾ�ұ���
                }
       }

}

//h文件部分
// 全局变量声明
extern int16 straight;

// 函数声明
int16 findMaxTransitionFromWhiteToBlack(int cols, int colf);
void judgeStraight();



//然后写两套pid，然后再pwm_out里面或是其它地方判断（说实话，需要改的东西有点多，要不直接换成增量式？）
//我先写完再跟他说PID需要分别试直弯吧（直跑弯速当然可以，但比不过别人怎么办）


//补充
//为保险起见，最好保证这个白色上方至少有三个连续黑块

/*
int findMaxTransitionWithBlackAbove(int cols, int colf) {
    int maxY = -1; // 用于记录最大的y值，初始化为-1表示还没找到

    // 遍历指定的列范围
    for (int col = cols; col <= colf; col++) {
        for (int row = 3; row < image_h; row++) { // 从第三行开始，确保可以检查当前点上方的三个黑色像素
            // 检查当前点是不是白色，并且上方有至少三个连续黑色像素
            if (bin_image[row][col] == 255 && // 当前点是白色
                bin_image[row - 1][col] == 0 && // 上方第一个像素是黑色
                bin_image[row - 2][col] == 0 && // 上方第二个像素是黑色
                bin_image[row - 3][col] == 0) { // 上方第三个像素是黑色
                
                // 找到符合条件的白色像素，更新maxY
                if (row > maxY) {
                    maxY = row; // 更新最大的y值
                }
                break; // 找到后就可以停止检查当前列的其它像素
            }
        }
    }

    return maxY; // 返回找到的最大y值
}
*/
