#include <string.h>

#include "stm32f4xx_hal.h"

#include "i2c.h"

bool isI2CBusy(const I2CDef *i2c) {
    return i2c->isBusy;
}

void takeI2CBus(I2CDef *i2c) {
    i2c->isBusy = true;
}

void freeI2CBus(I2CDef *i2c) {
    i2c->isBusy = false;
}

bool isI2CReading(const I2CDef *i2c) {
    return i2c->isReading;
}

int readI2CData(I2CDef *i2c, uint8_t devAdd, uint32_t memAdd, uint8_t addSize, void *data, uint16_t size) {
    if (!isI2CBusy(i2c))
        return 1;
    if (isI2CReading(i2c) || isI2CWriting(i2c))
        return 2;
    if (data == NULL || size == 0 || size > I2C_BUFFER_SIZE)
        return 3;

    i2c->isReading = true;
    i2c->deviceAdd = devAdd;
    i2c->memoryAdd = memAdd;
    i2c->rxData = data;
    i2c->rxSize = size;
    switch (addSize) {
        case 1:
            i2c->buffer[0] = i2c->memoryAdd;
            break;
        case 2:
            i2c->buffer[0] = i2c->memoryAdd;
            i2c->buffer[1] = i2c->memoryAdd >> 8;
            break;
        case 4:
            i2c->buffer[0] = i2c->memoryAdd;
            i2c->buffer[1] = i2c->memoryAdd >> 8;
            i2c->buffer[2] = i2c->memoryAdd >> 16;
            i2c->buffer[3] = i2c->memoryAdd >> 24;
            break;
        default:
            break;
    }

    //MCU specific code
    HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef *) i2c->obj, i2c->deviceAdd, i2c->buffer, addSize);
    return 0;
}

bool isI2CWriting(const I2CDef *i2c) {
    return i2c->isWriting;
}

int writeI2CData(I2CDef *i2c, uint8_t devAdd, uint32_t memAdd, uint8_t addSize, const void *data, uint16_t size) {
    if (!isI2CBusy(i2c))
        return 1;
    if (isI2CReading(i2c) || isI2CWriting(i2c))
        return 2;
    if (data == NULL || size == 0 || size > I2C_BUFFER_SIZE)
        return 3;

    i2c->isWriting = true;
    i2c->deviceAdd = devAdd;
    i2c->memoryAdd = memAdd;
    i2c->txData = data;
    i2c->txSize = size;
    switch (addSize) {
        case 1:
            i2c->buffer[0] = i2c->memoryAdd;
            break;
        case 2:
            i2c->buffer[0] = i2c->memoryAdd;
            i2c->buffer[1] = i2c->memoryAdd >> 8;
            break;
        case 4:
            i2c->buffer[0] = i2c->memoryAdd;
            i2c->buffer[1] = i2c->memoryAdd >> 8;
            i2c->buffer[2] = i2c->memoryAdd >> 16;
            i2c->buffer[3] = i2c->memoryAdd >> 24;
            break;
        default:
            break;
    }

    if (size > I2C_PAGE_SIZE)
        size = I2C_PAGE_SIZE;
    memcpy((void *) (i2c->buffer + addSize), i2c->txData, size);
    i2c->txSize -= size;
    i2c->txData = (uint8_t *) i2c->txData + size;

    //MCU specific code
    HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef *) i2c->obj, i2c->deviceAdd, i2c->buffer, addSize + size);
    return 0;
}

void updateI2CBus(I2CDef *i2c) {
    if (!isI2CBusy(i2c))
        return;
    if (isI2CReading(i2c) || isI2CWriting(i2c))
        return;

    uint16_t size = 0;
    //MCU specific code
    if (i2c->rxSize) {
        i2c->isReading = true;

        size = (i2c->rxSize > I2C_PAGE_SIZE) ? I2C_PAGE_SIZE : i2c->rxSize;
        memcpy(i2c->rxData, (const void *) i2c->buffer, size);
        i2c->rxSize -= size;
        if (i2c->rxSize)
            HAL_I2C_Master_Receive_IT((I2C_HandleTypeDef *) i2c->obj, i2c->deviceAdd, i2c->buffer, size);
    }
    //MCU specific code
    if (i2c->txSize) {
        i2c->isWriting = true;

        const void *data = i2c->txData;
        size = (i2c->txSize > I2C_PAGE_SIZE) ? I2C_PAGE_SIZE : i2c->txSize;
        i2c->txSize -= size;
        i2c->txData = (uint8_t *) i2c->txData + size;
        HAL_I2C_Master_Transmit_IT((I2C_HandleTypeDef *) i2c->obj, i2c->deviceAdd, (uint8_t *) data, size);
    }
}
