#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "main.h"

// --- C-Обгортки ---
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief RTOS task entry function (called by freertos.c).
 */
void radio_task_entry(void *argument);

/**
 * @brief C-callable function to initialize the radio subsystem.
 */
void radio_init(void);

/**
 * @brief Global semaphore, signaled by the nRF24L01 IRQ pin.
 */
extern SemaphoreHandle_t g_radio_irq_sem;

#ifdef __cplusplus
}
#endif

// --- C++ Світ ---
#ifdef __cplusplus

/**
 * @brief Main class for managing the nRF24L01 Radio (Receiver).
 */
class MyRadio
{
public:
    /**
     * @brief Constructor.
     */
    MyRadio();

    /**
     * @brief The main RTOS task loop for the radio.
     */
    void task(void);

private:
    /**
     * @brief Initializes the nRF24L01 controller.
     * @return true if successful, false otherwise.
     */
    bool init(void);

    // tx_queue and send_data() видалені, оскільки це приймач
};

#endif // __cplusplus
