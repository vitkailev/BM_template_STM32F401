#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

#include "variables.h"

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == ((TIM_HandleTypeDef *) MCU.timer.obj)->Instance) {
        MCU.timer.isTriggered = true;
    }
}
