#
# Project:      lef-pico
#
# Desription:   Demo of LEF running on Raspberry Pi Pico
#    
# Author:       Peter Malmberg <peter.malmberg@gmail.com>
# Date:         
# License:      MIT
# Organisation: ACME
# 
# CPU:  
#
#

cmake_minimum_required(VERSION 3.20)

# Generate compile_commands.json for use in VSCode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Import Pico SDK (must be BEFORE project)
if (DEFINED ENV{PICO_SDK_PATH})
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
else()
    message(FATAL_ERROR "PICO_SDK_PATH not set")
endif()

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Main project
project(lef-pico 
    VERSION 1.0
    DESCRIPTION "Demo of LEF running on Raspberry Pi Pico"
    LANGUAGES C CXX ASM
)

# Set target board
set(PICO_BOARD pico)  # alt. pico, pico_w, pico2, pico2_w

# Initialize SDK (must be BEFORE using its functions)
pico_sdk_init()

add_executable(lef-pico 
    src/main.c
    LEF/LEF.c
    LEF/LEF_Core.c
    LEF/LEF_Queue.c
    LEF/LEF_Timer.c
    LEF/LEF_Button.c
    LEF/LEF_Led.c
    LEF/LEF_Cli.c
    LEF/LEF_Cli_History.c
)

target_include_directories(lef-pico
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/LEF
)

target_link_libraries(lef-pico 
    pico_stdlib
#    pico_multicore
#    pico_bootrom
#    pico_sync
#    pico_unique_id
#    pico_sha256
#    hardware_gpio
#    hardware_pio
#    hardware_adc
#    hardware_i2c
#    hardware_uart
#    hardware_spi    
#    hardware_clocks
#    hardware_pwm
#    hardware_rtc
#    hardware_flash
#    hardware_dma
#    hardware_sync
#    hardware_irq
#    hardware_resets
#    hardware_interp
#    boot_uf2_headers
#    tinyusb_host
#    tinyusb_device
#    tinyusb_board
)

# These MUST come AFTER pico_sdk_init()
pico_enable_stdio_usb(lef-pico 1)
pico_enable_stdio_uart(lef-pico 0)

pico_add_extra_outputs(lef-pico)


# 
# Custom targets for programming etc.
#

# ---- Tools ----
set(PICOTOOL picotool)
set(OPENOCD openocd)

# ---- OpenOCD config ----
set(OPENOCD_INTERFACE interface/cmsis-dap.cfg)   
set(OPENOCD_TARGET target/rp2040.cfg)

set(UF2_FILE ${CMAKE_CURRENT_BINARY_DIR}/lef-pico.uf2)
set(ELF_FILE ${CMAKE_CURRENT_BINARY_DIR}/lef-pico.elf)


add_custom_target(flash
    DEPENDS lef-pico
    COMMAND picotool load -f ${UF2_FILE}
    COMMAND picotool reboot
    COMMENT "Flashing ${UF2_FILE}"
)

add_custom_target(flash_swd
    DEPENDS lef-pico
    COMMAND ${OPENOCD}
        -f ${OPENOCD_INTERFACE}
        -f ${OPENOCD_TARGET}
        -c "adapter speed 5000"
        -c "program ${ELF_FILE} verify reset exit"
    COMMENT "Flashing via OpenOCD (SWD)"
)

add_custom_target(flash_usb
    DEPENDS lef-pico
    COMMAND ${CMAKE_COMMAND} -E copy
        ${UF2_FILE}
        /media/$ENV{USER}/RPI-RP2
    COMMENT "Flashing direct via USB" 
)

add_custom_target(reboot
    COMMAND picotool reboot
    COMMENT "Rebooting Pico via USB"
)

add_custom_target(debug_server
    COMMAND ${OPENOCD}
        -f ${OPENOCD_INTERFACE}
        -f ${OPENOCD_TARGET}
    COMMENT "Starting OpenOCD GDB server"
)


add_custom_target(debug
    DEPENDS lef-pico
    COMMAND arm-none-eabi-gdb ${ELF_FILE}
        -ex "target remote localhost:3333"
        -ex "monitor reset init"
    COMMENT "Starting GDB and connecting to OpenOCD"
)


add_custom_target(ihelp
    COMMAND ${CMAKE_COMMAND} -E echo "flash        - Flash via picotool"
    COMMAND ${CMAKE_COMMAND} -E echo "flash_usb    - Flash via USB drive"
    COMMAND ${CMAKE_COMMAND} -E echo "flash_swd    - Flash via debugprobe/swd and openocd"
    COMMAND ${CMAKE_COMMAND} -E echo "reboot       - Reboot target"
    COMMAND ${CMAKE_COMMAND} -E echo "debug_server - Starting OpenOCD GDB server"
    COMMAND ${CMAKE_COMMAND} -E echo "debug        - Starting GDB and connecting to OpenOCD"
)

# Add printout of RAM and flash memory usage at end of compilation
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    # COMMAND arm-none-eabi-size $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND arm-none-eabi-size --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Memory usage:"
)