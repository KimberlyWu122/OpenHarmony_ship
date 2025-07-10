#include <stdio.h>
#include <stdlib.h>

#include "MQTTClient.h"
#include "cJSON.h"
#include "cmsis_os2.h"
#include "config_network.h"
#include "iot.h"
#include "los_task.h"
#include "ohos_init.h"
#include "smart_home_event.h"
#include "MQTTLiteOS.h"
#include "los_queue.h"
#include <string.h>
#include "motor_control.h"
#include "water_pump.h"


extern MQTTClient client;

// 指令宏（与 motor/pump 保持一致）
#define IOT_CMD_FORWARD   1
#define IOT_CMD_LEFT      2
#define IOT_CMD_BACKWARD  3
#define IOT_CMD_RIGHT     4
#define IOT_CMD_STOP      5
#define IOT_CMD_PUMP      6
#define IOT_CMD_AUTO_CIRCLE 7



// 来自设备连接信息文件
#define DEVICE_ID       "68480c9e32771f177b40af50_tds_1_0_0_2025061906"
#define MQTT_DEVICES_PWD "6a16623364d8851ef2aff557cc7038b3f772a2df2ee839aa32b2970143395284"
#define HOST_ADDR       "13f9a955a4.st1.iotda-device.cn-north-4.myhuaweicloud.com"
#define PORT            8883
#define USERNAME "68480c9e32771f177b40af50_tds_1"

#define PUBLISH_TOPIC "$oc/devices/" USERNAME "/sys/properties/report"
#define SUBCRIB_TOPIC "$oc/devices/" USERNAME "/sys/messages/down"
#define RESPONSE_TOPIC "$oc/devices/" USERNAME "/sys/messages/down/response"


#define MAX_BUFFER_LENGTH 512
#define MAX_STRING_LENGTH 64

static unsigned char sendBuf[MAX_BUFFER_LENGTH];
static unsigned char readBuf[MAX_BUFFER_LENGTH];

Network network;
MQTTClient client;

static char mqtt_devid[64]=DEVICE_ID;
static char mqtt_pwd[72]=MQTT_DEVICES_PWD;
static char mqtt_username[64]=USERNAME;
static char mqtt_hostaddr[64]=HOST_ADDR;

static char publish_topic[128] = PUBLISH_TOPIC;
static char subcribe_topic[128] = SUBCRIB_TOPIC;
static char response_topic[128] = RESPONSE_TOPIC;

static unsigned int mqttConnectFlag = 0;

extern bool motor_state;
extern bool light_state;
extern bool auto_state;

