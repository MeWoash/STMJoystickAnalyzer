#include "alt_main.h"
#include "lcd.h"
#include "spi.h"
#include <math.h>
#include "lvgl_adapter.h"
#include "lvgl.h"
#include <stdio.h>

extern UART_HandleTypeDef huart2;
UART_HandleTypeDef& txHuart = huart2;
extern ADC_HandleTypeDef hadc2;


void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi == &hspi2)
    {
        lcdTransferDone();
        lv_display_flush_ready(lv_display_get_default());
    }
}

#define POINTS              20
#define CHAR_SCALE_FACTOR   1

#define ADC_INTERNAL_VOLTAGE    3.3f
#define ADC_RES_BITS            12
#define ADC_UNIQ_VALUES         (1<<ADC_RES_BITS)
#define ADC_MAX_VAL             (ADC_UNIQ_VALUES-1)

/* -------- ADC1 -------- */
lv_obj_t * adc1_chart;
lv_chart_series_t * adc1_series;
int32_t ADC_1_x_points[POINTS];
int32_t ADC_1_y_points[POINTS];

/* -------- Additional UI Elements -------- */

/* Top-left label for HAL_GetTick */
lv_obj_t * label_tick;

/* -------- ADC2 -------- */
lv_obj_t * adc2_chart;
lv_chart_series_t * adc2_series;
int32_t ADC_2_x_points[POINTS];
int32_t ADC_2_y_points[POINTS];

/* -------- point -------- */
lv_obj_t * point_chart;
lv_chart_series_t * point_series;

volatile static uint16_t adc1Values[2];

float adcValToVoltage(uint16_t value)
{
    return ADC_INTERNAL_VOLTAGE * value / ADC_UNIQ_VALUES;
}

void updateADC1array()
{
    HAL_ADC_Start(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
    adc1Values[0] = HAL_ADC_GetValue(&hadc2);
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
    adc1Values[1] = HAL_ADC_GetValue(&hadc2);
}

void setupAdditionalUI(void)
{
    lv_obj_t * scr = lv_scr_act();
    
    /* Set entire screen background to dark */
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0);

    /* Create top-left label */
    label_tick = lv_label_create(scr);
    lv_obj_set_style_text_color(label_tick, lv_color_white(), 0);
    // lv_obj_set_pos(label_tick, 0, 0);
    lv_obj_align(label_tick, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_label_set_text(label_tick, "Tick: 0");

    /* -------- ADCIN1 ----------*/
    adc1_chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(adc1_chart, LCD_WIDTH/2, LCD_HEIGHT/2);
    lv_obj_align(adc1_chart, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_chart_set_type(adc1_chart, LV_CHART_TYPE_LINE);

    /* Apply dark theme to adc1 chart */
    lv_obj_set_style_bg_color(adc1_chart, lv_color_black(), 0);
    lv_obj_set_style_border_color(adc1_chart, lv_color_white(), 0);

    adc1_series = lv_chart_add_series(adc1_chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(adc1_chart, POINTS);
    lv_chart_set_ext_x_array(adc1_chart, adc1_series, ADC_1_x_points);
    lv_chart_set_ext_y_array(adc1_chart, adc1_series, ADC_1_y_points);
    lv_chart_set_range(adc1_chart, LV_CHART_AXIS_PRIMARY_Y,  0, ADC_MAX_VAL);
    // lv_chart_set_update_mode(adc1_chart, LV_CHART_UPDATE_MODE_CIRCULAR);

    /* Remove grid lines from adc1 chart */
    lv_chart_set_div_line_count(adc1_chart, 0, 0);

    /* Disable point markers and reduce line thickness */
    lv_obj_set_style_width(adc1_chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_height(adc1_chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(adc1_chart, 1, LV_PART_ITEMS);

    /* -------- ADCIN2 ----------*/
    adc2_chart = lv_chart_create(scr);
    lv_obj_set_size(adc2_chart, LCD_WIDTH/2, LCD_HEIGHT/2);
    lv_obj_align(adc2_chart, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(adc2_chart, lv_color_black(), 0);
    lv_obj_set_style_border_color(adc2_chart, lv_color_white(), 0);

    lv_chart_set_type(adc2_chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(adc2_chart, POINTS);
    adc2_series = lv_chart_add_series(adc2_chart, lv_palette_main(LV_PALETTE_CYAN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_x_array(adc2_chart, adc2_series, ADC_2_x_points);
    lv_chart_set_ext_y_array(adc2_chart, adc2_series, ADC_2_y_points);
    lv_chart_set_range(adc2_chart, LV_CHART_AXIS_PRIMARY_Y, 0, ADC_MAX_VAL);
    // lv_chart_set_update_mode(adc2_chart, LV_CHART_UPDATE_MODE_CIRCULAR);

    /* Remove grid lines from adc2 chart */
    lv_chart_set_div_line_count(adc2_chart, 0, 0);

    /* Disable point markers and reduce line thickness */
    lv_obj_set_style_width(adc2_chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_height(adc2_chart, 0, LV_PART_INDICATOR);
    lv_obj_set_style_line_width(adc2_chart, 1, LV_PART_ITEMS);

    /* -------- SCATTER ----------*/
    point_chart = lv_chart_create(scr);
    lv_obj_set_size(point_chart, LCD_WIDTH/2, LCD_HEIGHT/2);
    lv_obj_align(point_chart, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(point_chart, lv_color_black(), 0);
    lv_obj_set_style_border_color(point_chart, lv_color_white(), 0);

    /* Set chart type to "scatter" */
    lv_chart_set_type(point_chart, LV_CHART_TYPE_SCATTER);
    lv_chart_set_point_count(point_chart, 2);
    point_series = lv_chart_add_series(point_chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_range(point_chart, LV_CHART_AXIS_PRIMARY_Y, 0, ADC_MAX_VAL);
    lv_chart_set_range(point_chart, LV_CHART_AXIS_PRIMARY_X, 0, ADC_MAX_VAL);

    /* Remove grid lines from point chart */
    lv_chart_set_div_line_count(point_chart, 0, 0);
    lv_obj_set_style_line_width(point_chart, 0, LV_PART_ITEMS);

    lv_obj_set_style_width(point_chart, 10, LV_PART_INDICATOR);
    lv_obj_set_style_height(point_chart, 10, LV_PART_INDICATOR);
    
}

/* -------- Setup and Loop -------- */
void setup()
{
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    // HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc1Values, 2);

    initLvgl();
    lcdInit();          // Create existing sinus chart
    setupAdditionalUI();  // Add the label and ADC chart with dark theme

    writeUart("Setup complete!\n");
}

void loop()
{
    updateADC1array();

    lv_chart_set_next_value(adc1_chart, adc1_series, adc1Values[0]);
    lv_chart_set_next_value(adc2_chart, adc2_series, adc1Values[1]);
    lv_chart_set_next_value2(point_chart, point_series, static_cast<int32_t>(adc1Values[0]), static_cast<int32_t>(ADC_MAX_VAL - adc1Values[1]));

    uint32_t v1 = adcValToVoltage(adc1Values[0])*1000;
    uint32_t v2 = adcValToVoltage(adc1Values[1])*1000;
    lv_label_set_text_fmt(label_tick, "v1: %4lumV\nv2: %4lumV", v1, v2);

    lv_timer_handler();
}
