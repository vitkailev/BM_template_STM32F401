#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

enum I2C_Constants {
    I2C_BUFFER_SIZE = 64
};

enum I2C_Errors {
    I2C_SUCCESS = 0,
    I2C_NOT_INIT = -1,
    I2C_BUSY = -2,
    I2C_WRONG_DATA = -3,
    I2C_HW_ERROR = -4,

    I2C_NUMBER_ERRORS = 4
};

typedef struct {
    bool isInit;
    volatile bool isReading;
    volatile bool isWriting;
    volatile uint32_t errType;
    volatile uint32_t errors;

    uint16_t deviceAdd;

    uint16_t size;
    uint8_t buffer[I2C_BUFFER_SIZE];

    void *handle;
} I2CDef;

int I2C_init(I2CDef *i2c);

bool I2C_isReading(const I2CDef *i2c);

int I2C_readData(I2CDef *i2c, uint16_t devAdd, uint16_t size);

const uint8_t *I2C_getData(const I2CDef *i2c);

uint16_t I2C_getDataSize(const I2CDef *i2c);

bool I2C_isWriting(const I2CDef *i2c);

int I2C_writeData(I2CDef *i2c, uint16_t devAdd, const void *data, uint16_t size, bool isNeedStop);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */
