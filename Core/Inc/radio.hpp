/**
 * @file radio.hpp
 * @brief Radio task class definition for NRF24L01+ receiver
 */

#pragma once

#include "nrf24l01p.hpp"
#include "cmsis_os.h"
#include "radio_protocol.hpp"

/**
 * @class MyRadio
 * @brief Manages NRF24L01+ radio reception in FreeRTOS task
 */
class MyRadio {
public:
    MyRadio();
    
    /**
     * @brief Main radio task loop
     */
    void task();

private:
    Nrf24l01p radio;
    
    /**
     * @brief Initialize radio hardware
     * @return true on success
     */
    bool init(void);
};

extern MyRadio g_radio;