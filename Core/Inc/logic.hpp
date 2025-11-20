/**
 * @file logic.hpp
 * @brief Application logic controller class definition
 */

#pragma once

#include "radio_protocol.hpp"   // Input data (from NRF24)
#include "display_protocol.hpp" // Output data (to display)
#include "cmsis_os.h"

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