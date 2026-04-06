#include "stm32f4xx_hal.h"

#include "settings.h"
#include "functions.h"

McuDef Mcu;

static int initPeripheral(void);

static void application(void);

int main(void) {
    HAL_Init();
    initPeripheral();

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST))
        Mcu.flags.isWDTTriggered = true;
    __HAL_RCC_CLEAR_RESET_FLAGS();

    changePinState(&Mcu.leds[LED_RED], true);

    while (1) {
        HAL_IWDG_Refresh((IWDG_HandleTypeDef *) Mcu.handles.wdt);

        application();
    }
    return 1;
}

static void application(void) {
    if (Mcu.flags.isWDTTriggered) {
        Mcu.flags.isWDTTriggered = false;
    }
    if (Mcu.flags.isSysTickTriggered) {
        Mcu.flags.isSysTickTriggered = false;

        checkPinState(&Mcu.button[BUTTON_1]);
    }
    if (Mcu.measTimer.isTriggered) {
        Mcu.measTimer.isTriggered = false;

        readAnalogValues(&Mcu.adc);
    }
    if (isPinTriggered(&Mcu.button[BUTTON_1])) {
        Mcu.button[BUTTON_1].isTriggered = false;
    }
    if (isADCFinished(&Mcu.adc)) {
        Mcu.adc.isFinished = false;

        convertADCResults(&Mcu.adc);
    }

    UART_update(&Mcu.uart, HAL_GetTick());
    if (UART_isHaveData(&Mcu.uart)) {
    }
}

static int initPeripheral(void) {
    Mcu.leds[LED_GREEN].pin = GPIO_PIN_12;
    Mcu.leds[LED_GREEN].handle = GPIOD;
    Mcu.leds[LED_ORANGE].pin = GPIO_PIN_13;
    Mcu.leds[LED_ORANGE].handle = GPIOD;
    Mcu.leds[LED_RED].pin = GPIO_PIN_14;
    Mcu.leds[LED_RED].handle = GPIOD;
    Mcu.leds[LED_BLUE].pin = GPIO_PIN_15;
    Mcu.leds[LED_BLUE].handle = GPIOD;
    Mcu.button[BUTTON_1].duration = 50; // ms
    Mcu.button[BUTTON_1].pin = GPIO_PIN_0;
    Mcu.button[BUTTON_1].handle = GPIOA;

    UART_init(&Mcu.uart);
    I2C_init(&Mcu.i2c);

    if (SETTING_SUCCESS == initialization(&Mcu)) {
        HAL_TIM_PWM_Start((TIM_HandleTypeDef *) Mcu.pwmTimer.handle, TIM_CHANNEL_3);
        setPWMDutyCycle(&Mcu.pwmTimer, TIM_CHANNEL_3, 37);

        HAL_TIM_Base_Start_IT((TIM_HandleTypeDef *) Mcu.measTimer.handle);
    }
    return 0;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1) {
    }
}
#endif
