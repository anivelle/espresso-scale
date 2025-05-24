cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

include(/usr/share/pico-sdk/pico_sdk_init.cmake)
project(espresso_scale)

pico_sdk_init()
add_executable(espresso_scale src/main.c)

target_link_libraries(espresso_scale 
                    pico_stdlib 
                    hardware_adc
                    hardware_pwm
                  )

# pico_set_printf_implementation(espresso_scale compiler)
pico_enable_stdio_usb(espresso_scale 1)
pico_enable_stdio_uart(espresso_scale 0)
