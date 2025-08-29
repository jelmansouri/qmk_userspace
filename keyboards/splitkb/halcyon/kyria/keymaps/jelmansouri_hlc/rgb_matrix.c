#include "quantum.h"

#include "keymap.h"

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_BLUE);
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    for (uint8_t i = led_min; i < led_max; i++) {
        switch (get_highest_layer(layer_state | default_layer_state)) {
            case _BASE:
            case _BASE_NO_HRM:
                rgb_matrix_set_color(i, RGB_BLUE);
                break;
            case _LOWER:
                rgb_matrix_set_color(i, RGB_TEAL);
                break;
            case _RAISE:
                rgb_matrix_set_color(i, RGB_TURQUOISE);
                break;
            case _NAV_3D:
                rgb_matrix_set_color(i, RGB_CORAL);
                break;
            default:
                break;
        }
    }
    return false;
}

// bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
//     if (get_highest_layer(layer_state) > 0) {
//         uint8_t layer = get_highest_layer(layer_state);
//
//         for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
//             for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
//                 uint8_t index = g_led_config.matrix_co[row][col];
//
//                 if (index >= led_min && index < led_max && index != NO_LED && keymap_key_to_keycode(layer, (keypos_t){col, row}) > KC_TRNS) {
//                     rgb_matrix_set_color(index, RGB_GREEN);
//                 }
//             }
//         }
//     }
//     return false;
// }
