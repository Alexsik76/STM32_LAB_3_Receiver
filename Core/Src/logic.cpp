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
                    
                    // 1. Отримуємо сирі дані (0..255)
                    uint8_t val_x = rx_packet.payload[0];
                    uint8_t val_y = rx_packet.payload[1];

                    int16_t trim_x = 0;  // Спробуй підібрати експериментально
                    int16_t trim_y = 0;

                    // 2. Математика для моторів (залишається!)
                    // Нам все ще треба перетворити 0..255 у 1000..2000 для таймера
                    uint32_t pulse_x = 1000 + ((uint32_t)val_x * 1000 / 255) + trim_x;
                    uint32_t pulse_y = 1000 + ((uint32_t)val_y * 1000 / 255) + trim_y;

                    if (pulse_x < 500) pulse_x = 500;
                    if (pulse_x > 2500) pulse_x = 2500;
                    if (pulse_y < 500) pulse_y = 500;
                    if (pulse_y > 2500) pulse_y = 2500;
                    // 3. Крутимо мотори
                    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse_x);
                    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse_y);

                    // 4. Візуалізація (ЗМІНЕНО)
                    // Виводимо вхідні байти, а не мікросекунди.
                    // Формат: "X:127 Y:127" (влізе ідеально)
                    char buf[32];
                    snprintf(buf, sizeof(buf), "X:%3d Y:%3d", val_x, val_y);
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, buf);
                    
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