# knx-pt100

KNX device with STM32 L432KC cpu, TP-UART2 and PT100 thermal sensor

This project use KNX stack as a Git submodule

## Clone the repository

In order to clone this repository, you must use --recursive option:
```h
git clone --recursive https://github.com/condo4/knx-pt100.git
```

## Build and Flash the firmware on the Nucleo Board
You need arm-none-eabi-gcc and st-flash:
```sh
apt install gcc-arm-none-eabi binutils-arm-none-eabi stlink-tools
```

In order to build the firmware, you need cmake.
```sh
cd firmware
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../src/arm-none-eabi-gcc_toolchain.cmake ../src/
make
```

To flash the device, plug the Nucleo USB on your pc and run

```sh
make install
```
