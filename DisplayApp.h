#ifndef DISPLAY_APP_H
#define DISPLAY_APP_H
#include"Includes.h"
#include "Structs.h"
#include "Defines.h"

static uint32_t debug[] = {5,10,15,20,25,30,35,40,45,50,55,60,5,10,15,20,25,30,35,40,45,50,55,60,5};

// Code Here
void process_task(void *pvParameters);
void uart_read_task(void *pvParameters);
void uart_write_task(void *pvParameters);

void DataStreamingTask(void *pvParameters);
void grl_create_widgets(void);
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p);
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
void grl_display_init(void);
int grl_uart_write(uart_port_t uart_num, const void* src, size_t size);

const char Polling_API[] = {0x13,0x01,0x00,0x01};
const char fw_version[] = "FW_V_1.0.0";

/* Change to your screen resolution */
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;
extern bool g_button_pressed;
extern bool set_dd_pressed;
extern bool get_dd_pressed;

extern char selected_option[64];

// extern lv_obj_t * GrlMainTab;
// extern lv_obj_t * OverViewTab;
// extern lv_obj_t * AnalysisTab;
// extern lv_obj_t * t2 ;
// extern lv_obj_t * t3 ;
// extern lv_obj_t * t4 ;

// extern lv_obj_t * log_features_panel;
// extern lv_obj_t * tap4_panel2_tittle;
// extern lv_obj_t * system_info_panel;
// extern lv_obj_t * fw_ver_num_label;


//Display Sizes
static disp_size_t disp_size;

//Fonts
static const lv_font_t * font_large;
static const lv_font_t * font_normal;

//Styles
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

static lv_obj_t * chart1;
static lv_chart_series_t * ser1;
static lv_chart_series_t * ser2;
static lv_obj_t * chart3;
static lv_obj_t * voltage_chart;
static lv_obj_t * current_chart;
static lv_chart_series_t * volt_axis;
static lv_chart_series_t * curr_axis;
static lv_chart_series_t * pow_axis;

#endif // DISPLAY_APP_H