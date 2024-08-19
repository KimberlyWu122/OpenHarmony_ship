#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

typedef enum env_type{
    env_wifi_ssid,
    env_wifi_pwd,
    env_wifi_mac,
    env_iot_deviceid,
    env_iot_username,
    env_iot_password,
    env_iot_hostaddr,
}env_type_t;

int env_get(env_type_t type,char *value,int value_len);
int shell_getEnv(int argc, const char *argv[]);
int shell_setEnv(int argc, const char *argv[]);

#endif