/**
 * @file logic.cpp
 * @brief Application logic controller (MVC pattern)
 */

#include "logic.hpp"
#include "rtos_tasks.h"
#include "main.h" 
#include "tim.h"
#include <string.h>
#include <stdio.h>

// Global instance
LogicTask g_logic;
extern TIM_HandleTypeDef htim2;
// C wrapper for FreeRTOS
extern "C" {
    void logic_run_task(void) {
        g_logic.task();
    }
}

// Implementation

LogicTask::LogicTask() {
    // Constructor currently empty
}

/**
 * @brief Main logic task - processes radio packets and updates display
 */
void LogicTask::task() {
    
    RadioPacket rx_packet;

    // Initial status
    send_to_display(DISP_CMD_SET_STATUS, "Logic Ready");
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // PA0
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // PA2
    while (1) {
        // Wait for packet from radio queue
        if (osMessageQueueGet(radioToLogicQueueHandle, &rx_packet, NULL, osWaitForever) == osOK) {
            
            // Parse mode and decide what to display (MVC: Controller logic)
            switch (rx_packet.mode) {
                
                case static_cast<uint8_t>(SystemMode::Keypad):
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Mode Keypad");
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, "");
                    if (rx_packet.payload[0] != 0) {
                        send_to_display(DISP_CMD_SHOW_KEY, "", rx_packet.payload[0]);
                    }
                    break;

                case static_cast<uint8_t>(SystemMode::Servo):
                {
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Servo Ctrl");
                    
                    // 1. Read raw joystick data (0..255 from transmitter)
                    uint8_t val_x = rx_packet.payload[0];
                    uint8_t val_y = rx_packet.payload[1];

                    // 2. Convert 0..255 range to 1000..2000 microseconds for servo PWM
                    uint32_t pulse_x = SERVO_MIN_PULSE_US + 
                        ((uint32_t)val_x * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 255) + SERVO_TRIM_X;
                    uint32_t pulse_y = SERVO_MIN_PULSE_US + 
                        ((uint32_t)val_y * (SERVO_MAX_PULSE_US - SERVO_MIN_PULSE_US) / 255) + SERVO_TRIM_Y;

                    // 3. Apply safety limits to prevent servo damage
                    if (pulse_x < SERVO_SAFE_MIN_US) pulse_x = SERVO_SAFE_MIN_US;
                    if (pulse_x > SERVO_SAFE_MAX_US) pulse_x = SERVO_SAFE_MAX_US;
                    if (pulse_y < SERVO_SAFE_MIN_US) pulse_y = SERVO_SAFE_MIN_US;
                    if (pulse_y > SERVO_SAFE_MAX_US) pulse_y = SERVO_SAFE_MAX_US;
                    
                    // 4. Update servo positions via PWM timer
                    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse_x);
                    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_y);

                    // 5. Display raw input values for debugging
                    char buf[32];
                    snprintf(buf, sizeof(buf), "X:%3d Y:%3d", val_x, val_y);
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, buf);
                    
                    break;
                }
                
                case static_cast<uint8_t>(SystemMode::Auto):
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Auto Text");
                    // Pass entire string to screen
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, rx_packet.payload);
                    break;

                default:
                {
                    // Unknown mode
                   send_to_display(DISP_CMD_CLEAR, NULL);
                    send_to_display(DISP_CMD_SET_STATUS, "Unknown Data");
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, "Error");
                    
                    break;
                }
            }
        }
    }
}

/**
 * @brief Helper method to send commands to display queue
 * @param cmd Display command type
 * @param text Text to display (optional)
 * @param key Single character key (optional)
 */
void LogicTask::send_to_display(DisplayCommand_t cmd, const char* text, char key) {
    DisplayMessage_t msg;
    msg.command = cmd;
    
    if (text) {
        strncpy(msg.text, text, 31);
        msg.text[31] = '\0';
    } else {
        msg.text[0] = '\0';
    }
    
    msg.key = key;

    // Put in display queue
    osMessageQueuePut(displayQueueHandleHandle, &msg, 0, 0);
}