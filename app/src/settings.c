#include "stm32f4xx_hal.h"

#include "settings.h"

static TIM_HandleTypeDef timer4Handle;
static TIM_HandleTypeDef timer11Handle;
static ADC_HandleTypeDef adcHandle;
static UART_HandleTypeDef uart1Handle;
static I2C_HandleTypeDef i2c3Handle;
static CRC_HandleTypeDef crcHandle;
static IWDG_HandleTypeDef wdtHandle;

/**
 * @brief Setting system clocks
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingSystemClock(void) {
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    RCC_OscInitTypeDef oscInit = {0};
    oscInit.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    oscInit.LSEState = RCC_LSE_OFF;
    oscInit.HSEState = RCC_HSE_OFF;
    oscInit.LSIState = RCC_LSI_ON;
    oscInit.HSIState = RCC_HSI_ON;
    oscInit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;

    // Datasheet, DS9716 Rev. 11, Electrical characteristics, PLL characteristics, page 81
    oscInit.PLL.PLLState = RCC_PLL_ON;
    oscInit.PLL.PLLSource = RCC_PLLSOURCE_HSI; // 16MHz
    oscInit.PLL.PLLM = 16;
    oscInit.PLL.PLLN = 288; // 288MHz
    oscInit.PLL.PLLP = RCC_PLLP_DIV4; // 72MHz
    oscInit.PLL.PLLQ = 6; // 48MHz

    if (HAL_RCC_OscConfig(&oscInit) != HAL_OK)
        return SETTING_ERROR;

    RCC_ClkInitTypeDef clkInit = {0};
    clkInit.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    clkInit.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;

    // Datasheet, DS9716 Rev. 11, Electrical characteristics, Analog-to-digital converter characteristics, page 104
    // Vdda >= 2.4V, max 36MHz
    clkInit.AHBCLKDivider = RCC_SYSCLK_DIV1; // 72MHz
    clkInit.APB1CLKDivider = RCC_HCLK_DIV2; // 36MHz
    clkInit.APB2CLKDivider = RCC_HCLK_DIV2; // 36MHz

    if (HAL_RCC_ClockConfig(&clkInit, FLASH_LATENCY_2) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting GPIO pins (input and output)
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingGPIO(void) {
    GPIO_InitTypeDef gpioInit = {0};

    // Button
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_0;
    gpioInit.Mode = GPIO_MODE_INPUT;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &gpioInit);

    // LEDs
    __HAL_RCC_GPIOD_CLK_ENABLE();
    gpioInit.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_PULLDOWN;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &gpioInit);

    return SETTING_SUCCESS;
}

/**
 * @brief Setting general purpose timer
 * @param t is the base timer data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingTimer(TimerDef *t) {
    uint32_t sourceClock = HAL_RCC_GetPCLK2Freq();
    // APB2Divider != 1
    sourceClock *= 2;

    t->handle = (void *) &timer11Handle;
    t->freq = 4000;
    t->basePrescaler = t->currentPrescaler = 180 - 1;

    TIM_HandleTypeDef *timInit = (TIM_HandleTypeDef *) t->handle;
    timInit->Instance = TIM11;
    timInit->Init.Period = 100 - 1;
    timInit->Init.Prescaler = t->basePrescaler;
    timInit->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timInit->Init.CounterMode = TIM_COUNTERMODE_UP;
    timInit->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    timInit->Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(timInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting analog-to-digital converter (ADC)
 * @param adc is the base ADC data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingADC(AdcDef *adc) {
    adc->handle = (void *) &adcHandle;

    ADC_HandleTypeDef *adcInit = (ADC_HandleTypeDef *) adc->handle;
    adcInit->Instance = ADC1;

    // clock source - APB2
    adcInit->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;

    adcInit->Init.Resolution = ADC_RESOLUTION_12B;
    adcInit->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adcInit->Init.ScanConvMode = ENABLE;
    adcInit->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    adcInit->Init.ContinuousConvMode = DISABLE;
    adcInit->Init.NbrOfConversion = 3;
    adcInit->Init.DiscontinuousConvMode = ENABLE;
    adcInit->Init.NbrOfDiscConversion = 1;
    adcInit->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adcInit->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    adcInit->Init.DMAContinuousRequests = DISABLE;
    if (HAL_ADC_Init(adcInit) != HAL_OK)
        return SETTING_ERROR;

    ADC_ChannelConfTypeDef chInit = {0};

    // sampling time ( + 12 cycles, RM0368 Rev.6, section 11.5, page 222)
    chInit.SamplingTime = ADC_SAMPLETIME_144CYCLES; // 8.6667 us
    chInit.Offset = 0;

    // PA1, ADC1
    chInit.Channel = ADC_CHANNEL_1;
    chInit.Rank = 1;
    if (HAL_ADC_ConfigChannel(adcInit, &chInit) != HAL_OK)
        return SETTING_ERROR;

    // PB0, ADC1
    chInit.Channel = ADC_CHANNEL_8;
    chInit.Rank = 2;
    if (HAL_ADC_ConfigChannel(adcInit, &chInit) != HAL_OK)
        return SETTING_ERROR;

    // Temperature sensor, ADC1
    // Datasheet DS9716, 6.3.21 ,Ts_temp > 10us (accuracy - 1C)
    chInit.Channel = ADC_CHANNEL_18;
    chInit.Rank = 3;
    if (HAL_ADC_ConfigChannel(adcInit, &chInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting PWM output signal
 * @param t is the base timer data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingPWM(TimerDef *t) {
    uint32_t sourceClock = HAL_RCC_GetPCLK1Freq();
    // APB1Divider != 1
    sourceClock *= 2;

    t->handle = (void *) &timer4Handle;
    t->freq = 10000;
    t->basePrescaler = t->currentPrescaler = 72 - 1;

    TIM_HandleTypeDef *timInit = (TIM_HandleTypeDef *) t->handle;
    timInit->Instance = TIM4;
    timInit->Init.Period = 100 - 1;
    timInit->Init.Prescaler = t->basePrescaler;
    timInit->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    timInit->Init.CounterMode = TIM_COUNTERMODE_UP;
    timInit->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    timInit->Init.RepetitionCounter = 0;
    if (HAL_TIM_PWM_Init(timInit) != HAL_OK)
        return SETTING_ERROR;

    TIM_OC_InitTypeDef pwm = {0};
    pwm.OCMode = TIM_OCMODE_PWM1;
    pwm.OCPolarity = TIM_OCPOLARITY_HIGH;
    pwm.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    pwm.OCFastMode = TIM_OCFAST_DISABLE;
    pwm.OCIdleState = TIM_OCIDLESTATE_RESET;
    pwm.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    pwm.Pulse = 5;
    if (HAL_TIM_PWM_ConfigChannel(timInit, &pwm, TIM_CHANNEL_3) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting UART modules
 * @param uart is the UartDef data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingUART(UARTDef *uart) {
    uart->handle = (void *) &uart1Handle;

    UART_HandleTypeDef *uartInit = (UART_HandleTypeDef *) uart->handle;
    uartInit->Instance = USART1;
    uartInit->Init.BaudRate = 115200;
    uartInit->Init.WordLength = UART_WORDLENGTH_8B;
    uartInit->Init.StopBits = UART_STOPBITS_1;
    uartInit->Init.Parity = UART_PARITY_NONE;
    uartInit->Init.Mode = UART_MODE_TX_RX;
    uartInit->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uartInit->Init.OverSampling = UART_OVERSAMPLING_8;
    if (HAL_UART_Init(uartInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting I2C modules
 * @param i2c is the I2CDef data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingI2C(I2CDef *i2c) {
    i2c->handle = (void *) &i2c3Handle;

    I2C_HandleTypeDef *i2cInit = (I2C_HandleTypeDef *) i2c->handle;
    i2cInit->Instance = I2C3;
    i2cInit->Init.ClockSpeed = 100000;
    i2cInit->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2cInit->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2cInit->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2cInit->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    i2cInit->Init.DutyCycle = I2C_DUTYCYCLE_2;
    i2cInit->Init.OwnAddress1 = 0x69 << 1;
    i2cInit->Init.OwnAddress2 = 0x42 << 1;
    if (HAL_I2C_Init(i2cInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting Cyclic-Redundancy-Check (CRC) module
 * @param mcu is the base MCU data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingCRC(McuDef *mcu) {
    mcu->handles.crc = (void *) &crcHandle;

    CRC_HandleTypeDef *crcInit = (CRC_HandleTypeDef *) mcu->handles.crc;
    crcInit->Instance = CRC;
    if (HAL_CRC_Init(crcInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Setting watchdog timer (WDT) module
 * @param mcu is the base MCU data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
static int settingWDT(McuDef *mcu) {
    mcu->handles.wdt = (void *) &wdtHandle;

    IWDG_HandleTypeDef *wdtInit = (IWDG_HandleTypeDef *) mcu->handles.wdt;
    wdtInit->Instance = IWDG;
    wdtInit->Init.Prescaler = IWDG_PRESCALER_8;
    wdtInit->Init.Reload = 0x0FFF;
    if (HAL_IWDG_Init(wdtInit) != HAL_OK)
        return SETTING_ERROR;

    return SETTING_SUCCESS;
}

/**
 * @brief Run the MCU modules setting process
 * @param mcu is the base MCU data structure
 * @return SETTING_SUCCESS or SETTING_ERROR
 */
int initialization(McuDef *mcu) {
    if (SETTING_SUCCESS != settingSystemClock()) {
    } else if (SETTING_SUCCESS != settingGPIO()) {
    } else if (SETTING_SUCCESS != settingTimer(&mcu->measTimer)) {
    } else if (SETTING_SUCCESS != settingADC(&mcu->adc)) {
    } else if (SETTING_SUCCESS != settingPWM(&mcu->pwmTimer)) {
    } else if (SETTING_SUCCESS != settingUART(&mcu->uart)) {
    } else if (SETTING_SUCCESS != settingI2C(&mcu->i2c)) {
    } else if (SETTING_SUCCESS != settingCRC(mcu)) {
    } else if (SETTING_SUCCESS != settingWDT(mcu)) {
    } else {
        return SETTING_SUCCESS;
    }

    return SETTING_ERROR;
}
