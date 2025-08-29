// Include the main display functions so we don't have to initialize the display
// again
#include "hlc_tft_display/hlc_tft_display.h"
#include <stdint.h>

#include "graphics/modifiers/caps_word_enabled.qgf.h"
#include "graphics/modifiers/caps_word_shift_pressed.qgf.h"
#include "graphics/modifiers/command_pressed.qgf.h"
#include "graphics/modifiers/command_unpressed.qgf.h"
#include "graphics/modifiers/control_pressed.qgf.h"
#include "graphics/modifiers/control_unpressed.qgf.h"
#include "graphics/modifiers/fallback.qgf.h"
#include "graphics/modifiers/option_pressed.qgf.h"
#include "graphics/modifiers/option_unpressed.qgf.h"
#include "graphics/modifiers/shift_pressed.qgf.h"
#include "graphics/modifiers/shift_unpressed.qgf.h"

#include "graphics/layers/base_layer_icon_colored.qgf.h"
#include "graphics/layers/base_layer_text_colored.qgf.h"
#include "graphics/layers/lower_layer_icon_colored.qgf.h"
#include "graphics/layers/lower_layer_text_colored.qgf.h"
#include "graphics/layers/nav3d_layer_icon_colored.qgf.h"
#include "graphics/layers/nav3d_layer_text_colored.qgf.h"
#include "graphics/layers/raise_layer_icon_colored.qgf.h"
#include "graphics/layers/raise_layer_text_colored.qgf.h"
#include "graphics/layers/undefined_layer_icon.qgf.h"
#include "graphics/layers/undefined_layer_text.qgf.h"

#include "keymap.h"

typedef struct gfx_position {
    uint16_t x;
    uint16_t y;
} gfx_position;

typedef struct gfx_layer_definition {
    painter_image_handle_t icon;
    painter_image_handle_t text;
} gfx_layer_definition;

typedef struct gfx_modifier_definition {
    painter_image_handle_t pressed;
    painter_image_handle_t unpressed;
    gfx_position           position;
} gfx_modifier_definition;

// Setup painter devices
extern painter_device_t lcd;
extern painter_device_t lcd_surface;

static gfx_layer_definition layer_fallback;
static gfx_layer_definition layer_mapping[_COUNT] = {0};
static gfx_position         layer_icon_position   = {0};
static gfx_position         layer_text_position   = {0};

static gfx_modifier_definition shift;
static gfx_modifier_definition control;
static gfx_modifier_definition option;
static gfx_modifier_definition command;
static gfx_modifier_definition caps_word;
static uint16_t                modifiers_width  = 0;
static uint16_t                modifiers_height = 0;

static bool can_draw = false;

