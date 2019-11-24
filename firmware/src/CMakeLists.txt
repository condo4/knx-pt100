cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_EXE_LINKER arm-none-eabi-g++)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CPU "-mcpu=cortex-m4")
set(FPU "-mfpu=fpv4-sp-d16")
set(FLOAT_ABI "-mfloat-abi=hard")
set(MCU "${CPU} -mthumb ${FPU} ${FLOAT_ABI}")
set(LDSCRIPT "${CMAKE_SOURCE_DIR}/mcu-core/STM32L432KCUx_FLASH.ld")
set(COMMON_FLAGS "${MCU} ${C_DEFS} -Og -Wall -fdata-sections -ffunction-sections -g -gdwarf-2 -MMD")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS}  -std=gnu++0x"  CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS "${COMMON_FLAGS}"  CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${MCU} -specs=nosys.specs -T${LDSCRIPT} -lc -lm -lnosys -Wl,--gc-sections  "  CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${CMAKE_C_FLAGS} "  CACHE STRING "" FORCE)

project(knx-pt100)

enable_language(C CXX ASM)

include(knx-stack/knx.cmake)

include_directories(
    mcu-core/Core/Inc
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Inc
    mcu-core/Drivers/STM32L4xx_HAL_Driver/Inc/Legacy
    mcu-core/Drivers/CMSIS/Device/ST/STM32L4xx/Include
    mcu-core/Drivers/CMSIS/Include
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


set(EXECTABLE_NAME ${CMAKE_PROJECT_NAME}.elf)
add_executable(${CMAKE_PROJECT_NAME}.elf ${SRCS} ${KNX_STACK})

add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Oihex   ${CMAKE_PROJECT_NAME}.elf  ${CMAKE_PROJECT_NAME}.hex)
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} -Obinary ${CMAKE_PROJECT_NAME}.elf  ${CMAKE_PROJECT_NAME}.bin)

# display the size of the output binary after a build is finished
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND arm-none-eabi-size --format=berkeley "${CMAKE_PROJECT_NAME}.elf")

install(CODE "execute_process(COMMAND ${CMAKE_SOURCE_DIR}/install.sh knx-pt100.bin)")
