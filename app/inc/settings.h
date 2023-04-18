#ifndef SETTINGS_H
#define SETTINGS_H

#include "uart.h"
#include "i2c.h"

#include "variables.h"

int initialization(MCUDef *mcu, UARTDef *uart, I2CDef *i2c);

int turnOnInterrupts(MCUDef *mcu, UARTDef *uart);

#endif //SETTINGS_H
