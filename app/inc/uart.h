#ifndef UART_H
#define UART_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum UART_Constants {
    UART_RX_BUFFER_SIZE = 512,
    UART_TX_QUEUE_SIZE = 1024
};

typedef struct {
    bool isInit;
    bool isHaveData;
    bool isWriting;
    volatile bool isReading;

    uint32_t speed;
    uint32_t error;

    uint8_t rxByte;
    volatile int32_t counter;
    volatile uint16_t rxSize;
    uint8_t rxBuffer[UART_RX_BUFFER_SIZE];

    uint8_t *volatile txHead;
    uint8_t *txTail;
    volatile uint16_t nSent;
    uint8_t txQueue[UART_TX_QUEUE_SIZE];

    void *obj;
} UARTDef;

int initUART(UARTDef *uart, uint32_t speed);

bool isUARTHaveData(const UARTDef *uart);

bool isUARTWriting(const UARTDef *uart);

int writeUARTData(UARTDef *uart, const void *data, uint16_t size);

void updateUART(UARTDef *uart, int32_t counter);

void saveUARTByte(UARTDef *uart, int32_t counter);

void sendUARTData(UARTDef *uart);

#ifdef __cplusplus
}
#endif

#endif //UART_H
