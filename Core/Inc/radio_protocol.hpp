/**
 * @file radio_protocol.hpp
 * @brief Common data structures for radio communication
 */

#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include <stdint.h>

#define RADIO_PAYLOAD_SIZE 32

// System modes (shared between TX and RX)
enum class SystemMode : uint8_t {
    Keypad = 0,    // Mode 1: Keypad transmission
    Servo,         // Mode 2: Servo control (2,4,6,8)
    Auto           // Mode 3: Auto text transmission
};

// Packet structure
typedef struct {
    uint8_t mode;                 // SystemMode_t (1 byte)
    char    payload[31];          // Data (key character or string)
} RadioPacket;

#endif