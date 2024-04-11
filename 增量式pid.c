void TurnPD_IncrementalControl() {
    // 计算当前误差
    float current_error = 0.9 * Turn.Chazhi + 0.1 * Turn.last_error;

    // 应用响应下限，如果误差在±2之间，则认为误差为0
    if (current_error < 2 && current_error > -2) {
        current_error = 0;
    }

    // 计算误差的增量
    float error_increment = current_error - Turn.last_error;

    // 计算输出的增量：基于PID公式
    float output_increment = Turn.P * error_increment + Turn.I * current_error + Turn.D * (error_increment - Turn.prev_delta_error);

    // 更新PWM输出：将增量添加到当前输出上
    Turn.PWM_Dout += output_increment;

    // 应用输出限制，确保PWM输出在-80到80之间
    if (Turn.PWM_Dout > 80) {
        Turn.PWM_Dout = 80;
    } else if (Turn.PWM_Dout < -80) {
        Turn.PWM_Dout = -80;
    }

    // 更新变量以备下次使用
    Turn.last_error = current_error;
    Turn.prev_delta_error = error_increment;
}

//第二天4/5 验证一下有没有bug

//下面这个可以直接在现有的结构体基础上补充
typedef struct {
    float Chazhi;         // 当前误差
    float Chazhi_old;     // 上一次的误差
    float PWM_Lout;       // 左侧PWM输出
    float PWM_Rout;       // 右侧PWM输出
    float PWM_Dout;       // 当前的控制输出
    float intergrator;    // 积分项累加值
    float error;          // 当前误差
    float last_error;     // 上次的误差
    float prev_delta_error; // 上一次的误差增量，用于微分项计算
    float delta_output;   // 输出的增量
    float turnP;          // P项输出
    float turnI;          // I项输出
    float turnD;          // D项输出
    float P;              // 比例系数
    float I;              // 积分系数
    float D;              // 微分系数
} Turn_ct;

// 这也是补充
void Para_init() {
    // ...其它初始化代码...
    Turn.prev_delta_error = 0;
    Turn.delta_output = 0;
    // ...其它初始化代码...
}

