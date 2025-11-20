/**
 * @file radio.cpp
 * @brief NRF24L01+ radio receiver task implementation
 */

#include "radio.hpp"
#include "cmsis_os.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "rtos_tasks.h"
#include "ui_feedback.hpp" 
#include "nrf24l01p.hpp"

// Global objects
MyRadio g_radio;
extern SPI_HandleTypeDef hspi1;

// RX address (must match transmitter's TX_ADDRESS)
uint8_t RX_ADDRESS[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

// C wrapper functions for FreeRTOS integration
extern "C" {

void radio_init(void) {
    // Initialization happens in constructor and init() method
}

void radio_run_task(void) {
    g_radio.task();
}

} // extern "C"

// C++ Implementation

MyRadio::MyRadio()
    : radio(&hspi1,
            NRF24_CSN_GPIO_Port, NRF24_CSN_Pin,
            NRF24_CE_GPIO_Port,  NRF24_CE_Pin,
            RADIO_PAYLOAD_SIZE) // 32 bytes
{
}

/**
 * @brief Initialize NRF24L01+ in RX mode
 * @return true on success
 */
bool MyRadio::init(void)
{
    // Basic initialization (channel 106, 1Mbps - same as transmitter)
    radio.init_rx(106, _1Mbps); 

    // Configure Pipe 1 for reception
    radio.set_rx_address_p1(RX_ADDRESS);
    
    // Power up the chip
    radio.power_up();

    // Enter listening mode (CE High)
    radio.ce_high(); 

    return true;
}

/**
 * @brief Main radio task - waits for IRQ and processes received packets
 */
void MyRadio::task(void)
{
    if (!this->init()) {
        // If initialization failed, delete task
        vTaskDelete(NULL);
    }

    RadioPacket rx_buffer; // Reception buffer

    while(1)
    {
        // Wait for radio IRQ signal (via semaphore)
        if (osSemaphoreAcquire(radioIrqSemHandle, osWaitForever) == osOK)
        {
            // Check if data is ready in chip buffer
            if (radio.is_data_ready())
            {
                // Read data from radio
                radio.receive((uint8_t*)&rx_buffer);

                // Put data into logic queue
                // Timeout 0: if queue is full, packet is dropped (better than blocking)
                osStatus_t status = osMessageQueuePut(radioToLogicQueueHandle, (uint8_t*)&rx_buffer, 0, 0);
                
                // Visual feedback only on successful queue put
                if (status == osOK) {
                    UI_Blink_Once();
                }
                // If queue is full, packet is lost silently
            }

            // Clear IRQ flags on chip so IRQ pin goes back high
            radio.reset_irq_flags(); 
        }
    }
}