/***************************************************************
* 函数名称: send_msg_to_mqtt
* 说    明: 发送信息到iot
* 参    数: e_iot_data *iot_data：数据
* 返 回 值: 无
***************************************************************/
void send_msg_to_mqtt(e_iot_data *iot_data) {
  int rc;
  MQTTMessage message;
  char payload[MAX_BUFFER_LENGTH] = {0};
  char str[MAX_STRING_LENGTH] = {0};

  if (mqttConnectFlag == 0) {
    printf("mqtt not connect\n");
    return;
  }
  
  cJSON *root = cJSON_CreateObject();
  if (root != NULL) {
    cJSON *serv_arr = cJSON_AddArrayToObject(root, "services");
    cJSON *arr_item = cJSON_CreateObject();
    cJSON_AddStringToObject(arr_item, "service_id", "sensors");
    cJSON *pro_obj = cJSON_CreateObject();
    cJSON_AddItemToObject(arr_item, "properties", pro_obj);

    memset(str, 0, MAX_BUFFER_LENGTH);
    // 光照强度
    sprintf(str, "%5.2fLux", iot_data->illumination);
    cJSON_AddNumberToObject(pro_obj, "illumination", iot_data->illumination);
    // 温度
    sprintf(str, "%5.2f℃", iot_data->temperature);
    cJSON_AddNumberToObject(pro_obj, "temperature", iot_data->temperature);
    // 湿度
    sprintf(str, "%5.2f%%", iot_data->humidity);
    cJSON_AddNumberToObject(pro_obj, "humidity", iot_data->humidity);
    // TDS
    sprintf(str, "%5.2fppm", iot_data->tds_value);
    cJSON_AddNumberToObject(pro_obj, "tds", iot_data->tds_value);
    // ph
    sprintf(str, "%5.2fpH", iot_data->ph_value);  
    cJSON_AddNumberToObject(pro_obj, "ph", iot_data->ph_value);  
    cJSON_AddNumberToObject(pro_obj, "turbidity", iot_data->turbidity);


    sprintf(str, "%5.2fm", iot_data->ultrasonic); 
    cJSON_AddNumberToObject(pro_obj, "ultrasonic", iot_data->ultrasonic);

    cJSON_AddNumberToObject(pro_obj, "latitude", iot_data->latitude);

    cJSON_AddNumberToObject(pro_obj, "longitude", iot_data->longitude);

    // 电机状态
    if (iot_data->motor_state == true) {
      cJSON_AddStringToObject(pro_obj, "motorStatus", "ON");
    } else {
      cJSON_AddStringToObject(pro_obj, "motorStatus", "OFF");
    }
    // 灯光状态
    if (iot_data->light_state == true) {
      cJSON_AddStringToObject(pro_obj, "lightStatus", "ON");
    } else {
      cJSON_AddStringToObject(pro_obj, "lightStatus", "OFF");
    }
    // 自动状态模式
    if (iot_data->auto_state == true) {
      cJSON_AddStringToObject(pro_obj, "autoStatus", "ON");
    } else {
      cJSON_AddStringToObject(pro_obj, "autoStatus", "OFF");
    }

    cJSON_AddItemToArray(serv_arr, arr_item);

    char *palyload_str = cJSON_PrintUnformatted(root);
    strcpy(payload, palyload_str);

    cJSON_free(palyload_str);
    cJSON_Delete(root);
  }

  message.qos = 0;
  message.retained = 0;
  message.payload = payload;
  message.payloadlen = strlen(payload);

  sprintf(publish_topic,"$oc/devices/%s/sys/properties/report",mqtt_devid);
  if ((rc = MQTTPublish(&client, publish_topic, &message)) != 0) {
    printf("Return code from MQTT publish is %d\n", rc);
    mqttConnectFlag = 0;
  } else {
    printf("mqtt publish success:%s\n", payload);
  }
}

/***************************************************************
* 函数名称: set_light_state
* 说    明: 设置灯状态
* 参    数: cJSON *root
* 返 回 值: 无
***************************************************************/
void set_light_state(cJSON *root) {
  cJSON *para_obj = NULL;
  cJSON *status_obj = NULL;
  char *value = NULL;

  event_info_t event={0};
  event.event=event_iot_cmd;

  para_obj = cJSON_GetObjectItem(root, "paras");
  status_obj = cJSON_GetObjectItem(para_obj, "onoff");
  if (status_obj != NULL) {
    value = cJSON_GetStringValue(status_obj);
    if (!strcmp(value, "ON")) {
      event.data.iot_data = IOT_CMD_LIGHT_ON;
      // light_state = true;
    } else if (!strcmp(value, "OFF")) {
      event.data.iot_data = IOT_CMD_LIGHT_OFF;
      // light_state = false;
    }
    smart_home_event_send(&event);
  }
}

/***************************************************************
* 函数名称: set_motor_state
* 说    明: 设置电机状态
* 参    数: cJSON *root
* 返 回 值: 无
***************************************************************/
void set_motor_state(cJSON *root) {
  cJSON *para_obj = NULL;
  cJSON *status_obj = NULL;
  char *value = NULL;

  event_info_t event={0};
  event.event=event_iot_cmd;

  para_obj = cJSON_GetObjectItem(root, "paras");
  status_obj = cJSON_GetObjectItem(para_obj, "onoff");
  if (status_obj != NULL) {
    value = cJSON_GetStringValue(status_obj);
    if (!strcmp(value, "ON")) {
      // motor_state = true;
      event.data.iot_data = IOT_CMD_MOTOR_ON;
    } else if (!strcmp(value, "OFF")) {
      // motor_state = false;
      event.data.iot_data = IOT_CMD_MOTOR_OFF;
    }
    smart_home_event_send(&event);
  }
}

