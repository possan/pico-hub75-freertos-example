cmake_minimum_required(VERSION 3.5)

if (NOT WIFI_SSID OR NOT WIFI_PASSWORD)
    message(FATAL_ERROR "WIFI_SSID and WIFI_PASSWORD needs to be defined")
endif()

set(ENV{FREERTOS_KERNEL_PATH} "${CMAKE_SOURCE_DIR}/lib/FreeRTOS-Kernel")

include(pico_sdk_import.cmake)

include($ENV{FREERTOS_KERNEL_PATH}/portable/ThirdParty/GCC/RP2040/FreeRTOS_Kernel_import.cmake)

project(sign)

set (CMAKE_CXX_STANDARD 20)

pico_sdk_init()

add_executable(sign)

target_sources(sign PRIVATE
    src/demo.c
    src/leds.c
    src/main.c
    src/wifi.c
)

target_compile_definitions(sign PRIVATE
    "WIFI_SSID=\"${WIFI_SSID}\""
    "WIFI_PASSWORD=\"${WIFI_PASSWORD}\""
)

add_compile_options(-mfloat-abi=softfp)
include_directories(sign src)

target_link_libraries(sign LINK_PUBLIC
    FreeRTOS-Kernel-Heap4
    hardware_adc
    hardware_flash
    hardware_i2c
    hardware_pio
    pico_cyw43_arch_lwip_sys_freertos
    pico_multicore
    pico_stdlib
)

pico_enable_stdio_usb(sign 1)
pico_enable_stdio_uart(sign 0)

pico_add_extra_outputs(sign)
