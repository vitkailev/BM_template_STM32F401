#ifndef SETTINGS_H
#define SETTINGS_H

#include "uart.h"

#include "variables.h"

int initialization(MCUDef *mcu, UARTDef *uart);

int turnOnInterrupts(MCUDef *mcu, UARTDef *uart);

#endif //SETTINGS_H
