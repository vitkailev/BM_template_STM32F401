#include <string.h>

#include "stm32f4xx_hal.h"

#include "i2c.h"

/**
 * @brief Check, that the I2C interface is initialized
 * @param i2c is the base I2C data structure
 * @return True - I2C interface has been initialized, otherwise - False
 */
static bool isInit(const I2CDef *i2c) {
    return i2c->isInit;
}

/**
 * @brief I2C interface initialization
 * @param i2c is the base I2C data structure
 * @return I2C_SUCCESS - the data structure has been initialized, otherwise - error code
 */
int I2C_init(I2CDef *i2c) {
    if (i2c == NULL)
        return I2C_WRONG_DATA;

    i2c->errType = HAL_I2C_ERROR_NONE;
    i2c->errors = 0;
    memset(i2c->buffer, 0, I2C_BUFFER_SIZE);

    i2c->isInit = true;
    return I2C_SUCCESS;
}

/**
 * @brief Check, that the I2C interface is reading data
 * @param i2c is the base I2C data structure
 * @return True - is reading, otherwise - False
 */
bool I2C_isReading(const I2CDef *i2c) {
    return i2c->isReading;
}

/**
 * @brief Reading data via I2C interface into an internal buffer
 * @param i2c is the base I2C data structure
 * @param devAdd is the device address on the I2C bus
 * @param size is the amount of data need to be read (bytes)
 * @return I2C_Errors value
 */
int I2C_readData(I2CDef *i2c, uint16_t devAdd, uint16_t size) {
    if (!isInit(i2c))
        return I2C_NOT_INIT;
    if (I2C_isReading(i2c) || I2C_isWriting(i2c))
        return I2C_BUSY;
    if (size == 0 || size > I2C_BUFFER_SIZE)
        return I2C_WRONG_DATA;

    i2c->isReading = true;
    i2c->deviceAdd = devAdd;
    memset(i2c->buffer, 0, I2C_BUFFER_SIZE);
    i2c->size = size;
    i2c->errType = HAL_I2C_ERROR_NONE;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_I2C_Master_Seq_Receive_IT((I2C_HandleTypeDef *) i2c->handle, i2c->deviceAdd,
                                           i2c->buffer, i2c->size,
                                           I2C_LAST_FRAME);

    return (result == HAL_OK) ? I2C_SUCCESS : I2C_HW_ERROR;
}

/**
 * @brief Get a pointer to the received data
 * @param i2c is the base I2C data structure
 * @return pointer to the internal buffer
 */
const uint8_t *I2C_getData(const I2CDef *i2c) {
    return i2c->buffer;
}

/**
 * @brief Get the size of the received data
 * @param i2c is the base I2C data structure
 * @return an amount of available data (bytes)
 */
uint16_t I2C_getDataSize(const I2CDef *i2c) {
    return i2c->size;
}

/**
 * @brief Check, that the I2C interface is sending data
 * @param i2c is the base I2C data structure
 * @return True - is writing, otherwise - False
 */
bool I2C_isWriting(const I2CDef *i2c) {
    return i2c->isWriting;
}

/**
 * @brief Sending data via I2C interface
 * @param i2c is the base I2C data structure
 * @param devAdd is the device address on the I2C bus
 * @param src is the pointer of the source data
 * @param size is the size of the source data (bytes)
 * @param isNeedStop is a flag, that indicates whether a "Stop condition" should be generated after a transaction or not
 * @return I2C_Errors code
 */
int I2C_writeData(I2CDef *i2c, uint16_t devAdd, const void *src, uint16_t size, bool isNeedStop) {
    if (!isInit(i2c))
        return I2C_NOT_INIT;
    if (I2C_isReading(i2c) || I2C_isWriting(i2c))
        return I2C_BUSY;
    if (src == NULL || size == 0 || size > I2C_BUFFER_SIZE)
        return I2C_WRONG_DATA;

    i2c->isWriting = true;
    i2c->deviceAdd = devAdd;
    memcpy((void *) i2c->buffer, src, size);
    i2c->size = size;
    i2c->errType = HAL_I2C_ERROR_NONE;

    HAL_StatusTypeDef result = HAL_ERROR;
    result = HAL_I2C_Master_Seq_Transmit_IT((I2C_HandleTypeDef *) i2c->handle, i2c->deviceAdd,
                                            i2c->buffer, i2c->size,
                                            // I2C_FIRST_AND_LAST_FRAME - just send one data sequence
                                            // I2C_FIRST_FRAME - send a command and then, without a "Stop" condition,
                                            // read the data (when it is necessary to set the reading address in
                                            // a device/IC), (clock stretching)
                                            (isNeedStop) ? I2C_FIRST_AND_LAST_FRAME : I2C_FIRST_FRAME);

    return (result == HAL_OK) ? I2C_SUCCESS : I2C_HW_ERROR;
}
