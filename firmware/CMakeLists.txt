# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

cmake_minimum_required(VERSION 3.25)

# Select board:
# set(PICO_BOARD pico_w)
set(PICO_BOARD pico_w CACHE STRING "Board type")

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_SDK_PATH}/tools/CMakeLists.txt)

project(main C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_definitions(SYS_CLK_MHZ=200)


add_executable(${PROJECT_NAME}
    main.c
    DMA/dma.c
    LED/led.c
    Communication/Wire/wire_communication.c
    Communication/WiFi/wifi.c
    Communication/WiFi/dhcpserver.c

    PWM_timer/timer.c
    PIO/analyze.c
    ADC/ADS1115.c
    ADC/Pico_ADC.c
    ADC/ADC_bootStrap.c

    Ring_buffer/ring_buffer.c
    Multicore_fifo/multicore_fifo.c
    Core1_bootstrap/core1.c
)

include_directories(${PROJECT_NAME}
    .
    DMA
    LED
    Communication
    Communication/Wire
    Communication/WiFi

    PWM_timer
    PIO
    ADC


    Ring_buffer
    Multicore_fifo
    Core1_bootstrap
)

pico_generate_pio_header(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/PIO/read.pio
)

pico_add_extra_outputs(${PROJECT_NAME})


target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    pico_multicore

    hardware_gpio
    hardware_pwm
    hardware_pio
    hardware_clocks
    hardware_dma
    hardware_irq
    hardware_i2c
    hardware_adc
    pico_util
    pico_cyw43_arch_lwip_threadsafe_background
)

pico_enable_stdio_usb(${PROJECT_NAME}  1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)