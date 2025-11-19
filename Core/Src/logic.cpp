#include "logic.hpp"
#include "rtos_tasks.h" // Тут наші extern хендли
#include <string.h>
#include <stdio.h>

// Глобальний об'єкт
LogicTask g_logic;

// --- C-Wrapper (для виклику з freertos.c) ---
extern "C" {
    void logic_run_task(void) {
        g_logic.task();
    }
}

// --- Реалізація ---

LogicTask::LogicTask() {
    // Конструктор поки порожній
}

void LogicTask::task() {
    
    RadioPacket rx_packet;

    // Початковий статус
    send_to_display(DISP_CMD_SET_STATUS, "Logic Ready");

    while (1) {
        // 1. Чекаємо пакет від Радіо (з нової черги radioToLogicQueueHandle)
        if (osMessageQueueGet(radioToLogicQueueHandle, &rx_packet, NULL, osWaitForever) == osOK) {
            
            // 2. Парсимо режими (Логіка MVC: Controller вирішує)
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
                    send_to_display(DISP_CMD_SET_STATUS, "RX: Servo Ctrl");
                    // Тут буде логіка керування моторами.
                    // А поки - візуалізація для налагодження:
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
                    // Передаємо весь рядок на екран
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, rx_packet.payload);
                    break;

                default:
                {
                    // Невідомий режим
                   send_to_display(DISP_CMD_CLEAR, NULL);
                    send_to_display(DISP_CMD_SET_STATUS, "Unknown Data");
                    send_to_display(DISP_CMD_SET_MAIN_TEXT, "Error");
                    
                    break;
                }
            }
        }
    }
}

// Приватний метод для спрощення відправки в чергу дисплея
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

    // Кладемо в чергу дисплея (використовуємо хендл з rtos_tasks.h)
    osMessageQueuePut(displayQueueHandleHandle, &msg, 0, 0);
}