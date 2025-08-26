// Include the main display functions so we don't have to initialize the display again
#include <stdint.h>
#include "hlc_tft_display/hlc_tft_display.h"

#include "graphics/modifiers/caps_word_enabled.qgf.h"
#include "graphics/modifiers/caps_word_shift_pressed.qgf.h"
#include "graphics/modifiers/command_pressed.qgf.h"
#include "graphics/modifiers/command_unpressed.qgf.h"
#include "graphics/modifiers/control_pressed.qgf.h"
#include "graphics/modifiers/control_unpressed.qgf.h"
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

typedef struct layer_gfx_definition {
    painter_image_handle_t icon;
    painter_image_handle_t text;
} layer_gfx_definition;

typedef struct modifier_gfx_definition {
    painter_image_handle_t pressed;
    painter_image_handle_t unpressed;
} modifier_gfx_definition;

// Setup painter devices
extern painter_device_t lcd;
extern painter_device_t lcd_surface;

static layer_gfx_definition layer_mapping[_COUNT] = {0};

static modifier_gfx_definition shift;
static modifier_gfx_definition control;
static modifier_gfx_definition option;
static modifier_gfx_definition command;
static modifier_gfx_definition caps_word;

// This function is ran on bootup of the keyboard
bool module_post_init_user(void) {
    painter_image_handle_t base_layer_icon = qp_load_image_mem(gfx_base_layer_icon_colored);
    painter_image_handle_t base_layer_text = qp_load_image_mem(gfx_base_layer_text_colored);
    layer_mapping[_BASE].icon              = base_layer_icon;
    layer_mapping[_BASE].text              = base_layer_text;
    layer_mapping[_BASE_NO_HRM].icon       = base_layer_icon;
    layer_mapping[_BASE_NO_HRM].text       = base_layer_text;
    layer_mapping[_LOWER].icon             = qp_load_image_mem(gfx_lower_layer_icon_colored);
    layer_mapping[_LOWER].text             = qp_load_image_mem(gfx_lower_layer_text_colored);
    layer_mapping[_RAISE].icon             = qp_load_image_mem(gfx_raise_layer_icon_colored);
    layer_mapping[_RAISE].text             = qp_load_image_mem(gfx_raise_layer_text_colored);
    layer_mapping[_NAV_3D].icon            = qp_load_image_mem(gfx_nav3d_layer_icon_colored);
    layer_mapping[_NAV_3D].text            = qp_load_image_mem(gfx_nav3d_layer_text_colored);
    layer_mapping[_UNDEFINED].icon         = qp_load_image_mem(gfx_undefined_layer_icon);
    layer_mapping[_UNDEFINED].text         = qp_load_image_mem(gfx_undefined_layer_text);

    shift.pressed       = qp_load_image_mem(gfx_shift_pressed);
    shift.unpressed     = qp_load_image_mem(gfx_shift_unpressed);
    control.pressed     = qp_load_image_mem(gfx_control_pressed);
    control.unpressed   = qp_load_image_mem(gfx_control_unpressed);
    option.pressed      = qp_load_image_mem(gfx_option_pressed);
    option.unpressed    = qp_load_image_mem(gfx_option_unpressed);
    command.pressed     = qp_load_image_mem(gfx_command_pressed);
    command.unpressed   = qp_load_image_mem(gfx_command_unpressed);
    caps_word.pressed   = qp_load_image_mem(gfx_caps_word_shift_pressed);
    caps_word.unpressed = qp_load_image_mem(gfx_caps_word_enabled);

    return false;
}

// void draw_modifier(painter_image_handle_t status, uint16_t x, uint16_t y, bool force) {
//     const uint16_t width  = status->width;
//     const uint16_t height = status->height;
//     const uint16_t x      = (LCD_WIDTH - width * 3) / 2;
//     const uint16_t y      = LCD_HEIGHT - (((LCD_HEIGHT >> 1) - height) >> 1);
//     if (current_layer == _BASE_NO_HRM) {
//         qp_rect(lcd_surface, x, y, width - 1, height - 1, HSV_BLACK, true);
//     } else {
//         qp_drawimage(lcd_surface, x, y, status);
//     }
// }

