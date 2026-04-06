#ifndef VARIABLES_H
#define VARIABLES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "uart.h"
#include "i2c.h"

enum Constants {
    LED_GREEN = 0,
    LED_ORANGE,
    LED_RED,
    LED_BLUE,
    NUMBER_LEDS,

    BUTTON_1 = 0,
    NUMBER_BUTTONS,

    ANALOG_IN_1 = 0,
    ANALOG_IN_2,
    ANALOG_TEMP_VREF,
    NUMBER_ADC_CHANNELS,
};

typedef struct {
    volatile bool isSysTickTriggered;

    bool isWDTTriggered;
} FlagsDef;

typedef struct {
    bool isTriggered;
    bool state;

    uint16_t delay;
    uint16_t duration;

    uint16_t pin;
    void *handle;
} PortDef;

typedef struct {
    volatile bool isTriggered;

    uint16_t basePrescaler; // initialization value - "const"
    uint16_t currentPrescaler; // mutable value - via setTimerPrescaler function
    uint32_t freq; // Hz

    void *handle;
} TimerDef;

typedef struct {
    volatile bool isProcessing;
    volatile bool isFinished;
    volatile uint32_t errType;
    volatile uint32_t errors;

    uint8_t idx;
    uint16_t rawValues[NUMBER_ADC_CHANNELS]; // relative values
    uint16_t value[NUMBER_ADC_CHANNELS]; // mV

    // build-in
    int16_t temp; // C

    void *handle;
} AdcDef;

typedef struct {
    void *crc;
    void *wdt;
} HandlesDef;

typedef struct {
    FlagsDef flags;

    PortDef leds[NUMBER_LEDS];
    PortDef button[NUMBER_BUTTONS];

    HandlesDef handles;
    TimerDef measTimer;
    AdcDef adc;
    TimerDef pwmTimer;
    UARTDef uart;
    I2CDef i2c;
} McuDef;

extern McuDef Mcu;

#ifdef __cplusplus
}
#endif

#endif //VARIABLES_H
