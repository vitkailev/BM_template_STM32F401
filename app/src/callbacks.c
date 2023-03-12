#include "stm32f4xx_hal.h"

#include "uart.h"

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
