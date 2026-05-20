cmake_minimum_required(VERSION 3.13...3.27)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(wattown)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/generated)

add_executable(wattown
    wattown.c
)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()
pico_enable_stdio_usb(wattown 1)
pico_enable_stdio_uart(wattown 0)
pico_generate_pio_header(wattown ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(wattown pico_stdlib hardware_pio)
pico_add_extra_outputs(wattown)

add_executable(pio_ws2812)
pico_generate_pio_header(pio_ws2812 ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/generated)

target_sources(pio_ws2812 PRIVATE ws2812.c)
target_link_libraries(pio_ws2812 PRIVATE pico_stdlib hardware_pio)
pico_add_extra_outputs(pio_ws2812)
