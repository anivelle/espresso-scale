#include <stdio.h>
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

#define AVERAGE_LEN 30

int main() {
  stdio_init_all();
  adc_init();
  // printf("ADC measurement!\n");
  adc_gpio_init(26);

  adc_select_input(0);
  adc_fifo_setup(true, false, 2, true, false);
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  gpio_put(PICO_DEFAULT_LED_PIN, true);

  while (!stdio_usb_connected()) {}
  gpio_put(PICO_DEFAULT_LED_PIN, false);
  sleep_ms(1000);
  adc_run(true);
  bool led_on = true;

  uint32_t prev_avg = 0;
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

  while(1) {
    led_on = !led_on;
    result = adc_fifo_get();
    uint16_t last = window[index];
    
    prev_avg += (result - last) / AVERAGE_LEN;
    window[index] = result;
    index = (index + 1) % AVERAGE_LEN;
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    if (!(result & (1 << 15)))
      printf("%d\n", prev_avg);
    sleep_ms(20);
  }
  return 0;
}
