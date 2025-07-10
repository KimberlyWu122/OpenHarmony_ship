#ifndef AUTOPILOT_UTILS_H
#define AUTOPILOT_UTILS_H

#include <stdbool.h>

// 获取当前经度
double get_current_gps_lng();

// 获取当前纬度
double get_current_gps_lat();

// 获取当前罗盘角度
double get_current_heading();

// 计算当前坐标到目标坐标的理论方位角
double calculate_bearing(double lat1, double lng1, double lat2, double lng2);

// 判断当前航向角是否接近目标航向角
bool is_heading_close(double current, double target, double tolerance);

bool is_obstacle_detected(float threshold_cm);

#endif  // AUTOPILOT_UTILS_H