/***************************************************************
* 函数名称: set_light_state
* 说    明: 设置自动模式状态
* 参    数: cJSON *root
* 返 回 值: 无
***************************************************************/
void set_auto_state(cJSON *root) {
  cJSON *para_obj = NULL;
  cJSON *status_obj = NULL;
  char *value = NULL;

  para_obj = cJSON_GetObjectItem(root, "paras");
  status_obj = cJSON_GetObjectItem(para_obj, "onoff");
  if (status_obj != NULL) {
    value = cJSON_GetStringValue(status_obj);
    if (!strcmp(value, "ON")) {
      // auto_state = true;
    } else if (!strcmp(value, "OFF")) {
      // auto_state = false;
    }
  }
}

/***************************************************************
* 函数名称: mqtt_message_arrived
* 说    明: 接收mqtt数据
* 参    数: MessageData *data
* 返 回 值: 无
***************************************************************/
void mqtt_message_arrived(MessageData *data) {
    int rc;
    MQTTMessage message;
    char payload[MAX_BUFFER_LENGTH] = {0};
    char rsptopic[128] = {0};

    printf("MQTT Message arrived on topic %.*s: %.*s\n",
           data->topicName->lenstring.len, data->topicName->lenstring.data,
           data->message->payloadlen, (char *)data->message->payload);

    message.qos = 0;
    message.retained = 0;
    message.payload = payload;
    sprintf(payload, "{ \
        \"result_code\": 0, \
        \"response_name\": \"COMMAND_RESPONSE\", \
        \"paras\": { \
            \"result\": \"success\" \
        } \
    }");
    message.payloadlen = strlen(payload);
    sprintf(rsptopic, "%s", response_topic);

    rc = MQTTPublish(&client, rsptopic, &message);
    if (rc != 0) {
        printf("MQTT Publish response failed: %d\n", rc);
        mqttConnectFlag = 0;
    } else {
        printf("已向响应主题 %s 发布确认消息\n", rsptopic);
    }

    cJSON *root = cJSON_ParseWithLength(data->message->payload, data->message->payloadlen);
    if (root != NULL) {
        cJSON *content_obj = cJSON_GetObjectItem(root, "content");
        if (content_obj != NULL) {
            cJSON *control_obj = cJSON_GetObjectItem(content_obj, "contro");
            if (control_obj != NULL && cJSON_IsNumber(control_obj)) {
                UINT32 cmd_value = (UINT32)control_obj->valueint;
                printf("收到控制指令：contro = %u\n", cmd_value);
                printf("control_obj->valueint = %d\n", control_obj->valueint);
                printf("准备写入队列的值 = %u，大小 = %lu 字节\n", cmd_value, sizeof(cmd_value));

                if (cmd_value >= IOT_CMD_FORWARD && cmd_value <= IOT_CMD_AUTO_CIRCLE) {
                    printf("电机控制范围命中，准备写入 motor 队列，ID = 0x%x\n", g_motorQueueId);
                    UINT32 ret = LOS_QueueWriteCopy(g_motorQueueId, &cmd_value, sizeof(UINT32), 0);
                    if (ret == LOS_OK) {
                        printf("电机指令 %u 已写入 motor 队列\n", cmd_value);
                    } else {
                        printf("写入 motor 队列失败，错误码: 0x%x\n", ret);
                    }

                } else if (cmd_value == IOT_CMD_PUMP) {
                    printf("水泵采样指令命中，准备写入 pump 队列，ID = 0x%x\n", g_pumpQueueId);
                    UINT32 ret = LOS_QueueWriteCopy(g_pumpQueueId, &cmd_value, sizeof(UINT32), 0);
                    if (ret == LOS_OK) {
                        printf("水泵指令 %u 已写入 pump 队列\n", cmd_value);
                        printf("mqtt 中 g_pumpQueueId 地址: %p，值: 0x%x\n", &g_pumpQueueId, g_pumpQueueId);
                    } else {
                        printf("写入 pump 队列失败，错误码: 0x%x\n", ret);
                    }

                } else {
                    printf("未知控制指令: %u，已忽略处理\n", cmd_value);
                }

                // 自动巡航指令处理：解析 gps_list
                if (cmd_value == IOT_CMD_AUTO_CIRCLE) {
                    cJSON *gps_list = cJSON_GetObjectItem(content_obj, "gps_list");
                    if (gps_list != NULL && cJSON_IsArray(gps_list)) {
                        int count = cJSON_GetArraySize(gps_list);
                        waypointCount = (count > MAX_WAYPOINTS) ? MAX_WAYPOINTS : count;

                        for (int i = 0; i < waypointCount; ++i) {
                            cJSON *point = cJSON_GetArrayItem(gps_list, i);
                            if (point) {
                                cJSON *lng_obj = cJSON_GetObjectItem(point, "lng");
                                cJSON *lat_obj = cJSON_GetObjectItem(point, "lat");

                                if (lng_obj && lat_obj && cJSON_IsString(lng_obj) && cJSON_IsString(lat_obj)) {
                                    waypointList[i].lng = atof(lng_obj->valuestring);
                                    waypointList[i].lat = atof(lat_obj->valuestring);
                                    printf("航点 %d：经度 = %.6f，纬度 = %.6f\n", i + 1, waypointList[i].lng, waypointList[i].lat);
                                }
                            }
                        }

                    } else {
                        printf("gps_list 字段缺失或格式错误\n");
                    }
                }

            } else {
                printf("控制指令字段 'contro' 缺失或格式错误！\n");
            }
        } else {
            printf("消息中缺少 'content' 字段！\n");
        }
        cJSON_Delete(root);
    } else {
        printf("解析 MQTT 消息 JSON 失败！\n");
    }
}

