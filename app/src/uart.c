#include <stdlib.h>

#include "stm32f4xx_hal.h"

#include "uart.h"

UARTDef Terminal;
static uint8_t txQueue[UART_TX_QUEUE_SIZE];

static int32_t getCounter(const UARTDef *uart) {
    return uart->counter;
}

static bool isReading(const UARTDef *uart) {
    return uart->isReading;
}

static bool isQueueEmpty(const UARTDef *uart) {
    return (uart->txTail == uart->txHead);
}

static bool isQueueFull(const UARTDef *uart) {
    if (uart->txHead == txQueue && uart->txTail == txQueue + UART_TX_QUEUE_SIZE)
        return true;
    else
        return (uart->txHead == uart->txTail + 1);
}

static bool isInitialized(const UARTDef *uart) {
    return uart->isInit;
}

int initUART(UARTDef *uart, uint32_t speed) {
    uart->speed = speed;
    uart->txHead = txQueue;
    uart->txTail = txQueue;
    uart->isInit = true;
    return 0;
}

bool isUARTHaveData(const UARTDef *uart) {
    return uart->isHaveData;
}

bool isUARTWriting(const UARTDef *uart) {
    return uart->isWriting;
}

int writeUARTData(UARTDef *uart, const void *data, uint16_t size) {
    if (!isInitialized(uart))
        return -1;
    if (data == NULL || size == 0)
        return -2;

    uint16_t i = 0;
    for (i = 0; i < size && !isQueueFull(uart); ++i) {
        *(uart->txTail++) = *((uint8_t *) data + i);

        if (uart->txTail == txQueue + UART_TX_QUEUE_SIZE && uart->txHead != txQueue)
            uart->txTail = txQueue;
    }
    return (size - i);
}

void updateUART(UARTDef *uart, int32_t counter) {
    if (!isInitialized(uart))
        return;

    uint8_t dt = 2; // pause after last received byte - 20ms (the main timer - 100Hz)
    if (isReading(uart) && abs(counter - getCounter(uart)) >= dt) {
        uart->isReading = false;
        uart->isHaveData = true;
    }

    if (isQueueEmpty(uart))
        uart->isWriting = false;
    else if (!isUARTWriting(uart)) {
        uart->isWriting = true;

        // MCU specific code
        HAL_UART_Transmit_IT((UART_HandleTypeDef *) uart->obj, uart->txHead, 1);
    }
}

void saveUARTByte(UARTDef *uart, int32_t counter) {
    if (!isReading(uart))
        uart->rxSize = 0;

    uart->isReading = true;
    uart->counter = counter;
    *(uart->rxBuffer + uart->rxSize++) = uart->rxByte;
    if (uart->rxSize == UART_RX_BUFFER_SIZE)
        uart->rxSize = 0;

    // MCU specific code
    HAL_UART_Receive_IT((UART_HandleTypeDef *) uart->obj, &uart->rxByte, 1);
}

void sendUARTByte(UARTDef *uart) {
    uart->txHead++;
    if (uart->txHead == txQueue + UART_TX_QUEUE_SIZE)
        uart->txHead = txQueue;
    if (isQueueEmpty(uart))
        return;

    // MCU specific code
    HAL_UART_Transmit_IT((UART_HandleTypeDef *) uart->obj, uart->txHead, 1);
}
