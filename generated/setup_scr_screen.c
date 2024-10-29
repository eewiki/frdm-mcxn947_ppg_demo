/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 480, 320);
	lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_chart_1
	ui->screen_chart_1 = lv_chart_create(ui->screen);
	lv_chart_set_type(ui->screen_chart_1, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(ui->screen_chart_1, 1, 1);
	lv_chart_set_point_count(ui->screen_chart_1, 0);
	lv_chart_set_range(ui->screen_chart_1, LV_CHART_AXIS_PRIMARY_Y, -128, 127);
	lv_chart_set_range(ui->screen_chart_1, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	lv_chart_set_zoom_x(ui->screen_chart_1, 256);
	lv_chart_set_zoom_y(ui->screen_chart_1, 256);
	lv_obj_set_style_size(ui->screen_chart_1, 0, LV_PART_INDICATOR);
	ui->screen_chart_1_0 = lv_chart_add_series(ui->screen_chart_1, lv_color_hex(0xffffff), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
#endif
	lv_obj_set_pos(ui->screen_chart_1, 0, 80);
	lv_obj_set_size(ui->screen_chart_1, 480, 60);
	lv_obj_set_scrollbar_mode(ui->screen_chart_1, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_chart_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_chart_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_chart_1, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_chart_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_chart_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_chart_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_1, lv_color_hex(0xa1a1a1), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_chart_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_chart_1, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_chart_1, lv_color_hex(0x151212), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_chart_1, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_chart_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_1, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_1, lv_color_hex(0xe8e8e8), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_1, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write codes screen_chart_2
	ui->screen_chart_2 = lv_chart_create(ui->screen);
	lv_chart_set_type(ui->screen_chart_2, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(ui->screen_chart_2, 1, 1);
	lv_chart_set_point_count(ui->screen_chart_2, 0);
	lv_chart_set_range(ui->screen_chart_2, LV_CHART_AXIS_PRIMARY_Y, -128, 127);
	lv_chart_set_range(ui->screen_chart_2, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	lv_chart_set_zoom_x(ui->screen_chart_2, 256);
	lv_chart_set_zoom_y(ui->screen_chart_2, 256);
	lv_obj_set_style_size(ui->screen_chart_2, 0, LV_PART_INDICATOR);
	ui->screen_chart_2_0 = lv_chart_add_series(ui->screen_chart_2, lv_color_hex(0xf00000), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
#endif
	lv_obj_set_pos(ui->screen_chart_2, 0, 140);
	lv_obj_set_size(ui->screen_chart_2, 480, 60);
	lv_obj_set_scrollbar_mode(ui->screen_chart_2, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_chart_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_chart_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_chart_2, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_chart_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_chart_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_chart_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_2, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_2, lv_color_hex(0xa1a1a1), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_chart_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_chart_2, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_chart_2, lv_color_hex(0x151212), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_chart_2, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_chart_2, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_2, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_2, lv_color_hex(0xe8e8e8), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_2, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write codes screen_chart_3
	ui->screen_chart_3 = lv_chart_create(ui->screen);
	lv_chart_set_type(ui->screen_chart_3, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(ui->screen_chart_3, 1, 1);
	lv_chart_set_point_count(ui->screen_chart_3, 0);
	lv_chart_set_range(ui->screen_chart_3, LV_CHART_AXIS_PRIMARY_Y, -128, 127);
	lv_chart_set_range(ui->screen_chart_3, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	lv_chart_set_zoom_x(ui->screen_chart_3, 256);
	lv_chart_set_zoom_y(ui->screen_chart_3, 256);
	lv_obj_set_style_size(ui->screen_chart_3, 0, LV_PART_INDICATOR);
	ui->screen_chart_3_0 = lv_chart_add_series(ui->screen_chart_3, lv_color_hex(0x00ff00), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
#endif
	lv_obj_set_pos(ui->screen_chart_3, 0, 200);
	lv_obj_set_size(ui->screen_chart_3, 480, 60);
	lv_obj_set_scrollbar_mode(ui->screen_chart_3, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_chart_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_chart_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_chart_3, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_chart_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_chart_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_chart_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_3, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_3, lv_color_hex(0xa1a1a1), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_chart_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_chart_3, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_chart_3, lv_color_hex(0x151212), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_chart_3, &lv_font_montserratMedium_12, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_chart_3, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_3, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_3, lv_color_hex(0xe8e8e8), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_3, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write codes screen_chart_4
	ui->screen_chart_4 = lv_chart_create(ui->screen);
	lv_chart_set_type(ui->screen_chart_4, LV_CHART_TYPE_LINE);
	lv_chart_set_div_line_count(ui->screen_chart_4, 1, 1);
	lv_chart_set_point_count(ui->screen_chart_4, 0);
	lv_chart_set_range(ui->screen_chart_4, LV_CHART_AXIS_PRIMARY_Y, -128, 127);
	lv_chart_set_range(ui->screen_chart_4, LV_CHART_AXIS_SECONDARY_Y, 0, 100);
	lv_chart_set_zoom_x(ui->screen_chart_4, 256);
	lv_chart_set_zoom_y(ui->screen_chart_4, 256);
	lv_obj_set_style_size(ui->screen_chart_4, 0, LV_PART_INDICATOR);
	ui->screen_chart_4_0 = lv_chart_add_series(ui->screen_chart_4, lv_color_hex(0x0000ff), LV_CHART_AXIS_PRIMARY_Y);
#if LV_USE_FREEMASTER == 0
#endif
	lv_obj_set_pos(ui->screen_chart_4, 0, 260);
	lv_obj_set_size(ui->screen_chart_4, 480, 60);
	lv_obj_set_scrollbar_mode(ui->screen_chart_4, LV_SCROLLBAR_MODE_OFF);

	//Write style for screen_chart_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_chart_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_chart_4, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_chart_4, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_width(ui->screen_chart_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_chart_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_4, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_4, lv_color_hex(0xa1a1a1), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_chart_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_chart_4, Part: LV_PART_TICKS, State: LV_STATE_DEFAULT.
	lv_obj_set_style_text_color(ui->screen_chart_4, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_chart_4, &lv_font_montserratMedium_10, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_chart_4, 255, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_width(ui->screen_chart_4, 2, LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_color(ui->screen_chart_4, lv_color_hex(0xffffff), LV_PART_TICKS|LV_STATE_DEFAULT);
	lv_obj_set_style_line_opa(ui->screen_chart_4, 255, LV_PART_TICKS|LV_STATE_DEFAULT);

	//Write codes screen_bar_1
	ui->screen_bar_1 = lv_bar_create(ui->screen);
	lv_obj_set_style_anim_time(ui->screen_bar_1, 100, 0);
	lv_bar_set_mode(ui->screen_bar_1, LV_BAR_MODE_NORMAL);
	lv_bar_set_range(ui->screen_bar_1, 0, 49);
	lv_bar_set_value(ui->screen_bar_1, 0, LV_ANIM_OFF);
	lv_obj_set_pos(ui->screen_bar_1, 0, 0);
	lv_obj_set_size(ui->screen_bar_1, 480, 10);

	//Write style for screen_bar_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_bar_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_bar_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_bar_1, lv_color_hex(0xff2100), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_grad_dir(ui->screen_bar_1, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_bar_1, 0, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_label_1
	ui->screen_label_1 = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_label_1, "Heart Rate:");
	lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_label_1, 30, 25);
	lv_obj_set_size(ui->screen_label_1, 182, 30);

	//Write style for screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_label_1, &lv_font_montserratMedium_28, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_label_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_label_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_2
	ui->screen_label_2 = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_label_2, "60 BPM");
	lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_label_2, 226, 25);
	lv_obj_set_size(ui->screen_label_2, 152, 32);

	//Write style for screen_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_label_2, lv_color_hex(0xff2100), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_label_2, &lv_font_montserratMedium_32, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(ui->screen_label_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_label_2, LV_TEXT_ALIGN_LEFT, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_img_1
	ui->screen_img_1 = lv_img_create(ui->screen);
	lv_obj_add_flag(ui->screen_img_1, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_1, &_heart_scarlet_alpha_40x37);
	lv_img_set_pivot(ui->screen_img_1, 20,16);
	lv_img_set_angle(ui->screen_img_1, 0);
	lv_obj_set_pos(ui->screen_img_1, 418, 22);
	lv_obj_set_size(ui->screen_img_1, 40, 37);

	//Write style for screen_img_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_img_recolor_opa(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_img_opa(ui->screen_img_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_img_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_clip_corner(ui->screen_img_1, true, LV_PART_MAIN|LV_STATE_DEFAULT);

	//The custom code of screen.
	

	//Update current screen layout.
	lv_obj_update_layout(ui->screen);

}
