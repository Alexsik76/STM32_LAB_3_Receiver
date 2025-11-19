#ifndef __RTOS_TASKS_H
#define __RTOS_TASKS_H

// Підключаємо FreeRTOS для доступу до "SemaphoreHandle_t"
#include "cmsis_os.h"

#ifdef __cplusplus
extern "C" {
#endif

extern osMessageQueueId_t displayQueueHandleHandle; // Черга до дисплея
extern osMessageQueueId_t radioToLogicQueueHandle;

extern osSemaphoreId_t radioIrqSemHandle;
extern osSemaphoreId_t i2cTxDoneSemHandle;

extern osMutexId_t i2cMutexHandle;

void display_run_task(void);
void radio_run_task(void);
void logic_run_task(void);

void radio_init(void);
void display_init(void);

#ifdef __cplusplus
}
#endif
#endif
