#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "variables.h"

void changePinState(PortDef *port, bool state);

bool isPinTriggered(const PortDef *port);

bool getPinState(const PortDef *port);

void checkPinState(PortDef * port);

void setPWMDutyCycle(TimerDef *tim, uint16_t channel, uint8_t value);

bool isADCFinished(const AdcDef *adc);

int readAnalogValues(AdcDef * adc);

int convertADCResults(AdcDef * adc);

const void *getUniqueID(void);

uint32_t getCRC(McuDef *mcu, const void *data, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif //FUNCTIONS_H