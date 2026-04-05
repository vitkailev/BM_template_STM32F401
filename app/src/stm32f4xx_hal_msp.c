/**
  ******************************************************************************
  * @file    stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @brief   This file contains the HAL System and Peripheral (PPP) MSP initialization
  *          and de-initialization functions.
  *          It should be copied to the application folder and renamed into 'stm32f4xx_hal_msp.c'.           
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP HAL MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions HAL MSP Private Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @note   This function is called from HAL_Init() function to perform system
  *         level initialization (GPIOs, clock, DMA, interrupt).
  * @retval None
  */
void HAL_MspInit(void) {
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
}

/**
  * @brief  DeInitializes the Global MSP.
  * @note   This functiona is called from HAL_DeInit() function to perform system
  *         level de-initialization (GPIOs, clock, DMA, interrupt).
  * @retval None
  */
void HAL_MspDeInit(void) {
}

/**
 * @brief Initialize the base timers, turn ON a clock source and setup interrupt vector
 * @param htim is the pointer to the data structure of the base timer handle (HAL).
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM11) {
        __HAL_RCC_TIM11_CLK_ENABLE();

        HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
    }
}

/**
 * @brief DeInitialize the base timers
 * @param htim is the pointer to the data structure of the base timer handle (HAL).
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM11) {
        __HAL_RCC_TIM11_FORCE_RESET();
        __HAL_RCC_TIM11_RELEASE_RESET();
        __HAL_RCC_TIM11_CLK_DISABLE();

        HAL_NVIC_DisableIRQ(TIM1_TRG_COM_TIM11_IRQn);
    }
}

/**
 * @brief Initialize the PWM timer mode, turn ON a clock source and setup GPIOs
 * @param htim is the pointer to the data structure of the timer handle (HAL).
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
    GPIO_InitTypeDef gpioInit = {0};

    if (htim->Instance == TIM4) {
        __HAL_RCC_TIM4_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        gpioInit.Pin = GPIO_PIN_8;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Pull = GPIO_PULLDOWN;
        gpioInit.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        gpioInit.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &gpioInit);
    }
}

/**
 * @brief DeInitialize the PWM timer mode
 * @param htim is the pointer to the data structure of the timer handle (HAL).
 */
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        __HAL_RCC_TIM4_FORCE_RESET();
        __HAL_RCC_TIM4_RELEASE_RESET();
        __HAL_RCC_TIM4_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
    }
}

/**
 * @brief Initialize the ADC module, turn ON a clock source, setup GPIO and interrupt vector
 * @param hadc is the pointer to the data structure of the ADC handle (HAL).
 */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {
    GPIO_InitTypeDef gpioInit = {0};
    gpioInit.Mode = GPIO_MODE_ANALOG;
    gpioInit.Pull = GPIO_NOPULL;

    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        gpioInit.Pin = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOB, &gpioInit);

        __HAL_RCC_GPIOA_CLK_ENABLE();
        gpioInit.Pin = GPIO_PIN_1;
        HAL_GPIO_Init(GPIOA, &gpioInit);

        HAL_NVIC_SetPriority(ADC_IRQn, 3, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
}

/**
 * @brief DeInitialize the ADC module
 * @param hadc is the pointer to the data structure of the ADC handle (HAL).
 */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc) {
    if (hadc->Instance == ADC1) {
        __HAL_RCC_ADC_FORCE_RESET();
        __HAL_RCC_ADC_RELEASE_RESET();
        __HAL_RCC_ADC1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_0);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

        HAL_NVIC_DisableIRQ(ADC_IRQn);
    }
}

/**
 * @brief Initialize the UART interfaces, turn ON a clock source, setup GPIO and interrupt vector
 * @param huart is the pointer to the data structure of the UART handle (HAL).
 */
void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef initGPIO = {0};

    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        initGPIO.Pin = GPIO_PIN_6 | GPIO_PIN_7;
        initGPIO.Mode = GPIO_MODE_AF_PP;
        initGPIO.Pull = GPIO_PULLUP;
        initGPIO.Speed = GPIO_SPEED_FREQ_HIGH;
        initGPIO.Alternate = GPIO_AF7_USART1;
        HAL_GPIO_Init(GPIOB, &initGPIO);

        HAL_NVIC_SetPriority(USART1_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
}

/**
 * @brief DeInitialize the UART interfaces
 * @param huart is the pointer to the data structure of the UART handle (HAL).
 */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();
        __HAL_RCC_USART1_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_7);

        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

/**
 * @brief Initialize the I2C interfaces, turn ON a clock source, setup GPIO and interrupt vector
 * @param hi2c is the pointer to the data structure of the I2C handle (HAL).
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    GPIO_InitTypeDef initGPIO = {0};
    initGPIO.Mode = GPIO_MODE_AF_OD;
    initGPIO.Pull = GPIO_PULLUP;
    initGPIO.Speed = GPIO_SPEED_FREQ_HIGH;
    initGPIO.Alternate = GPIO_AF4_I2C3;

    if (hi2c->Instance == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        initGPIO.Pin = GPIO_PIN_8;
        HAL_GPIO_Init(GPIOA, &initGPIO);

        __HAL_RCC_GPIOC_CLK_ENABLE();
        initGPIO.Pin = GPIO_PIN_9;
        HAL_GPIO_Init(GPIOC, &initGPIO);

        HAL_NVIC_SetPriority(I2C3_EV_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_SetPriority(I2C3_ER_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
    }
}

/**
 * @brief DeInitialize the I2C interfaces
 * @param hi2c is the pointer to the data structure of the I2C handle (HAL).
 */
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C3) {
        __HAL_RCC_I2C3_FORCE_RESET();
        __HAL_RCC_I2C3_RELEASE_RESET();
        __HAL_RCC_I2C3_CLK_DISABLE();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);

        HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
    }
}

/**
 * @brief Initialize the CRC module, turn ON a clock source
 * @param hcrc is the pointer to the data structure of the CRC handle (HAL).
 */
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc) {
    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_ENABLE();
    }
}

/**
 * @brief DeInitialize the CRC module
 * @param hcrc is the pointer to the data structure of the CRC handle (HAL).
 */
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc) {
    if (hcrc->Instance == CRC) {
        __HAL_RCC_CRC_FORCE_RESET();
        __HAL_RCC_CRC_RELEASE_RESET();
        __HAL_RCC_CRC_CLK_DISABLE();
    }
}
