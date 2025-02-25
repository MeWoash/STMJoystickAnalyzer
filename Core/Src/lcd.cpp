#include "lcd.h"
#include "spi.h"
#include <type_traits>

#define CMD(x) (x | 0x100)

#define LCD_OFFSET_X  1
#define LCD_OFFSET_Y  2

enum Cmd : uint8_t
{
    ST7735S_SLPOUT	 = 0x11,
    ST7735S_DISPOFF	 = 0x28,
    ST7735S_DISPON	 = 0x29,
    ST7735S_CASET	 = 0x2a,
    ST7735S_RASET	 = 0x2b,
    ST7735S_RAMWR	 = 0x2c,
    ST7735S_MADCTL	 = 0x36,
    ST7735S_COLMOD	 = 0x3a,
    ST7735S_FRMCTR1	 = 0xb1,
    ST7735S_FRMCTR2	 = 0xb2,
    ST7735S_FRMCTR3	 = 0xb3,
    ST7735S_INVCTR	 = 0xb4,
    ST7735S_PWCTR1	 = 0xc0,
    ST7735S_PWCTR2	 = 0xc1,
    ST7735S_PWCTR3	 = 0xc2,
    ST7735S_PWCTR4	 = 0xc3,
    ST7735S_PWCTR5	 = 0xc4,
    ST7735S_VMCTR1	 = 0xc5,
    ST7735S_GAMCTRP1 = 0xe0,
    ST7735S_GAMCTRN1 = 0xe1
};

constexpr auto LCD_ON_RESET_TIME = 100;
constexpr auto LCD_POST_RESET_TIME = 100;

constexpr uint16_t init_table[] =
{
  CMD(Cmd::ST7735S_FRMCTR1),    0x01, 0x2c, 0x2d,
  CMD(Cmd::ST7735S_FRMCTR2),    0x01, 0x2c, 0x2d,
  CMD(Cmd::ST7735S_FRMCTR3),    0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d,
  CMD(Cmd::ST7735S_INVCTR),     0x07,
  CMD(Cmd::ST7735S_PWCTR1),     0xa2, 0x02, 0x84,
  CMD(Cmd::ST7735S_PWCTR2),     0xc5,
  CMD(Cmd::ST7735S_PWCTR3),     0x0a, 0x00,
  CMD(Cmd::ST7735S_PWCTR4),     0x8a, 0x2a,
  CMD(Cmd::ST7735S_PWCTR5),     0x8a, 0xee,
  CMD(Cmd::ST7735S_VMCTR1),     0x0e,
  CMD(Cmd::ST7735S_GAMCTRP1),   0x0f, 0x1a, 0x0f, 0x18, 0x2f, 0x28, 0x20, 0x22,
                                0x1f, 0x1b, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10,
  CMD(Cmd::ST7735S_GAMCTRN1),   0x0f, 0x1b, 0x0f, 0x17, 0x33, 0x2c, 0x29, 0x2e,
                                0x30, 0x30, 0x39, 0x3f, 0x00, 0x07, 0x03, 0x10,
  CMD(0xf0),                    0x01,
  CMD(0xf6),                    0x00,
  CMD(Cmd::ST7735S_COLMOD),     0x05,
  CMD(Cmd::ST7735S_MADCTL),     0xa0,
};

static void lcdSendCmd(const uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET); // enable command mode
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // enable chip select
	HAL_SPI_Transmit(&hspi2, &cmd, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); // disable chip select
}

template <typename T>
void lcdSendData(const T data)
{
    static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");
    static_assert(sizeof(T) <= 4, "T is max 32 bits");

    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // enable data mode
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // enable chip select

    HAL_SPI_Transmit(&hspi2,
                    reinterpret_cast<const uint8_t*>(&data),
                    sizeof(T),
                    HAL_MAX_DELAY);
    
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   //disable chip select
}

template <>
void lcdSendData<uint8_t>(const uint8_t data)
{
    HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // enable data mode
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // enable chip select
    HAL_SPI_Transmit(&hspi2, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // disable chip select
}

static void lcdSendGeneric(const uint16_t value)
{
    if (value & 0x100)
    {
	    lcdSendCmd(value);
	}
    else
    {
		lcdSendData(value);
	}
}

// template <typename T, size_t N>
// void lcdSendDataArray(T (&dataArray)[N])
// {
//     static_assert(std::is_unsigned<T>::value, "T must be an unsigned type");
//     static_assert(sizeof(T) <= 4, "T is max 32 bits");

//     HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);   // enable data mode
//     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); // enable chip select
//     HAL_SPI_Transmit(&hspi2, reinterpret_cast<const uint8_t*>(dataArray), N * sizeof(T), HAL_MAX_DELAY);
//     HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);   // disable chip select
// }

void lcdInit()
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(LCD_ON_RESET_TIME);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(LCD_POST_RESET_TIME);
    for (const auto& data : init_table) {
        lcdSendGeneric(data);
    }
    HAL_Delay(200);
    lcdSendCmd(ST7735S_SLPOUT);
    HAL_Delay(120);
    lcdSendCmd(ST7735S_DISPON);
}

void lcdSetWindowCoords(int x1, int y1, int x2, int y2)
{
  lcdSendCmd(ST7735S_CASET);
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_X + x1));
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_X + x2));
  
  lcdSendCmd(ST7735S_RASET);
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_Y + y1));
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_Y + y2));
}

void lcdSetWindowSize(int x, int y, int width, int height)
{
  lcdSendCmd(ST7735S_CASET);
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_X + x));
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_X + x + width - 1));
  
  lcdSendCmd(ST7735S_RASET);
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_Y + y));
  lcdSendData<uint16_t>(__REV16(LCD_OFFSET_Y + y + height- 1));
}

void lcdPutPixelInBuffer(uint16_t* frameBuffer , int x, int y, uint16_t color)
{
    frameBuffer[x + y * LCD_WIDTH] = color;
}

void lcdSendDataBuffer(uint8_t* frameBuffer, uint16_t sizeInBytes)
{
	lcdSendCmd(ST7735S_RAMWR);
	HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit_DMA(&hspi2, frameBuffer, sizeInBytes);
}

void lcdTransferDone(void)
{
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

bool lcdIsBusy(void)
{
	if (HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_BUSY)
		return true;
	else
		return false;
}

