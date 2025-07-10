#ifndef GPS_H
#define GPS_H

typedef struct {
    float latitude;
    float longitude;
    bool valid;
} GPS_Coordinate;

extern GPS_Coordinate g_current_gps;

void GPS_UART_Init(void);
void ParseGNRMC(char *line, float *latitude, float *longitude);
float convert_to_decimal(const char *nmea_coord, const char *direction);
void *GPS_Thread(const char *arg);  // 如果你用线程方式

#endif
