#ifndef __SMART_HOME_H__
#define __SMART_HOME_H__

#include <stdint.h>
#include <stdbool.h>

void i2c_dev_init(void);
void bh1750_read_data(double *dat);
void sht30_read_data(double *dat);

void light_dev_init(void);
void light_set_pwm(unsigned int duty);
void light_set_state(bool state);

void motor_dev_init(void);
void motor_set_pwm(unsigned int duty);
void motor_set_state(bool state);

void lcd_dev_init(void);
void lcd_load_ui(void);
void lcd_set_temperature(double temperature);
void lcd_set_humidity(double humidity);
void lcd_set_lllumination(double lllumination);
void lcd_set_light_state(bool state);
void lcd_set_motor_state(bool state);
void lcd_set_auto_state(bool state);

#endif
