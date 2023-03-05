#include "stm32f4xx_hal.h"

#include "main.h"
#include "settings.h"
#include "functions.h"

static void application(void);

static void startSettings(void);

static void error(uint32_t code);

static uint32_t counter;

int main(void) {
    uint32_t err = 0;
    startSettings();
    if ((err = HAL_Init()))
        error(err);
    if ((err = initialization(&MCU)))
        error(err);
    if ((err = turnOnInterrupts(&MCU)))
        error(err);

    while (true) {
        application();
    }
    return 1;
}

static void application(void) {
    if (isTimerTriggered(&MCU.timer)) {
        MCU.timer.isTriggered = false;

        counter++;
        if (counter % (MCU.timer.freq / 2) == 0) {
            MCU.leds[LED_GREEN].state ^= true;
            changePinState(&MCU.leds[LED_GREEN].port, MCU.leds[LED_GREEN].state);
        }
        if (counter % (MCU.timer.freq / 4) == 0) {
            MCU.leds[LED_ORANGE].state ^= true;
            changePinState(&MCU.leds[LED_ORANGE].port, MCU.leds[LED_ORANGE].state);
        }
        if (counter == MCU.timer.freq) {
            counter = 0;

            MCU.leds[LED_BLUE].state ^= true;
            changePinState(&MCU.leds[LED_BLUE].port, MCU.leds[LED_BLUE].state);
        }
    }

    changePinState(&MCU.leds[LED_RED].port, getPinState(&MCU.button));
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
}

static void error(uint32_t code) {
    while (true) {

    }
}