/***************************************************************
* 函数名称: wait_message
* 说    明: 等待信息
* 参    数: 无
* 返 回 值: 无
***************************************************************/
int wait_message() {
  uint8_t rec = MQTTYield(&client, 5000);
  if (rec != 0) {
    mqttConnectFlag = 0;
  }
  if (mqttConnectFlag == 0) {
    return 0;
  }
  return 1;
}

/***************************************************************
* 函数名称: mqtt_init
* 说    明: mqtt初始化
* 参    数: 无
* 返 回 值: 无
***************************************************************/
void mqtt_init() {
  int rc;

  printf("Starting MQTT...\n");

  /*网络初始化*/
  NetworkInit(&network);

begin:
  /* 连接网络*/
  printf("NetworkConnect  ...\n");
  NetworkConnect(&network, HOST_ADDR, 1883);
  printf("MQTTClientInit  ...\n");
  /*MQTT客户端初始化*/
  MQTTClientInit(&client, &network, 2000, sendBuf, sizeof(sendBuf), readBuf,
                 sizeof(readBuf));

  MQTTString clientId = MQTTString_initializer;
  clientId.cstring = mqtt_devid;

  MQTTString userName = MQTTString_initializer;
  userName.cstring = mqtt_username;

  MQTTString password = MQTTString_initializer;
  password.cstring = mqtt_pwd;

  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
  data.clientID = clientId;
  data.username = userName;
  data.password = password;
  data.willFlag = 0;
  data.MQTTVersion = 4;
  data.keepAliveInterval = 60;
  data.cleansession = 1;

  printf("MQTTConnect  ...\n");
  rc = MQTTConnect(&client, &data);
  if (rc != 0) {
    printf("MQTTConnect: %d\n", rc);
    NetworkDisconnect(&network);
    MQTTDisconnect(&client);
    osDelay(200);
    goto begin;
  }

  printf("MQTTSubscribe  ...\n");
  //sprintf(subcribe_topic,"$oc/devices/%s/sys/commands/+",mqtt_devid);
  rc = MQTTSubscribe(&client, subcribe_topic, 0, mqtt_message_arrived);
  if (rc != 0) {
    printf("MQTTSubscribe: %d\n", rc);
    osDelay(200);
    goto begin;
  }

  mqttConnectFlag = 1;
}

/***************************************************************
* 函数名称: mqtt_is_connected
* 说    明: mqtt连接状态
* 参    数: 无
* 返 回 值: unsigned int 状态
***************************************************************/
unsigned int mqtt_is_connected() { return mqttConnectFlag; }
