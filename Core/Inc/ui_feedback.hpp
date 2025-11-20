/**
 * @file ui_feedback.hpp
 * @brief LED blink feedback functions
 */

#ifndef UI_FEEDBACK_H
#define UI_FEEDBACK_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Perform a single LED blink (50ms)
 */
void UI_Blink_Once(void);

/**
 * @brief Perform multiple LED blinks
 * @param count Number of blinks
 */
void UI_Blink_Multi(int count);

#ifdef __cplusplus
}
#endif

#endif // UI_FEEDBACK_H