// This function is ran on bootup of the keyboard
bool module_post_init_user(void) {
    // We start with the undefined icon in case there are too may image loaded so
    // at least we can draw that something is wrong
    painter_image_handle_t layer_fallback_icon = qp_load_image_mem(gfx_undefined_layer_icon);
    painter_image_handle_t layer_fallback_text = qp_load_image_mem(gfx_undefined_layer_text);
    painter_image_handle_t modifier_fallback   = qp_load_image_mem(gfx_fallback);
    if (layer_mapping[_UNDEFINED].icon == NULL || layer_mapping[_UNDEFINED].text == NULL || modifier_fallback == NULL) {
        return false;
    }

    can_draw = true;

    uint16_t layer_icon_width  = layer_mapping[_UNDEFINED].icon->width;
    uint16_t layer_icon_height = layer_mapping[_UNDEFINED].icon->height;
    uint16_t layer_text_width  = layer_mapping[_UNDEFINED].text->width;
    uint16_t layer_text_height = layer_mapping[_UNDEFINED].text->height;
    modifiers_width            = modifier_fallback->width;
    modifiers_height           = modifier_fallback->height;

    // The layers are displayed icon first then text second
    //               --------
    //               | Icon |
    //             --+------+--
    //             |   Text   |
    //             +----------+
    layer_icon_position = (gfx_position){
        .x = (uint16_t)((LCD_WIDTH - layer_icon_width) / 2),
        .y = (uint16_t)(((LCD_HEIGHT / 2) - layer_icon_height) / 2),
    };
    layer_text_position = (gfx_position){
        .x = (uint16_t)((LCD_WIDTH - layer_text_width) / 2),
        .y = (uint16_t)((LCD_HEIGHT - layer_text_height) / 2),
    };
    layer_fallback = (gfx_layer_definition){
        .icon = layer_fallback_icon,
        .text = layer_fallback_text,
    };

    layer_mapping[_BASE] = (gfx_layer_definition){
        .icon = qp_load_image_mem(gfx_base_layer_icon_colored),
        .text = qp_load_image_mem(gfx_base_layer_text_colored),
    };

    layer_mapping[_BASE_NO_HRM] = (gfx_layer_definition){
        .icon = layer_mapping[_BASE].icon,
        .text = layer_mapping[_BASE].text,
    };

    layer_mapping[_LOWER] = (gfx_layer_definition){
        .icon = qp_load_image_mem(gfx_lower_layer_icon_colored),
        .text = qp_load_image_mem(gfx_lower_layer_text_colored),
    };

    layer_mapping[_RAISE] = (gfx_layer_definition){
        .icon = qp_load_image_mem(gfx_raise_layer_icon_colored),
        .text = qp_load_image_mem(gfx_raise_layer_text_colored),
    };

    layer_mapping[_NAV_3D] = (gfx_layer_definition){
        .icon = qp_load_image_mem(gfx_nav3d_layer_icon_colored),
        .text = qp_load_image_mem(gfx_nav3d_layer_text_colored),
    };

    for (int i = 0; i < _COUNT; ++i) {
        if (layer_mapping[i].icon == NULL || layer_mapping[i].icon->width != layer_icon_width ||
            layer_mapping[i].icon->height != layer_icon_height) {
            layer_mapping[i].icon = layer_fallback_icon;
        }
        if (layer_mapping[i].text == NULL || layer_mapping[i].text->width != layer_text_width ||
            layer_mapping[i].text->height != layer_text_height) {
            layer_mapping[i].text = layer_fallback_text;
        }
    }

    painter_image_handle_t control_pressed   = qp_load_image_mem(gfx_control_pressed);
    painter_image_handle_t control_unpressed = qp_load_image_mem(gfx_control_unpressed);
    painter_image_handle_t option_pressed    = qp_load_image_mem(gfx_option_pressed);
    painter_image_handle_t option_unpressed  = qp_load_image_mem(gfx_option_unpressed);
    painter_image_handle_t command_pressed   = qp_load_image_mem(gfx_command_pressed);
    painter_image_handle_t command_unpressed = qp_load_image_mem(gfx_command_unpressed);
    painter_image_handle_t shift_pressed     = qp_load_image_mem(gfx_shift_pressed);
    painter_image_handle_t shift_unpressed   = qp_load_image_mem(gfx_shift_unpressed);
    painter_image_handle_t caps_pressed      = qp_load_image_mem(gfx_caps_word_shift_pressed);
    painter_image_handle_t caps_unpressed    = qp_load_image_mem(gfx_caps_word_enabled);

    {
        painter_image_handle_t *modifier_icons[] = {
            &control_pressed,   &control_unpressed, &option_pressed,  &option_unpressed, &command_pressed,
            &command_unpressed, &shift_pressed,     &shift_unpressed, &caps_pressed,     &caps_unpressed};

        bool modifier_fallback_needed = false;
        for (int i = 0; i < sizeof(modifier_icons) / sizeof(modifier_icon[0]; ++i)) {
            if (*modifier_icon[i] == NULL || (*modifier_icon[i])->width != modifiers_width ||
                (*modifier_icon[i])->height != modifiers_height) {
                *modifier_icon[i]        = modifier_fallback;
                modifier_fallback_needed = true;
            }
        }
        if (!modifier_fallback_needed) {
            qp_close_imaget(modifier_fallback);
        }
    }

    // The goal is to position the keys on screen like follows, having them
    // centered on the y axis and having the line between the first and second row
    // fall at the 3/4 of thes screen:
    //        ----------------------
    //        | Ctl  |  Opt | Cmd  |
    //        +------+------+------+
    //               | Shft |
    //               +------+
    // The first row disappears if we disable home row mod, and Shift is rendered
    // plain if we enable Caps Word
    const uint16_t spacing         = 16;
    const uint16_t first_row_y     = LCD_HEIGHT - ((LCD_HEIGHT / 2 + modifiers_height) / 2);
    const uint16_t second_row_y    = LCD_HEIGHT - ((LCD_HEIGHT / 2 - modifiers_height - spacing) / 2);
    const uint16_t center_column_x = (LCD_WIDTH - modifiers_width) / 2;
    const uint16_t left_column_x   = (uint16_t)((LCD_WIDTH - modifiers_width * 3 - spacing) / 2);
    const uint16_t right_column_x  = (uint16_t)((LCD_WIDTH + modifiers_width + spacing) / 2);

    control = (gfx_modifier_definition){
        .pressed   = control_pressed,
        .unpressed = control_unpressed,
        .position =
            (gfx_position){
                .x = left_column_x,
                .y = first_row_y,
            },
    };
    option = (gfx_modifier_definition){
        .pressed   = option_pressed,
        .unpressed = option_unpressed,
        .position =
            (gfx_position){
                .x = center_column_x,
                .y = first_row_y,
            },
    };
    command = (gfx_modifier_definition){
        .pressed   = command_pressed,
        .unpressed = command_unpressed,
        .position =
            (gfx_position){
                .x = right_column_x,
                .y = first_row_y,
            },
    };
    shift = (gfx_modifier_definition){
        .pressed   = shift_pressed,
        .unpressed = shift_unpressed,
        .position =
            (gfx_position){
                .x = center_column_x,
                .y = second_row_y,
            },
    };
    caps_word = (gfx_modifier_definition){
        .pressed   = caps_pressed,
        .unpressed = caps_unpressed,
        .position =
            (gfx_position){
                .x = center_column_x,
                .y = second_row_y,
            },
    };
    return false;
}

