/**
 * @file display.hpp
 * @brief Display task class definition for SSD1306 OLED
 */

#pragma once
#include "main.h"
#include "cmsis_os.h" 
#include "radio_protocol.hpp"

/**
 * @class MyDisplay
 * @brief Manages SSD1306 OLED display in FreeRTOS task (MVC View)
 */
class MyDisplay {
public:
    MyDisplay(I2C_HandleTypeDef *hi2c);

    /**
     * @brief Setup task handles (must be called before task())
     */
    void setup(osMessageQueueId_t queue, osSemaphoreId_t i2c_sem);
    
    /**
     * @brief Main display task loop
     */
    void task();

private:
    I2C_HandleTypeDef *hi2c;
    
    osMessageQueueId_t queueHandle; 
    osSemaphoreId_t i2cSemHandle;

    char status_text[32];
    char main_text[32];
    char current_key;

    bool init();
    void update_screen_internal();
    void set_status_text(const char* text);
    void set_main_text(const char* text);
    void on_key_press(char key);
    void clear_all();
};