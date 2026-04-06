#include "stm32f4xx_hal.h"

#include "functions.h"

/**
 * @brief Set the state/level of the output pin
 * @param port is the base GPIO port data structure
 * @param state is a pin state (True - HIGH or False - LOW)
 */
void changePinState(PortDef *port, bool state) {
    port->state = state;
    HAL_GPIO_WritePin((GPIO_TypeDef *) port->handle, port->pin, state);
}

/**
 * @brief Check if the input pin has changed state (detect this event)
 * @param port is the base GPIO port data structure
 * @return True - a pin changed state from LOW to HIGH, otherwise - False
 */
bool isPinTriggered(const PortDef *port) {
    return port->isTriggered;
}

/**
 * @brief Get/read the current state of the input pin
 * @param port is the base GPIO port data structure
 * @return True - a pin is HIGH, otherwise - LOW
 */
bool getPinState(const PortDef *port) {
    return port->state;
}

/**
 * @brief Update the GPIO pin data structure state
 * @param port is the base GPIO port data structure
 */
void checkPinState(PortDef *port) {
    bool isSet = HAL_GPIO_ReadPin((GPIO_TypeDef *) port->handle, port->pin);
    if (isSet) {
        port->delay++;
    } else {
        port->delay = 0;

        // if you need to control the moment of release
        //        port->isTriggered = port->state;

        port->state = false;
    }

    if (port->delay == port->duration) {
        port->isTriggered = !port->state;
        port->state = true;
    }
}

/**
 * @brief Set the duty value of the PWM signal
 * @param tim is the base timer data structure
 * @param channel is the timer output channel (TIM_CHANNEL_x (x=1..6) or TIM_CHANNEL_ALL)
 * @param value is the duty value (from 0 to 100)
 */
void setPWMDutyCycle(TimerDef *tim, uint16_t channel, uint8_t value) {
    value = (value > 100) ? 100 : value;
    __HAL_TIM_SET_COMPARE((TIM_HandleTypeDef *) tim->handle, channel, value);
}

/**
 * @brief Check if the ADC module has finished conversion process (all pins/channels)
 * @param adc is the base ADC data structure
 * @return True - conversion completed, otherwise - False
 */
bool isADCFinished(const AdcDef *adc) {
    return adc->isFinished;
}

/**
 * @brief Start/run analog-to-digital conversion process
 * @param adc is the base ADC data structure
 * @return HAL_OK - successfully, otherwise - error code
 */
int readAnalogValues(AdcDef *adc) {
    if (adc->isProcessing)
        return HAL_BUSY;

    adc->isProcessing = true;
    adc->errType = 0;
    adc->idx = 0;
    return HAL_ADC_Start_IT((ADC_HandleTypeDef *) adc->handle);
}

/**
 * @brief Convert relative ADC values to the "real" values (mV and degrees)
 * @param adc is the base ADC data structure
 * @return HAL_OK - ADC values have been converted, otherwise - HAL_BUSY
 */
int convertADCResults(AdcDef *adc) {
    if (adc->isProcessing)
        return HAL_BUSY;

    int32_t value = 0;
    const uint32_t resolution = ADC_GET_RESOLUTION((ADC_HandleTypeDef *)adc->handle);
    // reference manual, 11.12.2 ADC_CR1, page 230
    const uint8_t bits[] = {12, 10, 8, 6};
    if (resolution >= sizeof(bits))
        return HAL_ERROR;

    for (size_t i = 0; i < NUMBER_ADC_CHANNELS; ++i) {
        value = adc->rawValues[i];
        switch (i) {
            case ANALOG_TEMP_VREF:
                // HAL and LL resolution values are the same, so I do not need to use LL_ADC_RESOLUTION_xxB
                adc->temp = (int16_t) __LL_ADC_CALC_TEMPERATURE(VDD_VALUE, value, resolution);
                value = 0;
                break;
            default:
                value *= VDD_VALUE;
                value >>= bits[resolution]; // 0.732mV error is negligible for this case
                break;
        }

        adc->value[i] = (uint16_t) value;
    }

    return HAL_OK;
}

/**
 * @brief Get the MCU Unique ID
 * @return pointer to a unique id value
 */
const void *getUniqueID(void) {
    // RM0368 Reference manual, 24.1 Unique device ID, page 841
    return (const void *) 0x1FFF7A10;
}

/**
 * @brief Calculate CRC-32
 * @param data is target data
 * @param size is target data size (in words - uint32_t)
 * @return if successfully - CRC value, otherwise - 0
 */
uint32_t getCRC(McuDef *mcu, const void *data, uint16_t size) {
    // CRC-32/MPEG-2
    return HAL_CRC_Calculate((CRC_HandleTypeDef *) mcu->handles.crc, (uint32_t *) data, size);
}
