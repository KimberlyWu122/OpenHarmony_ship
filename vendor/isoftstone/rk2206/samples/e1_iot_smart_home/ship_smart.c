#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "los_task.h"
#include "ohos_init.h"
#include "cmsis_os.h"
#include "config_network.h"
#include "smart_home.h"
#include "smart_home_event.h"
#include "su_03t.h"
#include "iot.h"
#include "lcd.h"
#include "picture.h"
#include "adc_key.h"
#include "tds_sensor.h"
#include "drv_light.h"
#include "drv_sensors.h"
#include "ph_sensor.h"
#include "mux_selector.h"
#include "hmc5883l_sensor.h"
#include "iot_uart.h"
#include "ultrasonic_sensor.h"
#include "iot_gpio.h"
#include "los_tick.h"
#include "motor_control.h"
#include "water_pump.h"
#include "gps.h"

#define ULTRASONIC_TRIG_GPIO GPIO0_PA4
#define ULTRASONIC_ECHO_GPIO GPIO0_PA3

#define ROUTE_SSID      "wuwwu"
#define ROUTE_PASSWORD  "wsyzmlhh"

#define MSG_QUEUE_LENGTH 16
#define BUFFER_LEN 50

void smart_home_key_process(int key_no);  
void lcd_set_network_state(bool state);   

// ===== IoT线程：连接WiFi + 初始化MQTT客户端 =====
void iot_thread(void *args) {
    uint8_t mac_address[12] = {0x00, 0xdc, 0xb6, 0x90, 0x01, 0x00, 0};
    unsigned char ssid[32] = ROUTE_SSID;
    unsigned char password[32] = ROUTE_PASSWORD;

    FlashDeinit();
    FlashInit();

    VendorSet(VENDOR_ID_WIFI_MODE, (unsigned char *)"STA", 3);
    VendorSet(VENDOR_ID_MAC, mac_address, 6);
    VendorSet(VENDOR_ID_WIFI_ROUTE_SSID, ssid, sizeof(ssid));
    VendorSet(VENDOR_ID_WIFI_ROUTE_PASSWD, password, sizeof(password));

reconnect:
    SetWifiModeOff();
    int ret = SetWifiModeOn();
    if (ret != 0) {
        printf("WiFi连接失败，请检查配置或热点状态！\n");
        return;
    }

    mqtt_init(); // MQTT连接及注册回调

    while (1) {
        if (!wait_message()) {
            goto reconnect;
        }
        LOS_Msleep(1);
    }
}

// ===== 主线程：采集数据 + 上传数据 + 显示UI =====
void smart_home_thread(void *arg) {
    MuxGpioInit();
    PhSensorInit();
    TdsSensorInit();

    e_iot_data iot_data = {0};

    i2c_dev_init();
    lcd_dev_init();
    light_dev_init();
    lcd_show_ui();
    UltrasonicSensorInit();
    HMC5883L_Init();
    GPS_UART_Init();

    while (1) {
        event_info_t event_info = {0};
        int ret = smart_home_event_wait(&event_info, 3000);

        if (ret == LOS_OK) {
            switch (event_info.event) {
                case event_key_press:
                    smart_home_key_process(event_info.data.key_no);
                    break;
                case event_su03t:
                    smart_home_su03t_cmd_process(event_info.data.su03t_data);
                    break;
                default:
                    break;
            }
        }

        // 采集环境数据
        double temp, humi, lum;
        sht30_read_data(&temp, &humi);
        bh1750_read_data(&lum);

        float tds_value = TdsSensorRead();
        float ph_value = PhSensorRead();
        float turbidity_value = TurbiditySensorRead();
        float heading_angle = HMC5883L_ReadAngle();
        float ultrasonic_distance = UltrasonicSensorRead();

        lcd_set_illumination(lum);
        lcd_set_temperature(temp);
        lcd_set_humidity(humi);

        printf("罗盘方向角：%.2f 度\n", heading_angle);
        printf("超声波测距：%.2f cm\n", ultrasonic_distance);
        printf("开始读取GPS...\n");

        extern GPS_Coordinate g_current_gps;

        if (mqtt_is_connected()) {
            iot_data.illumination = lum;
            iot_data.temperature = temp;
            iot_data.humidity = humi;
            iot_data.light_state = get_light_state();
            iot_data.tds_value = tds_value;
            iot_data.ph_value = ph_value;
            iot_data.turbidity = turbidity;
            iot_data.ultrasonic = ultrasonic_distance;
            if (g_current_gps.valid) {
                iot_data.latitude = g_current_gps.latitude;
                iot_data.longitude = g_current_gps.longitude;
            }
            send_msg_to_mqtt(&iot_data);
            lcd_set_network_state(true);
        } else {
            lcd_set_network_state(false);
        }

        lcd_show_ui();
    }
}

