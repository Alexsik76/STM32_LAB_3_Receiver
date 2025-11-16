#include "radio.h"
#include "nrf24l01p.h" // Правильний драйвер
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "display.h"
#include <stdio.h>
#include "ui_feedback.h"

// --- Global Objects ---
SemaphoreHandle_t g_radio_irq_sem; // Семафор для IRQ
MyRadio g_radio;                   // Глобальний об'єкт радіо
extern MyDisplay g_display;        // Глобальний об'єкт дисплея

uint8_t TX_ADDRESS[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};
uint8_t RX_ADDRESS[5] = {0xEE, 0xDD, 0xCC, 0xBB, 0xAA};

// --- C-Wrappers (Entry Point) ---
extern "C" {

void radio_init(void)
{
    g_radio_irq_sem = xSemaphoreCreateBinary();
}

void radio_task_entry(void *argument)
{
    g_radio.task();
}

} // extern "C"

// --- C++ Class Implementation ---

MyRadio::MyRadio()
{
    // Конструктор. Черга tx_queue не потрібна.
}

/**
 * @brief Ініціалізація nRF24 як Приймача (RX)
 */
bool MyRadio::init(void)
{
    // Викликаємо ініціалізацію для RX
    nrf24l01p_rx_init(106, _1Mbps);
    nrf24l01p_set_rx_address_p0(RX_ADDRESS);
	nrf24l01p_set_tx_address(RX_ADDRESS);
    return true;
}

/**
 * @brief Головна задача радіо (тільки Приймач)
 */
void MyRadio::task(void)
{
    // 1. Оголошуємо буфер ЗРАЗУ на початку функції
    uint8_t rx_buf[NRF24L01P_PAYLOAD_LENGTH];
    memset(rx_buf, 0, NRF24L01P_PAYLOAD_LENGTH); // Очищуємо "сміття" з пам'яті

    if (!this->init()) {
        g_display.set_status_text("Radio Fail!");
        vTaskDelete(NULL);
    }

    // 2. Встановлюємо початковий СТАТУС (один раз)
    // Він не буде змінюватись
    g_display.set_status_text("Listening...");
    g_display.set_main_text(""); // Очищуємо головну зону

    // nrf24l01p_rx_init вже встановив CE HIGH, модуль слухає ефір

    while(1)
    {
        // Чекаємо на IRQ (отримання даних)
        if (xSemaphoreTake(g_radio_irq_sem, portMAX_DELAY) == pdTRUE)
        {
            // IRQ спрацював
            uint8_t status = nrf24l01p_get_status();

            if (status & (1 << 6)) // Перевірка прапора RX_DR
            {
                UI_Blink_Triple(); // Блимаємо діодом

                // Отримано дані
                nrf24l01p_rx_receive(rx_buf);
                rx_buf[NRF24L01P_PAYLOAD_LENGTH - 1] = '\0'; // Гарантуємо нуль-термінатор

                // 3. ОНОВЛЮЄМО ТІЛЬКИ ГОЛОВНУ ЗОНУ
                // Статус "Listening..." залишається вгорі
                g_display.set_main_text((char*)rx_buf); // Дані в центрі
            }
            else
            {
                // Скидаємо інші прапори
                nrf24l01p_clear_rx_dr();
                nrf24l01p_clear_tx_ds();
                nrf24l01p_clear_max_rt();
            }
        }
    }
}
