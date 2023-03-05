#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "variables.h"

void changePinState(PortDef *port, bool state);

bool getPinState(const PortDef *port);

bool isTimerTriggered(const TimerDef *timer);

#endif //FUNCTIONS_H
