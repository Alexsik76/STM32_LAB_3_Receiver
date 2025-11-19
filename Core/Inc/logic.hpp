#pragma once

#include "radio_protocol.hpp"   // Вхідні дані (від NRF24)
#include "display_protocol.hpp" // Вихідні дані (до дисплея)
#include "cmsis_os.h"         // Для черг

class LogicTask {
public:
    LogicTask();
    
    // Головний метод
    void task();

private:
    // Допоміжні методи для відправки команд на дисплей
    void send_to_display(DisplayCommand_t cmd, const char* text, char key = 0);
    
    // Сюди ми будемо класти повідомлення для дисплея
    // (А пізніше сюди додамо чергу для сервоприводів)
};

extern LogicTask g_logic; // Глобальний об'єкт