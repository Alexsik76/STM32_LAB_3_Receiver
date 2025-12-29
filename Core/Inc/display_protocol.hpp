/**
 * @file display_protocol.hpp
 * @brief Display command protocol structures
 */

#ifndef DISPLAY_PROTOCOL_H
#define DISPLAY_PROTOCOL_H

#include <stdint.h>

typedef enum {
    DISP_CMD_SET_STATUS,     // Set status bar text
    DISP_CMD_SET_MAIN_TEXT,  // Set main display text
    DISP_CMD_SHOW_KEY,       // Show single key press
    DISP_CMD_CLEAR           // Clear display
} DisplayCommand_t;

typedef struct {
    DisplayCommand_t command;
    char text[32];  // Message text
    char key;       // Single character (for SHOW_KEY)
} DisplayMessage_t;

#endif