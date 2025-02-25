#pragma once

#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void waitForUartByteReceive();
char* getUartBuffer();
void addCallback(void(*callback)());

#ifdef __cplusplus
}
#endif