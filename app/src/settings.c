#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rcc.h"

#include "settings.h"

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

int initialization(MCUDef *mcu) {
    uint32_t result = 0;
    result |= settingSystemClock();
    settingGPIO();
    return (int) result;
}



