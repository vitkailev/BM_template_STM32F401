#include <string.h>

#include "stm32f4xx_hal.h"

#include "main.h"
#include "settings.h"
#include "functions.h"

int main(void) {
    int32_t err = 0;
    startSettings();
    if ((err = HAL_Init()))
        error(err);
    if ((err = initialization(&MCU, &Terminal, &I2C3Bus)))
        error(err);
    if ((err = turnOnInterrupts(&MCU, &Terminal)))
        error(err);

    changePinState(&MCU.leds[LED_RED].port, true);
    while (true) {
        application();
    }
    return 1;
}

void application(void) {
    if (isTimerTriggered(&MCU.timer)) {
        MCU.timer.isTriggered = false;

        changeCounterValue(&MCU);
        checkButtonState(&MCU.button);

        // Usual time page write - 5 ms
        updateI2CBus(&I2C3Bus);
    }

    if (isButtonPressed(&MCU.button)) {
        MCU.button.isTriggered = false;
    }

    updateUART(&Terminal, getCounterValue(&MCU));
    if (isUARTHaveData(&Terminal)) {
        Terminal.isHaveData = false;

        writeUARTData(&Terminal, Terminal.rxBuffer, Terminal.rxSize);
    }
}

void startSettings(void) {
    MCU.button.port.pin = GPIO_PIN_0;
    MCU.button.port.obj = GPIOA;
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

void error(int32_t errCode) {
    while (true) {

    }
}
