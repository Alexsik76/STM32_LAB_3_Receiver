/**
 * @file rtos_tasks.h
 * @brief FreeRTOS task declarations and shared handles
 */

#ifndef __RTOS_TASKS_H
#define __RTOS_TASKS_H

#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

// Queue handles
extern osMessageQueueId_t displayQueueHandleHandle; // Queue to display task
extern osMessageQueueId_t radioToLogicQueueHandle;  // Queue from radio to logic

// Semaphore handles
extern osSemaphoreId_t radioIrqSemHandle;   // IRQ semaphore for radio  
extern osSemaphoreId_t i2cTxDoneSemHandle;  // I2C DMA complete semaphore

// Task entry points
void display_run_task(void);
void radio_run_task(void);
void logic_run_task(void);

#ifdef __cplusplus
}
#endif
#endif
