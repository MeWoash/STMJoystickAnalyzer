#include "uartRx.h"
#include <array>
#include <string.h>

extern UART_HandleTypeDef& rxHuart;
constexpr size_t MAX_MESSAGE_SIZE = 100;

static std::array<char, MAX_MESSAGE_SIZE> receiveBuffer;
static uint32_t receiveBufferIndex = 0;
static void(*receiveLineCallback)();


void addCallback(void(*callback)())
{
    receiveLineCallback = callback;
}

char* getUartBuffer()
{
    return receiveBuffer.data();
}

void waitForUartByteReceive()
{
    HAL_UART_Receive_IT(&rxHuart, reinterpret_cast<uint8_t*>(&receiveBuffer[receiveBufferIndex]), 1);
}

void handleUartReceiveCallback()
{
    if (receiveBuffer[receiveBufferIndex] == '\n'
        || receiveBuffer[receiveBufferIndex] == '\r'
        || receiveBufferIndex == (MAX_MESSAGE_SIZE-1))
    {
        receiveBuffer[receiveBufferIndex] = '\0';
        if (receiveLineCallback)
        {
            receiveLineCallback();
        }
        receiveBufferIndex = 0;
        memset(receiveBuffer.data(), 0, receiveBuffer.size());
    }
    else
    {
        ++receiveBufferIndex;
    }
    waitForUartByteReceive();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &rxHuart)
    {
        handleUartReceiveCallback();
    }
}