set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_EXE_LINKER arm-none-eabi-g++)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)


unset(CMAKE_C_FLAGS CACHE)
unset(CMAKE_ASM_FLAGS CACHE)
unset(CMAKE_CXX_FLAGS CACHE)
unset(CMAKE_EXE_LINKER_FLAGS CACHE)

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
