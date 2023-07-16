#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"

void changePinState(PortDef *port, bool state);

bool getPinState(const PortDef *port);

bool isButtonPressed(const ButtonDef *bt);

void checkButtonState(ButtonDef *bt);

bool isTimerTriggered(const TimerDef *timer);

void changeCounterValue(MCUDef *mcu);

int32_t getCounterValue(const MCUDef *mcu);

uint32_t getCRC(MCUDef *mcu, const void *data, uint16_t size);

#endif //FUNCTIONS_H
