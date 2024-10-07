// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.2
// LVGL version: 8.3.6
// Project name: led_test_light

#ifndef _LED_TEST_LIGHT_UI_H
#define _LED_TEST_LIGHT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
#if __has_include("lvgl.h")
#include "lvgl.h"
#elif __has_include("lvgl/lvgl.h")
#include "lvgl/lvgl.h"
#else
#include "lvgl.h"
#endif
#else
#include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "components/ui_comp.h"
#include "components/ui_comp_hook.h"
#include "ui_events.h"

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
void ui_event_Screen1(lv_event_t * e);
extern lv_obj_t * ui_Screen1;
extern lv_obj_t * ui_Image1;
// SCREEN: ui_Screen2
void ui_Screen2_screen_init(void);
void ui_event_Screen2(lv_event_t * e);
extern lv_obj_t * ui_Screen2;
extern lv_obj_t * ui_Label2;
extern lv_obj_t * ui_Label4;
extern lv_obj_t * ui_Spinner1;
// SCREEN: ui_Screen3
void ui_Screen3_screen_init(void);
void ui_event_Screen3(lv_event_t * e);
extern lv_obj_t * ui_Screen3;
extern lv_obj_t * ui_Label5;
// SCREEN: ui_Screen4
void ui_Screen4_screen_init(void);
void ui_event_Screen4(lv_event_t * e);
extern lv_obj_t * ui_Screen4;
extern lv_obj_t * ui_Label6;
extern lv_obj_t * ui____initial_actions0;


LV_IMG_DECLARE(ui_img_xinhuo1_png);    // assets/xinhuo1.png
LV_IMG_DECLARE(ui_img_bat16_png);    // assets/bat16.png
LV_IMG_DECLARE(ui_img_wenshidu32_png);    // assets/wenshidu32.png



LV_FONT_DECLARE(ui_font_chinese28);
LV_FONT_DECLARE(ui_font_chineseLarge);
LV_FONT_DECLARE(ui_font_chineseMicro);
LV_FONT_DECLARE(ui_font_chineseMin);
LV_FONT_DECLARE(ui_font_chineseNano);
LV_FONT_DECLARE(ui_font_WSChinese16ALL);
LV_FONT_DECLARE(ui_font_WSChinese32ALL);



void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
