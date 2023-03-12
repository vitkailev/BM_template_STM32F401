#ifndef UART_H
#define UART_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum UART_Constants {
    UART_RX_BUFFER_SIZE = 512,
    UART_TX_BUFFER_SIZE = 128,
    UART_TX_QUEUE_SIZE = 1024
};

typedef struct {
    bool isInit;
    bool isHaveData;
    bool isWriting;

    uint32_t speed;
    uint32_t error;

    uint8_t *volatile txHead;
    uint8_t *txTail;

    uint8_t rxByte;
    volatile bool isReading;
    volatile int32_t counter;
    volatile uint16_t rxSize;
    uint8_t rxBuffer[UART_RX_BUFFER_SIZE];

    uint16_t txSize;
    uint8_t txBuffer[UART_TX_BUFFER_SIZE];

    void *obj;
} UARTDef;

extern UARTDef Terminal;

int initUART(UARTDef *uart, uint32_t speed);

bool isUARTHaveData(const UARTDef *uart);

bool isUARTWriting(const UARTDef *uart);

int writeUARTData(UARTDef *uart, const void *data, uint16_t size);

void updateUART(UARTDef *uart, int32_t counter);

void saveUARTByte(UARTDef *uart, int32_t counter);

void sendUARTByte(UARTDef *uart);

#endif //UART_H
