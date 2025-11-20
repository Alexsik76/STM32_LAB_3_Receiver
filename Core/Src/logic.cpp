/**
 * @file logic.cpp
 * @brief Application logic controller (MVC pattern)
 */

#include "logic.hpp"
#include "rtos_tasks.h"
#include <string.h>
#include <stdio.h>

// Global instance
LogicTask g_logic;

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

    while (1) {
        // Wait for packet from radio queue
        if (osMessageQueueGet(radioToLogicQueueHandle, &rx_packet, NULL, osWaitForever) == osOK) {
            
            // Parse mode and decide what to display (MVC: Controller logic)
            switch (rx_packet.mode) {
                
                case MODE_KEYPAD:
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Mode Keypad");
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, "");
                    if (rx_packet.payload[0] != 0) {
                        send_to_display(DISP_CMD_SHOW_KEY, "", rx_packet.payload[0]);
                    }
                    break;

                case MODE_SERVO:
                {
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Servo Ctrl");
                    // Servo control logic will go here
                    // For now - visualization for debugging:
                    char cmd_char = rx_packet.payload[0];
                    if (cmd_char == '2')      send_to_display(DISP_CMD_SET_MAIN_TEXT, "SERVO UP");
                    else if (cmd_char == '8') send_to_display(DISP_CMD_SET_MAIN_TEXT, "SERVO DOWN");
                    else if (cmd_char == '4') send_to_display(DISP_CMD_SET_MAIN_TEXT, "SERVO LEFT");
                    else if (cmd_char == '6') send_to_display(DISP_CMD_SET_MAIN_TEXT, "SERVO RIGHT");
                    else                      send_to_display(DISP_CMD_SET_MAIN_TEXT, "---");
                    break;
                }
                
                case MODE_AUTO:
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