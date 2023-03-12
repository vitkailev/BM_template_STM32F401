#include <string.h>

#include "stm32f4xx_hal.h"

#include "main.h"
#include "settings.h"
#include "functions.h"

static void application(void);

static void startSettings(void);

static void error(uint32_t code);

int main(void) {
    int32_t err = 0;
    startSettings();
    if ((err = HAL_Init()))
        error(err);
    if ((err = initialization(&MCU, &Terminal)))
        error(err);
    if ((err = turnOnInterrupts(&MCU, &Terminal)))
        error(err);

    changePinState(&MCU.leds[LED_RED].port, true);
    while (true) {
        application();
    }
    return 1;
}

static void application(void) {
    if (isTimerTriggered(&MCU.timer)) {
        MCU.timer.isTriggered = false;

        changeCounterValue(&MCU);
    }

    updateUART(&Terminal, getCounterValue(&MCU));
    if (isUARTHaveData(&Terminal)) {
        Terminal.isHaveData = false;
    }
}

static void startSettings(void) {
    MCU.button.pin = GPIO_PIN_0;
    MCU.button.obj = GPIOA;
    MCU.leds[LED_GREEN].port.pin = GPIO_PIN_12;
    MCU.leds[LED_GREEN].port.obj = GPIOD;
    MCU.leds[LED_ORANGE].port.pin = GPIO_PIN_13;
    MCU.leds[LED_ORANGE].port.obj = GPIOD;
    MCU.leds[LED_RED].port.pin = GPIO_PIN_14;
    MCU.leds[LED_RED].port.obj = GPIOD;
    MCU.leds[LED_BLUE].port.pin = GPIO_PIN_15;
    MCU.leds[LED_BLUE].port.obj = GPIOD;
    MCU.timer.freq = 100; // Hz

    initUART(&Terminal, 115200);
}

static void error(uint32_t code) {
    while (true) {

    }
}
