cmake_minimum_required(VERSION 3.0.0)

project(knx-pt100)

enable_language(C CXX ASM)


include_directories(
    knx-stack/src
    mcu-core/Core/Inc
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Inc
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    mcu-core/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    mcu-core/Drivers/CMSIS/Include
)

file(GLOB KNX_STACK
    knx-stack/src/knx/address_table_object.cpp
    knx-stack/src/knx/apdu.cpp
    knx-stack/src/knx/application_layer.cpp
    knx-stack/src/knx/application_program_object.cpp
    knx-stack/src/knx/association_table_object.cpp
    knx-stack/src/knx/bau07B0.cpp
    knx-stack/src/knx/bau_systemB.cpp
    knx-stack/src/knx/bau.cpp
    knx-stack/src/knx/bits.cpp
    knx-stack/src/knx/cemi_frame.cpp
    knx-stack/src/knx/data_link_layer.cpp
    knx-stack/src/knx/datapoint_types.cpp
    knx-stack/src/knx/device_object.cpp
    knx-stack/src/knx/dpt.cpp
    knx-stack/src/knx/dptconvert.cpp
    knx-stack/src/knx/group_object.cpp
    knx-stack/src/knx/group_object_table_object.cpp
    knx-stack/src/knx/interface_object.cpp
    knx-stack/src/knx/ip_data_link_layer.cpp
    knx-stack/src/knx/ip_parameter_object.cpp
    knx-stack/src/knx/knx_value.cpp
    knx-stack/src/knx/memory.cpp
    knx-stack/src/knx/network_layer.cpp
    knx-stack/src/knx/npdu.cpp
    knx-stack/src/knx/platform.cpp
    knx-stack/src/knx/table_object.cpp
    knx-stack/src/knx/tpdu.cpp
    knx-stack/src/knx/tpuart_data_link_layer.cpp
    knx-stack/src/knx/transport_layer.cpp
    knx-stack/src/knx_facade.cpp
    knx-stack/src/stm32_platform.cpp
    knx-stack/src/ring_buffer.cpp
)
file(GLOB SRCS
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ramfunc.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_exti.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
    mcu-core/Core/Src/stm32l4xx_hal_msp.c
    mcu-core/Core/Src/stm32l4xx_hal_timebase_tim.c
    mcu-core/Core/Src/stm32l4xx_it.c
    mcu-core/Core/Src/system_stm32l4xx.c
    mcu-core/startup_stm32l432xx.s
    newstdlib.c
    knx-pt100.cpp
)

add_definitions("-DUSE_HAL_DRIVER")
add_definitions("-DSTM32L432xx")
add_definitions("-DARCH_STM32")
add_definitions("-Wno-unknown-pragmas") # To ignore pragma regions in KNX stack code
add_definitions("-Wno-switch")
add_definitions("-Wno-comment")

set(EXECTABLE_NAME ${CMAKE_PROJECT_NAME}.elf)
add_executable(${CMAKE_PROJECT_NAME}.elf ${SRCS} ${KNX_STACK})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex   ${CMAKE_PROJECT_NAME}.elf  ${CMAKE_PROJECT_NAME}.hex)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf  ${CMAKE_PROJECT_NAME}.bin)

# display the size of the output binary after a build is finished
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND arm-none-eabi-size --format=berkeley "${CMAKE_PROJECT_NAME}.elf")

install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/install.sh knx-pt100.bin)")