// This function runs after every matrix scan
bool display_module_housekeeping_task_user(bool second_display) {
    static bool    layer_initialized = false;
    static uint8_t last_layer        = 0;

    static bool    modifiers_initialized = false;
    static uint8_t last_modifiers        = 0;
    static bool    last_caps_word_on     = false;

    if (!second_display && can_draw) {
        uint8_t current_layer       = get_highest_layer(layer_state | default_layer_state);
        bool    in_or_out_of_no_hrm = false;
        if (!layer_initialized || current_layer != last_layer) {
            in_or_out_of_no_hrm         = (last_layer == _BASE_NO_HRM) || (current_layer == _BASE_NO_HRM);
            gfx_layer_definition *layer = current_layer < _COUNT ? &layer_mapping[current_layer] : layer_fallback;
            qp_drawimage(lcd_surface, layer_icon_position.x, layer_icon_position.y, layer->icon);
            qp_drawimage(lcd_surface, layer_text_position.x, layer_text_position.y, layer->text);
            layer_initialized = true;
            last_layer        = current_layer;
        }

        uint8_t current_modifiers    = get_mods();
        bool    current_caps_word_on = is_caps_word_on();
        bool    caps_word_changed    = current_caps_word_on != last_caps_word_on;
        if (!modifiers_initialized || in_or_out_of_no_hrm || current_modifiers != last_modifiers || caps_word_changed) {
            uint8_t changed_modifiers = last_modifiers ^ current_modifiers;
            if (in_or_out_of_no_hrm && current_layer == _BASE_NO_HRM) {
                qp_rect(lcd_surface, control.position.x, control.position.y, modifiers_width - 1, modifiers_height - 1,
                        HSV_BLACK, true);
                qp_rect(lcd_surface, option.position.x, option.position.y, modifiers_width - 1, modifiers_height - 1,
                        HSV_BLACK, true);
                qp_rect(lcd_surface, command.position.x, command.position.y, modifiers_width - 1, modifiers_height - 1,
                        HSV_BLACK, true);
            } else {
                bool force_update = !modifiers_initialized || in_or_out_of_no_hrm;
                if (force_update || (changed_modifiers & MOD_MASK_CTRL)) {
                    qp_drawimage(lcd_surface, control.position.x, control.position.y,
                                 current_modifiers & MOD_MASK_CTRL ? control.pressed : control.unpressed);
                }
                if (force_update || (changed_modifiers & MOD_MASK_ALT)) {
                    qp_drawimage(lcd_surface, option.position.x, option.position.y,
                                 current_modifiers & MOD_MASK_ALT ? option.pressed : option.unpressed);
                }
                if (force_update || (changed_modifiers & MOD_MASK_GUI)) {
                    qp_drawimage(lcd_surface, command.position.x, command.position.y,
                                 current_modifiers & MOD_MASK_GUI ? command.pressed : command.unpressed);
                }
            }

            if (!modifiers_initialized || caps_word_changed || (changed_modifiers & MOD_MASK_SHIFT)) {
                gfx_modifier_definition *modifier = current_caps_word_on ? &caps_word : &shift;
                qp_drawimage(lcd_surface, modifier->position.x, modifier->position.y,
                             current_modifiers & MOD_MASK_SHIFT ? modifier->pressed : modifier->unpressed);
            }

            modifiers_initialized = true;
            last_modifiers        = current_modifiers;
            last_caps_word_on     = current_caps_word_on;
        }

        // Move surface to lcd
        qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
        qp_flush(lcd);
    }

    return false;
}