// ===== 主函数：创建所有线程 =====
void iot_smart_home_example() {
    unsigned int ret;
    unsigned int thread_id;

    // 初始化事件系统（用于非控制类事件，如按键、串口数据）
    smart_home_event_init();

    // 1. 电机线程
    TSK_INIT_PARAM_S motorTask = {0};
    motorTask.pfnTaskEntry = (TSK_ENTRY_FUNC)motor_control_thread;
    motorTask.uwStackSize = 4096;
    motorTask.pcName = "motor_thread";
    motorTask.usTaskPrio = 23;
    ret = LOS_TaskCreate(&thread_id, &motorTask);
    if (ret != LOS_OK) {
        printf("电机线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("电机线程已启动\n");

    // 2. 水泵线程
    TSK_INIT_PARAM_S pumpTask = {0};
    pumpTask.pfnTaskEntry = (TSK_ENTRY_FUNC)water_pump_thread;
    pumpTask.uwStackSize = 4096;
    pumpTask.pcName = "water_pump";
    pumpTask.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &pumpTask);
    if (ret != LOS_OK) {
        printf("水泵线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("水泵线程已启动\n");

    // 3. 主线程（采集 + 显示 + 上报）
    TSK_INIT_PARAM_S smartTask = {0};
    smartTask.pfnTaskEntry = (TSK_ENTRY_FUNC)smart_home_thread;
    smartTask.uwStackSize = 6144;
    smartTask.pcName = "smart_home";
    smartTask.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &smartTask);
    if (ret != LOS_OK) {
        printf("智能主线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("智能主线程已启动\n");

    // 4. 按键线程
    TSK_INIT_PARAM_S keyTask = {0};
    keyTask.pfnTaskEntry = (TSK_ENTRY_FUNC)adc_key_thread;
    keyTask.uwStackSize = 6144;
    keyTask.pcName = "key_thread";
    keyTask.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &keyTask);
    if (ret != LOS_OK) {
        printf("按键线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("按键线程已启动\n");

    // 5. IoT通信线程
    TSK_INIT_PARAM_S iotTask = {0};
    iotTask.pfnTaskEntry = (TSK_ENTRY_FUNC)iot_thread;
    iotTask.uwStackSize = 20480;
    iotTask.pcName = "iot_thread";
    iotTask.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &iotTask);
    if (ret != LOS_OK) {
        printf("IoT线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("IoT线程已启动，系统启动成功！\n");

    // 6. GPS线程
    TSK_INIT_PARAM_S gpsTask = {0};
    gpsTask.pfnTaskEntry = (TSK_ENTRY_FUNC)GPS_Thread;
    gpsTask.uwStackSize = 6144;
    gpsTask.pcName = "gps_thread";
    gpsTask.usTaskPrio = 24;
    ret = LOS_TaskCreate(&thread_id, &gpsTask);
    if (ret != LOS_OK) {
        printf("GPS线程创建失败，ret: 0x%x\n", ret);
        return;
    }
    printf("GPS线程已启动\n");
}

APP_FEATURE_INIT(iot_smart_home_example);