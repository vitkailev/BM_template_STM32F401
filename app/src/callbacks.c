#include "stm32f4xx_hal.h"

#include "uart.h"
#include "i2c.h"

#include "functions.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == ((TIM_HandleTypeDef *) MCU.timer.obj)->Instance) {
        MCU.timer.isTriggered = true;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == ((UART_HandleTypeDef *) Terminal.obj)->Instance) {
        sendUARTByte(&Terminal);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == ((UART_HandleTypeDef *) Terminal.obj)->Instance) {
        saveUARTByte(&Terminal, getCounterValue(&MCU));
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == ((UART_HandleTypeDef *) Terminal.obj)->Instance) {
        Terminal.error++;
    }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) I2CBus1.obj)->Instance) {
        I2CBus1.isWriting = false;
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) I2CBus1.obj)->Instance) {
        I2CBus1.isReading = false;
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == ((I2C_HandleTypeDef *) I2CBus1.obj)->Instance) {
        I2CBus1.error++;
    }
}
