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
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
}

/**
  * @brief  DeInitializes the Global MSP.
  * @note   This functiona is called from HAL_DeInit() function to perform system
  *         level de-initialization (GPIOs, clock, DMA, interrupt).
  * @retval None
  */
void HAL_MspDeInit(void) {

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM9) {
        __HAL_RCC_TIM9_CLK_ENABLE();
        HAL_NVIC_SetPriority(TIM1_BRK_TIM9_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(TIM1_BRK_TIM9_IRQn);
    }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM9) {
        __HAL_RCC_TIM9_FORCE_RESET();
        __HAL_RCC_TIM9_RELEASE_RESET();
        HAL_NVIC_DisableIRQ(TIM1_BRK_TIM9_IRQn);
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        __HAL_RCC_USART2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef initGPIO;
        initGPIO.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        initGPIO.Mode = GPIO_MODE_AF_PP;
        initGPIO.Pull = GPIO_NOPULL;
        initGPIO.Speed = GPIO_SPEED_FREQ_MEDIUM;
        initGPIO.Alternate = GPIO_AF7_USART2;
        HAL_GPIO_Init(GPIOA, &initGPIO);

        HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        __HAL_RCC_USART2_FORCE_RESET();
        __HAL_RCC_USART2_RELEASE_RESET();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_3);

        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C3) {
        __HAL_RCC_I2C3_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitTypeDef initGPIO;
        initGPIO.Mode = GPIO_MODE_AF_OD;
        initGPIO.Pull = GPIO_PULLUP;
        initGPIO.Speed = GPIO_SPEED_FREQ_MEDIUM;
        initGPIO.Alternate = GPIO_AF4_I2C3;
        initGPIO.Pin = GPIO_PIN_8;
        HAL_GPIO_Init(GPIOA, &initGPIO);
        initGPIO.Pin = GPIO_PIN_9;
        HAL_GPIO_Init(GPIOC, &initGPIO);

        HAL_NVIC_SetPriority(I2C3_EV_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_SetPriority(I2C3_ER_IRQn, 4, 0);
        HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
    if (hi2c->Instance == I2C3) {
        __HAL_RCC_I2C3_FORCE_RESET();
        __HAL_RCC_I2C3_RELEASE_RESET();

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8);
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_9);

        HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
    }
}

void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc) {
    __HAL_RCC_CRC_CLK_ENABLE();
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc) {
    __HAL_RCC_CRC_FORCE_RESET();
    __HAL_RCC_CRC_RELEASE_RESET();
}

/**
  * @brief  Initializes the PPP MSP.
  * @note   This functiona is called from HAL_PPP_Init() function to perform 
  *         peripheral(PPP) system level initialization (GPIOs, clock, DMA, interrupt)
  * @retval None
  */
void HAL_PPP_MspInit(void) {

}

/**
  * @brief  DeInitializes the PPP MSP.
  * @note   This functiona is called from HAL_PPP_DeInit() function to perform 
  *         peripheral(PPP) system level de-initialization (GPIOs, clock, DMA, interrupt)
  * @retval None
  */
void HAL_PPP_MspDeInit(void) {

}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
