#pragma once

#include "nrf24l01p.hpp"
#include "cmsis_os.h"

// Включаємо наш протокол, щоб знати розмір пакету
#include "radio_protocol.hpp" 

class MyRadio {
public:
    MyRadio();
    void task();

private:
    Nrf24l01p radio;
    bool init(void);
};

extern MyRadio g_radio;