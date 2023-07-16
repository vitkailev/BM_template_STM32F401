#include "stm32f4xx_hal.h"

#include "functions.h"

void changePinState(PortDef *port, bool state) {
    HAL_GPIO_WritePin((GPIO_TypeDef *) port->obj, port->pin, state);
}

bool getPinState(const PortDef *port) {
    return HAL_GPIO_ReadPin((GPIO_TypeDef *) port->obj, port->pin);
}

bool isButtonPressed(const ButtonDef *bt) {
    return bt->isTriggered;
}

void checkButtonState(ButtonDef *bt) {
    if (getPinState(&bt->port))
        bt->delay++;
    else {
        bt->delay = 0;

        // uncomment, if you need to detect release event
//        bt->isTriggered = bt->state;

        bt->state = false;
    }

    uint16_t duration = 5; // 50 ms, main timer - 100 Hz
    if (bt->delay == duration) {
        bt->isTriggered = !bt->state;
        bt->state = true;
    }
}

bool isTimerTriggered(const TimerDef *timer) {
    return timer->isTriggered;
}

void changeCounterValue(MCUDef *mcu) {
    mcu->counter++;
    if (mcu->counter < 0)
        mcu->counter = 0;
}

int32_t getCounterValue(const MCUDef *mcu) {
    return mcu->counter;
}

uint32_t getCRC(MCUDef *mcu, const void *data, uint16_t size) {
    // CRC-32/MPEG-2
    return HAL_CRC_Calculate((CRC_HandleTypeDef *) mcu->crc, (uint32_t *) data, size);
}
