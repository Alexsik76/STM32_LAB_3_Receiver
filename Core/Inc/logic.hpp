/**
 * @file logic.hpp
 * @brief Application logic controller class definition
 */

#pragma once

#include "radio_protocol.hpp"   // Input data (from NRF24)
#include "display_protocol.hpp" // Output data (to display)
#include "cmsis_os.h"

// =============================================================================
// Servo Configuration Constants
// =============================================================================

/**
 * @brief PWM pulse width range for servo control (in microseconds)
 */
static constexpr uint32_t SERVO_MIN_PULSE_US = 1000;  ///< Minimum servo pulse width (1ms)
static constexpr uint32_t SERVO_MAX_PULSE_US = 2000;  ///< Maximum servo pulse width (2ms)
static constexpr uint32_t SERVO_SAFE_MIN_US = 500;    ///< Safety limit minimum (0.5ms)
static constexpr uint32_t SERVO_SAFE_MAX_US = 2500;   ///< Safety limit maximum (2.5ms)

/**
 * @brief Servo trim adjustments (experimental tuning values)
 */
static constexpr int16_t SERVO_TRIM_X = 0;  ///< X-axis servo trim offset in microseconds
static constexpr int16_t SERVO_TRIM_Y = 0;  ///< Y-axis servo trim offset in microseconds

// =============================================================================

/**
 * @class LogicTask
 * @brief MVC Controller - processes radio data and controls display
 */
class LogicTask {
public:
    LogicTask();
    
    /**
     * @brief Main logic task loop
     */
    void task();

private:
    /**
     * @brief Helper to send commands to display queue
     * @param cmd Display command type
     * @param text Text message (nullable)
     * @param key Single character key (default: 0)
     */
    void send_to_display(DisplayCommand_t cmd, const char* text, char key = 0);
};

extern LogicTask g_logic;