// This function runs after every matrix scan
bool display_module_housekeeping_task_user(bool second_display) {
    static bool    layer_initialized = false;
    static uint8_t last_layer        = 0;

    static bool    modifiers_initialized = false;
    static uint8_t last_modifiers        = 0;
    static bool    last_caps_word_on     = false;

    if (!second_display) {
        uint8_t current_layer       = get_highest_layer(layer_state | default_layer_state);
        bool    in_or_out_of_no_hrm = false;
        if (!layer_initialized || current_layer != last_layer) {
            in_or_out_of_no_hrm         = (last_layer == _BASE_NO_HRM) || (current_layer == _BASE_NO_HRM);
            layer_gfx_definition* layer = &layer_mapping[current_layer > _UNDEFINED ? _UNDEFINED : current_layer];
            qp_drawimage(lcd_surface, (LCD_WIDTH - layer->icon->width) >> 1, ((LCD_HEIGHT >> 1) - layer->icon->height) >> 1, layer->icon);
            qp_drawimage(lcd_surface, (LCD_WIDTH - layer->text->width) >> 1, (LCD_HEIGHT - layer->text->height) >> 1, layer->text);
            layer_initialized = true;
            last_layer        = current_layer;
        }

        uint8_t current_modifiers    = get_mods();
        bool    current_caps_word_on = is_caps_word_on();
        bool    caps_word_changed    = current_caps_word_on != last_caps_word_on;
        if (!modifiers_initialized || in_or_out_of_no_hrm || current_modifiers != last_modifiers || caps_word_changed) {
            uint8_t changed_modifiers = last_modifiers ^ current_modifiers;
            if (!modifiers_initialized || changed_modifiers & MOD_MASK_CTRL || in_or_out_of_no_hrm) {
                painter_image_handle_t status = current_modifiers & MOD_MASK_CTRL ? control.pressed : control.unpressed;
                const uint16_t         width  = status->width;
                const uint16_t         height = status->height;
                const uint16_t         x      = (LCD_WIDTH - width * 3) >> 1;
                const uint16_t         y      = LCD_HEIGHT - (((LCD_HEIGHT >> 1) + height) >> 1);
                if (current_layer == _BASE_NO_HRM) {
                    qp_rect(lcd_surface, x, y, width - 1, height - 1, HSV_BLACK, true);
                } else {
                    qp_drawimage(lcd_surface, x, y, status);
                }
            }
            if (!modifiers_initialized || changed_modifiers & MOD_MASK_ALT || in_or_out_of_no_hrm) {
                painter_image_handle_t status = current_modifiers & MOD_MASK_ALT ? option.pressed : option.unpressed;
                const uint16_t         width  = status->width;
                const uint16_t         height = status->height;
                const uint16_t         x      = (LCD_WIDTH - width) >> 1;
                const uint16_t         y      = LCD_HEIGHT - (((LCD_HEIGHT >> 1) + height) >> 1);
                if (current_layer == _BASE_NO_HRM) {
                    qp_rect(lcd_surface, x, y, width - 1, height - 1, HSV_BLACK, true);
                } else {
                    qp_drawimage(lcd_surface, x, y, status);
                }
            }
            if (!modifiers_initialized || changed_modifiers & MOD_MASK_GUI || in_or_out_of_no_hrm) {
                painter_image_handle_t status = current_modifiers & MOD_MASK_GUI ? command.pressed : command.unpressed;
                const uint16_t         width  = status->width;
                const uint16_t         height = status->height;
                const uint16_t         x      = (LCD_WIDTH + width) >> 1;
                const uint16_t         y      = LCD_HEIGHT - (((LCD_HEIGHT >> 1) + height) >> 1);
                if (current_layer == _BASE_NO_HRM) {
                    qp_rect(lcd_surface, x, y, width - 1, height - 1, HSV_BLACK, true);
                } else {
                    qp_drawimage(lcd_surface, x, y, status);
                }
            }
            if (!modifiers_initialized || changed_modifiers & MOD_MASK_SHIFT || caps_word_changed) {
                modifier_gfx_definition* modifier = current_caps_word_on ? &caps_word : &shift;
                painter_image_handle_t   status   = current_modifiers & MOD_MASK_SHIFT ? modifier->pressed : modifier->unpressed;
                const uint16_t           width    = status->width;
                const uint16_t           height   = status->height;
                const uint16_t           x        = (LCD_WIDTH - width) >> 1;
                const uint16_t           y        = LCD_HEIGHT - (((LCD_HEIGHT >> 1) - height) >> 1);
                qp_drawimage(lcd_surface, x, y, status);
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
