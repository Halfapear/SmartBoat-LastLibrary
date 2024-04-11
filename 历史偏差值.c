#include <vector>

std::vector<float> deviationHistory; // 存储历史偏差值

// 计算当前帧的偏差，并更新历史偏差列表
float calculateCurrentDeviation() {
    float currentDeviation = calculateDeviation(); // 假设的函数
    deviationHistory.push_back(currentDeviation);
    
    // 保持历史数据长度，这里假设用最近的10个数据
    if (deviationHistory.size() > 10) {
        deviationHistory.erase(deviationHistory.begin());
    }
    
    return currentDeviation;
}

// 使用历史数据判断直道或弯道
bool isStraightPath() {
    float averageDeviation = 0;
    for (auto deviation : deviationHistory) {
        averageDeviation += deviation;
    }
    averageDeviation /= deviationHistory.size();
    
    // 检查平均偏差是否在某个阈值范围内，这里阈值需要根据实际情况调整
    if (std::abs(averageDeviation) < someThreshold) {
        return true; // 直道
    } else {
        return false; // 弯道
    }
}
