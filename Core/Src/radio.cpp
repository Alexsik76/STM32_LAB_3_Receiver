#include "radio.hpp"
#include "cmsis_os.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "rtos_tasks.h"     // Тут оголошені extern хендли (radioIrqSemHandle, displayQueueHandleHandle)
#include "ui_feedback.hpp" 
#include "nrf24l01p.hpp"

// --- Глобальні об'єкти ---
MyRadio g_radio;
extern SPI_HandleTypeDef hspi1;

// Адреса приймача (має збігатися з TX_ADDRESS передавача)
uint8_t RX_ADDRESS[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

// --- C-Wrappers ---
extern "C" {

// Функції, які викликаються з main.c / freertos.c
void radio_init(void) {
    // Порожня, ініціалізація відбувається в конструкторі та методі init()
}

void radio_run_task(void) {
    g_radio.task();
}

} // extern "C"

// --- C++ Implementation ---

MyRadio::MyRadio()
    : radio(&hspi1,
            NRF24_CSN_GPIO_Port, NRF24_CSN_Pin,
            NRF24_CE_GPIO_Port,  NRF24_CE_Pin,
            RADIO_PAYLOAD_SIZE) // 32 байти
{
}

bool MyRadio::init(void)
{
    // 1. Базова ініціалізація (106 канал, 1Mbps - як у передавача)
    radio.init_rx(106, _1Mbps); 

    // 2. Налаштовуємо трубу (Pipe 1) на прийом
    radio.set_rx_address_p1(RX_ADDRESS);
    
    // 3. Вмикаємо живлення
    radio.power_up();

    // 4. Переходимо в режим прослуховування (CE High)
    radio.ce_high(); 

    return true;
}

void MyRadio::task(void)
{
    if (!this->init()) {
        // Якщо ініціалізація не вдалась, видаляємо задачу
        vTaskDelete(NULL);
    }

    RadioPacket rx_buffer; // Буфер для прийому даних

    while(1)
    {
        // --- 1. Чекаємо на СИГНАЛ ВІД РАДІО (IRQ) ---
        // Чекаємо, поки пін IRQ дасть сигнал (через семафор)
        if (osSemaphoreAcquire(radioIrqSemHandle, osWaitForever) == osOK)
        {
            // 2. Перевіряємо, чи є дані в буфері чіпа
            if (radio.is_data_ready())
            {
                // 3. Читаємо дані
                radio.receive((uint8_t*)&rx_buffer);

                // 4. Кладемо дані в чергу для дисплея
                // timeout 0: якщо черга повна, пакет просто пропаде (це краще, ніж зависання)
                osMessageQueuePut(radioToLogicQueueHandle, (uint8_t*)&rx_buffer, 0, 0);
                
                // Індикація прийому (якщо реалізовано)
                UI_Blink_Once();
            }

            // 5. Скидаємо прапори переривань на чіпі, щоб пін IRQ піднявся назад
            radio.reset_irq_flags(); 
        }
    }
}