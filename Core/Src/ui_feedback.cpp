/**
 * @file ui_feedback.cpp
 * @brief LED blink feedback for user interface events
 */

#include <ui_feedback.hpp>
#include "cmsis_os.h"

/**
 * @brief Turn on LED (blink start)
 */
static void UI_Blink_Start(void)
{
  HAL_GPIO_WritePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin, GPIO_PIN_RESET);
}

/**
 * @brief Turn off LED (blink end)
 */
static void UI_Blink_End(void)
{
    HAL_GPIO_WritePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin, GPIO_PIN_SET);
}

/**
 * @brief Perform a single 50ms LED blink
 */
void UI_Blink_Once(void)
{
	UI_Blink_Start();
    osDelay(50);
	UI_Blink_End();
}

/**
 * @brief Perform multiple LED blinks
 * @param count Number of blinks (default: 3)
 */
void UI_Blink_Multi(int count)
{
	for (int i = 0; i < count; ++i)
	{
		UI_Blink_Once();
		osDelay(50);
	}
}
