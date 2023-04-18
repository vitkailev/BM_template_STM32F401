#include "stm32f4xx_hal.h"

#include "settings.h"

static TIM_HandleTypeDef tim9Handle;
static UART_HandleTypeDef uart2Handle;
static I2C_HandleTypeDef i2c3Handle;
static CRC_HandleTypeDef crcHandle;

static int settingSystemClock(void) {
    RCC_OscInitTypeDef oscInit = {0};
    oscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    oscInit.HSIState = RCC_HSI_ON;
    oscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    oscInit.LSIState = RCC_LSI_ON;
    oscInit.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&oscInit) != HAL_OK)
        return 1;

    RCC_ClkInitTypeDef clkInit = {0};
    clkInit.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkInit.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    clkInit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clkInit.APB1CLKDivider = RCC_HCLK_DIV1;
    clkInit.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_0) != HAL_OK)
        return 2;
    return 0;
}

static int settingGPIO(void) {
    GPIO_InitTypeDef gpioInit = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_0;
    gpioInit.Mode = GPIO_MODE_INPUT;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpioInit);

    __HAL_RCC_GPIOD_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &gpioInit);
    return 0;
}

static int settingTimer(TimerDef *timer) {
    timer->obj = &tim9Handle;
    TIM_HandleTypeDef *timInit = (TIM_HandleTypeDef *) timer->obj;
    timInit->Instance = TIM9;
    timInit->Init.Period = timer->freq;
    timInit->Init.Prescaler = SystemCoreClock / timer->freq / 100 - 1;
    timInit->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timInit->Init.CounterMode = TIM_COUNTERMODE_UP;
    timInit->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    timInit->Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(timInit) != HAL_OK)
        return 1;
    return 0;
}

static int settingUART(UARTDef *uart) {
    uart->obj = &uart2Handle;
    UART_HandleTypeDef *uartInit = (UART_HandleTypeDef *) uart->obj;
    uartInit->Instance = USART2;
    uartInit->Init.BaudRate = uart->speed;
    uartInit->Init.WordLength = UART_WORDLENGTH_8B;
    uartInit->Init.StopBits = UART_STOPBITS_1;
    uartInit->Init.Parity = UART_PARITY_NONE;
    uartInit->Init.Mode = UART_MODE_TX_RX;
    uartInit->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartInit->Init.OverSampling = UART_OVERSAMPLING_8;
    if (HAL_UART_Init(uartInit) != HAL_OK)
        return 1;
    return 0;
}

static int settingI2C(I2CDef *i2c) {
    i2c->obj = &i2c3Handle;
    I2C_HandleTypeDef *i2cInit = (I2C_HandleTypeDef *) i2c->obj;
    i2cInit->Init.ClockSpeed = 100000;
    i2cInit->Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2cInit->Init.OwnAddress1 = 11;
    i2cInit->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2cInit->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2cInit->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2cInit->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(i2cInit) != HAL_OK)
        return 1;
    return 0;
}

static int settingCRC(MCUDef *mcu) {
    mcu->crc = &crcHandle;
    CRC_HandleTypeDef *crcInit = (CRC_HandleTypeDef *) mcu->crc;
    crcInit->Instance = CRC;
    if (HAL_CRC_Init(crcInit) != HAL_OK)
        return 1;
    return 0;
}

int initialization(MCUDef *mcu, UARTDef *uart, I2CDef *i2c) {
    int32_t result = 0;
    result |= settingSystemClock();
    settingGPIO();
    result |= (settingTimer(&mcu->timer) << 1);
    result |= (settingUART(uart) << 2);
    result |= (settingI2C(i2c) << 3);
    result |= (settingCRC(mcu) << 4);
    return result;
}

int turnOnInterrupts(MCUDef *mcu, UARTDef *uart) {
    if (HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *) mcu->timer.obj) != HAL_OK)
        return 1;
    if (HAL_UART_Receive_IT((UART_HandleTypeDef *) uart->obj, &uart->rxByte, 1) != HAL_OK)
        return 2;
    return 0;
}
