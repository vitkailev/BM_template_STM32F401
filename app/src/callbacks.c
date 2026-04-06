#include "stm32f4xx_hal.h"

#include "functions.h"

/**
 * @brief A timer interrupt callback function
 * @param htim is the timer handle structure (HAL)
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == ((TIM_HandleTypeDef *) Mcu.measTimer.handle)->Instance) {
        Mcu.measTimer.isTriggered = true;
    }
}

/**
 * @brief ADC interrupt callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ((ADC_HandleTypeDef *) Mcu.adc.handle)->Instance) {
        uint32_t value = HAL_ADC_GetValue((ADC_HandleTypeDef *) Mcu.adc.handle);

        if (Mcu.adc.idx >= NUMBER_ADC_CHANNELS)
            Mcu.adc.idx = 0;

        Mcu.adc.rawValues[Mcu.adc.idx++] = (uint16_t) value;

        if (Mcu.adc.idx == NUMBER_ADC_CHANNELS) {
            Mcu.adc.isProcessing = false;
            Mcu.adc.isFinished = true;
        } else {
            HAL_ADC_Start_IT((ADC_HandleTypeDef *) Mcu.adc.handle);
        }
    }
}

/**
 * @brief ADC interrupt error callback function
 * @param hadc is the ADC handle structure (HAL)
 */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ((ADC_HandleTypeDef *) Mcu.adc.handle)->Instance) {
        Mcu.adc.isProcessing = false;
        Mcu.adc.errType = HAL_ADC_GetError((ADC_HandleTypeDef *) Mcu.adc.handle);
        Mcu.adc.errors++;
    }
}

/**
 * @brief UART data transfer callback function
 * @param huart is the UART handle structure (HAL)
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == ((UART_HandleTypeDef *) Mcu.uart.handle)->Instance) {
        Mcu.uart.isWriting = false;
        Mcu.uart.head += Mcu.uart.nSent;
    }
}

/**
 * @brief UART data receive callback function
 * @param huart is the UART handle structure (HAL)
 * @param Size an amount of data which has been received (bytes)
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    if (huart->Instance == ((UART_HandleTypeDef *) Mcu.uart.handle)->Instance) {
        Mcu.uart.time = HAL_GetTick();

        if (!Mcu.uart.isReading) {
            Mcu.uart.isReading = true;
            Mcu.uart.size = 0;
        }

        Mcu.uart.size += Size;
        uint16_t freeSpace = sizeof(Mcu.uart.buffer) - Mcu.uart.size;
        HAL_UARTEx_ReceiveToIdle_IT((UART_HandleTypeDef *) Mcu.uart.handle, Mcu.uart.buffer + Mcu.uart.size,
                                    freeSpace);
    }
}

/**
 * @brief UART errors callback function
 * @param huart is the UART handle structure (HAL)
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == ((UART_HandleTypeDef *) Mcu.uart.handle)->Instance) {
        Mcu.uart.isWriting = false;
        Mcu.uart.isReading = false;
        Mcu.uart.errType = HAL_UART_GetError((UART_HandleTypeDef *) Mcu.uart.handle);
        Mcu.uart.errors++;
    }
}

/**
 * @brief I2C data transfer callback function
 * @param hi2c is the I2C handle structure (HAL)
 */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) Mcu.i2c.handle)->Instance) {
        Mcu.i2c.isWriting = false;
    }
}

/**
 * @brief I2C data receive callback function
 * @param hi2c is the I2C handle structure (HAL)
 */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) Mcu.i2c.handle)->Instance) {
        Mcu.i2c.isReading = false;
    }
}

/**
 * @brief I2C errors callback function
 * @param hi2c is the I2C handle structure (HAL)
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) Mcu.i2c.handle)->Instance) {
        Mcu.i2c.isWriting = false;
        Mcu.i2c.isReading = false;
        Mcu.i2c.errType = HAL_I2C_GetError((I2C_HandleTypeDef *) Mcu.i2c.handle);
        Mcu.i2c.errors++;
    }
}
