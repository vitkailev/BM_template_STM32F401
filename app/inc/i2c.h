#ifndef I2C_H
#define I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum I2C_Constants {
    I2C_PAGE_SIZE = 128, // in EEPROM
    I2C_ADDRESS_SIZE = 4,
    I2C_BUFFER_SIZE = I2C_PAGE_SIZE + I2C_ADDRESS_SIZE,
};

typedef struct {
    bool isBusy;
    bool isReading;
    bool isWriting;

    uint8_t deviceAdd;
    uint32_t memoryAdd;

    uint16_t rxSize;
    uint16_t txSize;
    void *rxData;
    const void *txData;
    uint8_t buffer[I2C_BUFFER_SIZE];

    uint32_t error;

    void *obj;
} I2CDef;

extern I2CDef I2CBus1;

bool isI2CBusy(const I2CDef *i2c);

void takeI2CBus(I2CDef *i2c);

void freeI2CBus(I2CDef *i2c);

bool isI2CReading(const I2CDef *i2c);

int readI2CData(I2CDef *i2c, uint8_t devAdd, uint32_t memAdd, uint8_t addSize, void *data, uint16_t size);

bool isI2CWriting(const I2CDef *i2c);

int writeI2CData(I2CDef *i2c, uint8_t devAdd, uint32_t memAdd, uint8_t addSize, const void *data, uint16_t size);

void updateI2CBus(I2CDef *i2c);

#ifdef __cplusplus
}
#endif

#endif /* I2C_H */