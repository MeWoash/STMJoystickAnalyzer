#include "uartTx.h"
#include "staticVector.h"

#include <stdarg.h>
#include <array>
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef& txHuart;


constexpr size_t MAX_BUFFER_SIZE = 10;
constexpr size_t MAX_MESSAGE_SIZE = 100;

static static_vector<std::array<char, MAX_MESSAGE_SIZE>, MAX_BUFFER_SIZE> messages;

static void sendFromBuffer()
{
    auto& front = messages.front();
    HAL_UART_Transmit_IT(&txHuart, reinterpret_cast<uint8_t*>(front.data()), strlen(front.data()));
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &txHuart)
    {
        messages.pop_front();
        if ( !messages.empty() )
        {
            sendFromBuffer();
        }
    }
}

void writeUart(const char* fmt, ...)
{
    if( !messages.full() )
    {
        std::array<char, MAX_MESSAGE_SIZE> buffer{};

        va_list args;

        va_start(args, fmt);
        vsnprintf(buffer.data(), MAX_MESSAGE_SIZE, fmt, args);
        va_end(args);

        messages.push_back(buffer);
    }
    else
    {
        messages.pop_back();
        messages.push_back(std::array<char, MAX_MESSAGE_SIZE>{"Message queue is full!\n"});
    }
    sendFromBuffer();
}
