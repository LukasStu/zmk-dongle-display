/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/modifiers.h"
#include "widgets/bongo_cat.h"
#include "widgets/layer_status.h"
#include "widgets/output_status.h"
#include "widgets/hid_indicators.h"
#include "widgets/wpm_status.h"

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define FRAME_OFFSET_PX 1
#define FRAME_BORDER_WIDTH_PX 2
#define TITLE_LINE_COUNT 5
#define TITLE_LINE_THICKNESS_PX 2
#define TITLE_LINE_GAP_PX 3
#define TITLE_TO_MAIN_GAP_PX 2
#define MAIN_WINDOW_BORDER_WIDTH_PX 2

static struct zmk_widget_output_status output_status_widget;

#if IS_ENABLED(CONFIG_ZMK_BATTERY)
static struct zmk_widget_dongle_battery_status dongle_battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
static struct zmk_widget_modifiers modifiers_widget;
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
static struct zmk_widget_hid_indicators hid_indicators_widget;
#endif

#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
static struct zmk_widget_bongo_cat bongo_cat_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

lv_style_t global_style;

static lv_coord_t clamp_coord(lv_coord_t value, lv_coord_t minimum) {
    return value < minimum ? minimum : value;
}

static void style_border_box(lv_obj_t *obj, lv_coord_t border_width) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_color(obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, border_width, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

static void style_solid_fill(lv_obj_t *obj) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    lv_obj_t *main_window;

    screen = lv_obj_create(NULL);

    lv_style_init(&global_style);
    lv_style_set_bg_color(&global_style, lv_color_white());
    lv_style_set_bg_opa(&global_style, LV_OPA_COVER);
    lv_style_set_text_color(&global_style, lv_color_black());
    lv_style_set_text_font(&global_style, &lv_font_unscii_8);
    lv_style_set_text_letter_space(&global_style, 1);
    lv_style_set_text_line_space(&global_style, 1);
    lv_obj_add_style(screen, &global_style, LV_PART_MAIN);

    const lv_coord_t screen_width = lv_obj_get_width(screen);
    const lv_coord_t screen_height = lv_obj_get_height(screen);

    const lv_coord_t frame_x = FRAME_OFFSET_PX;
    const lv_coord_t frame_y = FRAME_OFFSET_PX;
    const lv_coord_t frame_width = clamp_coord(screen_width - (FRAME_OFFSET_PX * 2), 1);
    const lv_coord_t frame_height = clamp_coord(screen_height - (FRAME_OFFSET_PX * 2), 1);

    const lv_coord_t inner_x = frame_x + FRAME_BORDER_WIDTH_PX;
    const lv_coord_t inner_y = frame_y + FRAME_BORDER_WIDTH_PX;
    const lv_coord_t inner_width = clamp_coord(frame_width - (FRAME_BORDER_WIDTH_PX * 2), 1);
    const lv_coord_t inner_height = clamp_coord(frame_height - (FRAME_BORDER_WIDTH_PX * 2), 1);

    const lv_coord_t title_bar_height =
        (TITLE_LINE_COUNT * TITLE_LINE_THICKNESS_PX) + ((TITLE_LINE_COUNT - 1) * TITLE_LINE_GAP_PX);

    lv_obj_t *frame = lv_obj_create(screen);
    style_border_box(frame, FRAME_BORDER_WIDTH_PX);
    lv_obj_set_pos(frame, frame_x, frame_y);
    lv_obj_set_size(frame, frame_width, frame_height);

    for (int i = 0; i < TITLE_LINE_COUNT; i++) {
        lv_obj_t *line = lv_obj_create(screen);
        style_solid_fill(line);
        lv_obj_set_pos(line, inner_x,
                       inner_y + (i * (TITLE_LINE_THICKNESS_PX + TITLE_LINE_GAP_PX)));
        lv_obj_set_size(line, inner_width, TITLE_LINE_THICKNESS_PX);
    }

    const lv_coord_t main_window_y = inner_y + title_bar_height + TITLE_TO_MAIN_GAP_PX;
    const lv_coord_t main_window_height =
        clamp_coord(inner_height - title_bar_height - TITLE_TO_MAIN_GAP_PX, 1);

    main_window = lv_obj_create(screen);
    style_border_box(main_window, MAIN_WINDOW_BORDER_WIDTH_PX);
    lv_obj_set_style_pad_all(main_window, MAIN_WINDOW_BORDER_WIDTH_PX, LV_PART_MAIN);
    lv_obj_set_pos(main_window, inner_x, main_window_y);
    lv_obj_set_size(main_window, inner_width, main_window_height);
    
    zmk_widget_output_status_init(&output_status_widget, main_window);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
    zmk_widget_wpm_status_init(&wpm_status_widget, main_window);
    lv_obj_align_to(zmk_widget_wpm_status_obj(&wpm_status_widget), zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_OUT_RIGHT_MID, 7, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    zmk_widget_bongo_cat_init(&bongo_cat_widget, main_window);
    lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, -7);
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
    zmk_widget_modifiers_init(&modifiers_widget, main_window);
    lv_obj_align(zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
    zmk_widget_hid_indicators_init(&hid_indicators_widget, main_window);
    lv_obj_align_to(zmk_widget_hid_indicators_obj(&hid_indicators_widget), zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_OUT_TOP_LEFT, 0, -2);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
    zmk_widget_layer_status_init(&layer_status_widget, main_window);
#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    lv_obj_align_to(zmk_widget_layer_status_obj(&layer_status_widget), zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 5);
#else
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, -3);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_BATTERY)
    zmk_widget_dongle_battery_status_init(&dongle_battery_status_widget, main_window);
    lv_obj_align(zmk_widget_dongle_battery_status_obj(&dongle_battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

    return screen;
}
