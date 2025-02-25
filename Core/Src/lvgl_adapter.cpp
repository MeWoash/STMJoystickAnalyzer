#include "lvgl_adapter.h"
#include "lvgl.h"
#include "lcd.h"
#include "main.h"

static_assert(LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565) == LCD_BYTES_PER_PIXEL, "Color format size must equal LCD_BYTES_PER_PIXEL");
static uint8_t buf_1[ LCD_WIDTH * LCD_HEIGHT/4 * LCD_BYTES_PER_PIXEL];
static uint8_t* buf_2 = nullptr;
static lv_display_t * disp;

void my_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    while (lcdIsBusy()){
        static uint32_t busyCycle;
        busyCycle++;
    };

    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    uint32_t pixel_count = w * h;
    uint32_t byte_count = pixel_count * LCD_BYTES_PER_PIXEL;

    uint16_t* buf16 = (uint16_t*)(px_map);
    for(uint32_t nByte = 0; nByte<pixel_count; nByte++)
    {
        buf16[nByte] = __REV16(buf16[nByte]); // little-endian to big-endian
    }
    
    lcdSetWindowCoords(area->x1, area->y1, area->x2, area->y2);
    lcdSendDataBuffer(px_map, byte_count);
}

void initLvgl()
{
    //Initialise LVGL UI library
    lv_init();
    lv_tick_set_cb(HAL_GetTick);

    disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT); /* Basic initialization with horizontal and vertical resolution in pixels */

    lv_display_set_flush_cb(disp, my_flush_cb); /* Set a flush callback to draw to the display */
    lv_display_set_buffers(disp, buf_1, buf_2, sizeof(buf_1) , LV_DISPLAY_RENDER_MODE_PARTIAL); /* Set an initialized buffer */

}