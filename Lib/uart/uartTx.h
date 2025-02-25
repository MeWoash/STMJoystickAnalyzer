#pragma once

#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void writeUart(const char* fmt, ...);

#ifdef __cplusplus
}
#endif