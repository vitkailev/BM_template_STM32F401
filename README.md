# Bare-Metal template for STM32F401VC

Project template for STM32F401VC microcontroller.  
Hardware: [board](https://www.st.com/en/evaluation-tools/32f401cdiscovery.html)  
Library: [STM32 HAL](https://github.com/STMicroelectronics/STM32CubeF4)

## Features

- [x] Button;
- [x] 4 LEDs;
- [x] 2 analog input pins;
- [x] Build-in temperature sensor;
- [x] PWM;
- [x] UART interface;
- [x] I2C interface;
- [x] CRC-32;
- [x] Independent WDT;

## MCU Settings

1) Clock: HSI(16MHz) -> PLL -> SYSCLK(144MHz) -> HCLK(144MHz) -> PCLK1(36MHz) / PCLK2(36MHz);
2) Digital input pins: PA0 (pull-down);
3) Digital output pins: PD12/PD13/PD14/PD15 (pull-down);
4) SystemTick timer: 1kHz;
5) Measurement timer: Timer 11, 4kHz;
6) ADC: 12-bits, right, software start, 144 cycles, PB0/PA1 + temperature sensor;
7) PWM: Timer 4, 10kHz, channel 3, PB8;
8) UART: UART1 - PB6/PB7, 115200, 8N1, TX/RX, FIFO - disabled;
9) I2C: I2C3 - PA8/PC9, 100kHz, 7 bits, stretching;
10) CRC: input data - word/uint32_t, polynomial - 0x04C11DB7, init value - 0xFFFFFFFF, input inversion, output
    inversion;
11) WDT: 1 second, prescaler - 8, reload value = 0x0FFF;

## Project structure

| Folder name | Description                                             |
|:-----------:|:--------------------------------------------------------|
|     app     | Source code                                             |
|    core     | CMSIS and core ARM/Cortex header files                  |
|   lib/hal   | Hardware abstraction layer(HAL) source and header files |
|   startup   | Linker files                                            |
|   system    | System source and header files                          |

## Project settings

- CMakeLists.txt file
