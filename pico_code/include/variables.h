#include "hardware/i2c.h"

// ADC values
#define AVERAGE_LEN 750
#define SLEEP_TIME_US 100

// GPIO values
#define ADC_PIN 26
#define PWM_PIN 27
#define SDA_PIN 18
#define SCL_PIN 19

// LCD variables
#define LCD_ADDR 0x7C >> 1
#define I2C_INST i2c1_inst
#define RGB_ADDR 0xC0 >> 1
#define LCD_FUNCTIONSET 0x20
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
