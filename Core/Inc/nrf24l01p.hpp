/**
 * @file nrf24l01p.hpp
 * @brief NRF24L01+ radio chip C++ driver class
 */

#pragma once

#include "main.h"
#include "nrf24l01p.h"

/**
 * @class Nrf24l01p
 * @brief Object-oriented driver for NRF24L01+ 2.4GHz radio transceiver
 */
class Nrf24l01p
{
public:
    /**
     * @brief Constructor - initializes driver with hardware configuration
     * @param spi Pointer to HAL SPI handle
     * @param cs_port GPIO port for CSN (Chip Select)
     * @param cs_pin GPIO pin for CSN
     * @param ce_port GPIO port for CE (Chip Enable)
     * @param ce_pin GPIO pin for CE
     * @param payload_len Payload length in bytes
     */
    Nrf24l01p(SPI_HandleTypeDef* spi,
              GPIO_TypeDef* cs_port, uint16_t cs_pin,
              GPIO_TypeDef* ce_port, uint16_t ce_pin,
              uint8_t payload_len);

    // Main API functions
    void init_rx(channel MHz, air_data_rate bps);
    void init_tx(channel MHz, air_data_rate bps);

    void receive(uint8_t* rx_payload);
    void transmit(uint8_t* tx_payload);
    void handle_tx_irq();

    // Configuration
    void set_tx_address(uint8_t* address);
    void set_rx_address_p0(uint8_t* address);
    void set_rx_address_p1(uint8_t* address);
    
    bool is_data_ready(void);
    void reset_irq_flags(void);

    // Control
    void power_up();
    void power_down();
    void flush_rx_fifo();
    void flush_tx_fifo();
    void ce_high();
    void ce_low();

    // Status
    uint8_t get_status();
    uint8_t get_fifo_status();

    // IRQ flag clear
    void clear_rx_dr();
    void clear_tx_ds();
    void clear_max_rt();

private:
    // Hardware configuration
    SPI_HandleTypeDef* spi_handle;
    GPIO_TypeDef* csn_port;
    uint16_t           csn_pin;
    GPIO_TypeDef* ce_port;
    uint16_t           ce_pin;
    uint8_t            payload_length;

    // Pin control
    void cs_high();
    void cs_low();

    // SPI communication
    uint8_t read_register(uint8_t reg);
    uint8_t write_register(uint8_t reg, uint8_t value);
    void write_register_multi(uint8_t reg, uint8_t* value, uint8_t len);

    // Internal functions
    void reset();
    void set_prx_mode();
    void set_ptx_mode();
    uint8_t read_rx_fifo(uint8_t* rx_payload);
    uint8_t write_tx_fifo(uint8_t* tx_payload);
    void set_rx_payload_widths(widths bytes);
    void set_crc_length(length bytes);
    void set_address_widths(widths bytes);
    void set_auto_retransmit_count(count cnt);
    void set_auto_retransmit_delay(delay us);
    void set_rf_channel(channel MHz);
    void set_rf_tx_output_power(output_power dBm);
    void set_rf_air_data_rate(air_data_rate bps);
};
