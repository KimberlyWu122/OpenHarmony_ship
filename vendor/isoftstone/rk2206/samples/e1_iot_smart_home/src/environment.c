#include <stdio.h>
#include <string.h>
#include "environment.h"
#include "kv_store.h"

#if (LOGCFG_SHELL == 1)
#include "shcmd.h"
#include "shell.h"
#endif // LOGCFG_SHELL

//定义env环境变量的key
#define ENV_WIFI_SSID "wifi_ssid"
#define ENV_WIFI_PWD  "wifi_pwd"
#define ENV_WIFI_MAC  "wifi_mac"
#define ENV_IOT_DEVICEID "iot_devid"
#define ENV_IOT_USERNAME "iot_username"
#define ENV_IOT_PASSWORD "iot_pwd"
#define ENV_IOT_HOSTADDR "iot_host"

static char env_keys[][32]={
    ENV_WIFI_SSID,
    ENV_WIFI_PWD,
    ENV_WIFI_MAC,
    ENV_IOT_DEVICEID,
    ENV_IOT_USERNAME,
    ENV_IOT_PASSWORD,
    ENV_IOT_HOSTADDR

};

static evn_keys_num = sizeof(env_keys)/sizeof(env_keys[0]);

int env_shell_init(void){
    osCmdReg(CMD_TYPE_STD, "setenv", 0, (CMD_CBK_FUNC)shell_setEnv);
    osCmdReg(CMD_TYPE_STD, "getenv", 0, (CMD_CBK_FUNC)shell_getEnv);

    return 0;
}
int env_get(env_type_t type,char *value,int value_len){
    if(value == NULL){
        return -1;
    }

    int ret = 0;
    switch(type){
        case env_wifi_ssid:
             ret = UtilsGetValue(ENV_WIFI_SSID, value,value_len-1);
            break;
        case env_wifi_pwd:
            ret =  UtilsGetValue(ENV_WIFI_PWD, value,value_len-1);
            break;
        case env_wifi_mac:
            ret = UtilsGetValue(ENV_WIFI_MAC, value,value_len-1);
            break;
        case env_iot_deviceid:
            ret =  UtilsGetValue(ENV_IOT_DEVICEID, value,value_len-1);
        break;
        case env_iot_username:
            ret = UtilsGetValue(ENV_IOT_USERNAME, value,value_len-1);
            break;
        case env_iot_password:
            ret = UtilsGetValue(ENV_IOT_PASSWORD, value,value_len-1);
            break;
        case env_iot_hostaddr:
            ret = UtilsGetValue(ENV_IOT_HOSTADDR, value,value_len-1);
            break;
        default:
            return -1;
    }  

    return ret;
}

int shell_getEnv(int argc, const char *argv[])
{
    if(argc != 2){
        printf("getenv usage: getenv <key> \n");
        printf("\tkeys list: \n");
        printf("\t -all : get all keys and values\n");
        printf("\t "ENV_WIFI_SSID"  : get wifi sta wifi ssid\n");
        printf("\t "ENV_WIFI_PWD"   : get wifi password\n");
        printf("\t "ENV_WIFI_MAC"   : get wifi mac addr\n");
        printf("\t "ENV_IOT_DEVICEID"  : get iot mqtt device id\n");
        printf("\t "ENV_IOT_PASSWORD"    : get iot mqtt device password\n");
        printf("\t "ENV_IOT_USERNAME"  : get iot mqtt device username\n");
        printf("\t "ENV_IOT_HOSTADDR"  : get iot mqtt host addr\n");

        return -1;
    }

    char buffer[64]={0};
    int find = 0;
    for(int i=0;i<evn_keys_num;i++){
        memset(buffer,0,sizeof(buffer));
        /* 轮询查找对应的字段*/
        if((strcmp(argv[1],"-all") == 0)||(strcmp(argv[1],env_keys[i]) == 0) ){
            if (UtilsGetValue(env_keys[i], buffer,sizeof(buffer)-1) < 0){
                printf("get %s failed! \n",env_keys[i]);
               
            }else{
                
                printf("get %s success!,vaule:%s\n",env_keys[i],buffer);
            }
            find = 1 ;
            
        }
    }

    if(find == 0){
        printf("error: unrecognized parameter!\n");
        return -1;
    }

    return 0;
}

int shell_setEnv(int argc, const char *argv[])
{
    if(argc != 3){
        printf("setenv usage: setenv <key> <value> \n");
        printf("\t -cls <key>: clear env value by key,the key can be all \n");
        printf("\tdemo: setenv wifi-ssid abcd :set the wifi ssid to abcd\n");
        printf("\t support keys list: \n");
        printf("\t all  : only for -cls , clear the all values has been set\n");
        printf("\t "ENV_WIFI_SSID"  : set wifi sta ssid\n");
        printf("\t "ENV_WIFI_PWD"   : set wifi sta password\n");
        printf("\t "ENV_WIFI_MAC"   : set wifi mac addr\n");
        printf("\t "ENV_IOT_DEVICEID"  : set iot mqtt device id\n");
        printf("\t "ENV_IOT_PASSWORD"    : set iot mqtt device password\n");
        printf("\t "ENV_IOT_USERNAME"  : set iot mqtt device username\n");
        printf("\t "ENV_IOT_HOSTADDR"  : set iot mqtt host addr\n");
        return -1;
    }
    int find = 0;
    if(strcmp(argv[1],"-cls") == 0 ){
        printf("clear env value by key:%s\n",argv[2]);
            for(int i=0;i<evn_keys_num;i++){
                if((strcmp(argv[2],"all") == 0 ) || 
                    (strcmp(argv[2],env_keys[i]) == 0)){
                    if (UtilsDeleteValue(env_keys[i]) !=0){
                        printf("clear %s failed! \n",env_keys[i]);
                        
                    }else{

                        printf("clear %s success!\n",env_keys[i]);
                    }
                    find = 1;
                }
            }
            if(find == 0){
                printf("error: unrecognized parameter!\n");
                return -1;
            }
        
        return 0;
    }
    
    printf("set %s to %s\n",argv[1],argv[2]);
    find =0;
    for(int i=0;i<evn_keys_num;i++){
        
        if(strcmp(argv[1],env_keys[i]) == 0 ){

            if(strcmp(argv[1],ENV_WIFI_MAC) == 0){
                uint8_t mac[6]={0};
                int ret =sscanf(argv[2],"%02x:%02x:%02x:%02x:%02x:%02x",
                &mac[0],&mac[1],&mac[2],&mac[3],&mac[4],&mac[5]);
    
                /* sscanf函数返回匹配到的数量*/
                if(ret != 6){
                    printf("error: mac addr format incorrectness!\n");
                    return -1;
                }
                
            }

            if (UtilsSetValue(env_keys[i], argv[2]) !=0){
                printf("set %s failed! \n",env_keys[i]);
                return -1;
            }
            printf("set %s success!\n",env_keys[i]);
            find = 1;
            break;

        }
    }

    if(find == 0){
        printf("error: unrecognized parameter!\n");
        return -1;
    }else{
        printf("setenv success! please reboot device to take effect\n");
    }

    return 0;
}
