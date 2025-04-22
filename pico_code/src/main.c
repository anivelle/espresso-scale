#include <stdio.h>
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"

int main() {
  stdio_init_all();
  adc_init();
  while (!stdio_usb_connected()) {}
  printf("ADC measurement!\n");
  adc_gpio_init(26);

  while(1) {

    uint16_t result = adc_read();
    printf("%d\n", result);
    sleep_ms(10);
  }
  return 0;
}
