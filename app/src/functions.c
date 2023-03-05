#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"

#include "functions.h"

void changePinState(PortDef *port, bool state) {
    HAL_GPIO_WritePin((GPIO_TypeDef *) port->obj, port->pin, state);
}

bool getPinState(const PortDef *port) {
    return HAL_GPIO_ReadPin((GPIO_TypeDef *) port->obj, port->pin);
}

bool isTimerTriggered(const TimerDef *timer) {
    return timer->isTriggered;
}


