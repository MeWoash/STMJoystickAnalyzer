#pragma once
#include "stdint.h"
#include <array>


#define LCD_WIDTH	160
#define LCD_HEIGHT	128

#define LCD_BYTES_PER_PIXEL	2

#define BLACK			0x0000
#define RED			    0x00f8
#define GREEN			0xe007
#define BLUE			0x1f00
#define YELLOW			0xe0ff
#define MAGENTA			0x1ff8
#define CYAN			0xff07
#define WHITE			0xffff


void lcdInit();

void lcdSetWindowCoords(int x1, int y1, int x2, int y2);
void lcdSetWindowSize(int x, int y, int width, int height);

void lcdPutPixelInBuffer(uint16_t* frameBuffer , int x, int y, uint16_t color);
void lcdSendDataBuffer(uint8_t* frameBuffer, uint16_t sizeInBytes);
void lcdTransferDone();
bool lcdIsBusy();

std::array<uint16_t, LCD_WIDTH * LCD_HEIGHT>& getFrameBuffer();
