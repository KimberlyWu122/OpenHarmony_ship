#include <math.h>
#include "gps.h"
#include "hmc5883l_sensor.h"

extern GPS_Coordinate g_current_gps;

// 获取当前经度
double get_current_gps_lng() {
    return g_current_gps.valid ? g_current_gps.longitude : 0.0;
}

// 获取当前纬度
double get_current_gps_lat() {
    return g_current_gps.valid ? g_current_gps.latitude : 0.0;
}

// 获取当前航向角（基于罗盘）
double get_current_heading() {
    return HMC5883L_ReadAngle();  // 调用罗盘模块的角度读取函数
}

// 计算两点之间的方位角
// lat1/lng1：当前坐标；lat2/lng2：目标坐标
double calculate_bearing(double lat1, double lng1, double lat2, double lng2) {
    double dLon = (lng2 - lng1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double y = sin(dLon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(dLon);

    double brng = atan2(y, x) * 180.0 / M_PI;
    brng = fmod((brng + 360.0), 360.0);
    return brng;
}

// 判断两个角度是否接近
bool is_heading_close(double current, double target, double tolerance) {
    double diff = fabs(current - target);
    if (diff > 180.0) diff = 360.0 - diff;
    return diff <= tolerance;
}

bool is_obstacle_detected(float threshold_cm) {
    float distance = UltrasonicSensorRead();
    if (distance > 0 && distance < threshold_cm) {
        printf("[OBSTACLE] 距离障碍物 %.2f cm，触发避障逻辑
", distance);
        return true;
    }
    return false;
}