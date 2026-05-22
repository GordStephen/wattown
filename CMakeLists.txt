cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(wattown)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

add_executable(wattown
    wattown.c
)

pico_sdk_init()
pico_enable_stdio_usb(wattown 1)
pico_enable_stdio_uart(wattown 0)
pico_generate_pio_header(wattown ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_link_libraries(wattown pico_stdlib hardware_pio hardware_i2c)
pico_add_extra_outputs(wattown)
