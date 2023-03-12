#ifndef VARIABLES_H
#define VARIABLES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum LED_Types {
    LED_GREEN,
    LED_ORANGE,
    LED_RED,
    LED_BLUE,
    NUMBER_LEDS
};

typedef struct {
    uint16_t pin;
    void *obj;
} PortDef;

typedef struct {
    bool state;
    uint8_t mode;
    PortDef port;
} LEDDef;

typedef struct {
    bool isTriggered;
    uint16_t freq;
    void *obj;
} TimerDef;

typedef struct {
    int32_t counter;

    PortDef button;
    LEDDef leds[NUMBER_LEDS];
    TimerDef timer;
    void *crc;
} MCUDef;

extern MCUDef MCU;

#endif //VARIABLES_H
