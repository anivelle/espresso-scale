#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define AVERAGE_LEN 100
#define ADC_PIN 26
#define PWM_PIN 27
#define SDA_PIN 18
#define SCL_PIN 19
#define LCD_ADDR 0x7C >> 1
#define I2C_INST i2c1_inst
#define RGB_ADDR 0xC0 >> 1
#define LCD_FUNCTIONSET 0x20
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08

int set_reg(uint8_t addr, uint8_t data) {
    uint8_t data_send[2] = {addr, data};
    return i2c_write_blocking(&I2C_INST, RGB_ADDR, data_send, 2, 0);
}

void setRGB(uint8_t red, uint8_t green, uint8_t blue) {
    set_reg(0x4, red);
    set_reg(0x3, green);
    set_reg(0x2, blue);
}

void write_lcd(char *string) {
    char *cur;
    uint8_t buffer_write[2] = {0x40, 0};
    uint8_t return_home[2] = {0x80, 0x80};
    cur = string;
    while (*cur != 0) {
        buffer_write[1] = *cur;
        printf("%c", *cur);
        i2c_write_blocking(&I2C_INST, LCD_ADDR, buffer_write, 2, 0);
        // sleep_ms(1);
        // *cur = 0;
        cur++;
    }
    printf("\n");
    // Return the cursor to home
    i2c_write_blocking(&I2C_INST, LCD_ADDR, return_home, 2, 0);
}

void tare() {
  // TODO
}

int calibrate() {
  //TODO 
  return 1;
}
int main() {
    stdio_init_all();
    adc_init();
    i2c_init(&I2C_INST, 400 * 1000);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    // printf("ADC measurement!\n");
    adc_gpio_init(ADC_PIN);

    adc_select_input(0);
    adc_fifo_setup(true, false, 2, true, false);
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_LED_PIN, true);

    while (!stdio_usb_connected()) {
    }
    gpio_put(PICO_DEFAULT_LED_PIN, false);
    sleep_ms(1000);
    adc_run(true);
    bool led_on = true;

    printf("Connected!\n");
    uint8_t buffer_command[2] = {0x80, 0x20 | 0x08};
    for (int i = 0; i < 3; i++) {
        if (i2c_write_blocking(&I2C_INST, LCD_ADDR, buffer_command, 2, 0) ==
            PICO_ERROR_GENERIC)
            printf("Could not access the LCD\n");
        printf("Initiated LCD\n");
        sleep_ms(5);
    }

    // Set the display on with blinking cursor
    buffer_command[1] = LCD_DISPLAYCONTROL | 0x07;
    if (i2c_write_blocking(&I2C_INST, LCD_ADDR, buffer_command, 2, 0) ==
        PICO_ERROR_GENERIC)
        printf("Could not access the LCD\n");

    // Clear the screen
    buffer_command[1] = 0x01;
    if (i2c_write_blocking(&I2C_INST, LCD_ADDR, buffer_command, 2, 0) ==
        PICO_ERROR_GENERIC)
        printf("Could not access the LCD\n");
    sleep_ms(2000);

    // Set left entry mode with decrementing entry shift
    buffer_command[1] = 0x04 | 0x02;
    if (i2c_write_blocking(&I2C_INST, LCD_ADDR, buffer_command, 2, 0) ==
        PICO_ERROR_GENERIC)
        printf("Could not access the LCD\n");

    set_reg(0, 0);
    set_reg(0x8, 0xFF);
    set_reg(1, 0x2);

    setRGB(75, 175, 255);
    float prev_avg = 0;
    uint8_t index = 0;
    uint16_t window[AVERAGE_LEN];
    uint16_t result;

    // Moving average window
    for (int i = 0; i < AVERAGE_LEN; i++) {
        result = adc_fifo_get();
        prev_avg += result;
        window[index] = result;
        index = (index + 1) % AVERAGE_LEN;
        sleep_ms(10);
    }
    prev_avg /= AVERAGE_LEN;

    char value[5] = {0};
    char *cur;
    uint8_t buffer_write[5] = {0x40, 0};
    buffer_command[1] = 0x80;
    while (1) {

        result = adc_fifo_get();
        uint16_t last = window[index];

        prev_avg += (float)(result - last) / AVERAGE_LEN;
        window[index] = result;
        index = (index + 1) % AVERAGE_LEN;
        if (!(result & (1 << 15))) {
            // printf("%d\n", (int)prev_avg);
            sprintf(value, "%d", (int)prev_avg);
            // Write out the value to the LCD
            write_lcd(value);
            sleep_us(100);
        }
    }
    return 0;
}
