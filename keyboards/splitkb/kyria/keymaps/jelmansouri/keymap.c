#include QMK_KEYBOARD_H

#include "keymap.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_split_3x6_5(
       KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                                        KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
       KC_TAB,    KC_A,   R_CTL,   S_ALT,   T_GUI,    KC_G,                                        KC_M,   N_GUI,   E_ALT,   I_CTL,    KC_O, KC_SCLN,
      KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,  KC_ESC,  KC_ENT,   KC_SPC, KC_BSPC,   KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                  KC_GLO, KC_LGUI,   LOWER, KC_LSFT,  NAV_3D,   CW_TOGG,KC_LSFT,  RAISE,  BWOHRM,  KC_DEL
    ),

    [LAYER_BASE_NO_HRM] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS,    KC_R,    KC_S,    KC_T, KC_TRNS,                                     KC_TRNS,    KC_N,    KC_E,    KC_I, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS
    ),

    [LAYER_LOWER] = LAYOUT_split_3x6_5(
      KC_TRNS,   KC_F9,  KC_F10,  KC_F11,  KC_F12, DSCROLL,                                       KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
      KC_TRNS,   KC_F5,  F6_CTL,  F7_ALT,  F8_GUI, MS_BTN1,                                       KC_NO,  N4_GUI,  N5_ALT,  N6_CTL,   KC_NO,   KC_NO,
      KC_TRNS,   KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_0,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [LAYER_RAISE] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END,   KC_NO,   KC_NO,
      KC_TRNS, KC_CIRC,  AM_CTL,  AS_ALT,  LP_GUI, KC_RPRN,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT,   KC_NO,   KC_NO,
      KC_TRNS,   KC_NO,   KC_NO,   KC_NO, KC_LCBR, KC_RCBR, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS, KC_TRNS
    ),

    [LAYER_NAV_3D] = LAYOUT_split_3x6_5(
        KC_NO,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,                                     RM_TOGG, RM_VALU, RM_VALD,   KC_NO,   KC_NO,   KC_NO,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                                       KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,  KC_ESC,  KC_SPC,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                                 KC_LALT, KC_LGUI,   KC_NO,   KC_NO,    BASE,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    )
};
// clang-format on

bool set_scrolling = false;

// Track hold state of left hand mod-tap keys
uint8_t right_mod_hold_count = 0;

// Handle new Mod Tap shifted keycodes as they are not supported using the MT macro
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case R_CTL:
            if (!record->tap.count && record->event.pressed) {
                // R_CTL is being held - check if other left mods are active
                if (right_mod_hold_count > 0) {
                    // Other left hand mods are held, just send R instead of control mod
                    tap_code16(KC_R);
                    return false;
                }
            }
            break;
        case I_CTL:
            if (!record->tap.count && record->event.pressed) {
                // S_ALT is being held
                right_mod_hold_count++;
            } else if (!record->tap.count && !record->event.pressed) {
                // S_ALT hold is released
                right_mod_hold_count--;
            }
            break;
        case AS_ALT:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_ASTR);
                return false;
            }
            break;
        case LP_GUI:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_LPRN);
                return false;
            }
            break;
        case DRAG_SCROLL:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            set_scrolling = record->event.pressed;
            break;
    }
    return true;
}

// Function to handle layer changes and disable drag scrolling
layer_state_t layer_state_set_user(layer_state_t state) {
    set_scrolling = false;
    return state;
}
