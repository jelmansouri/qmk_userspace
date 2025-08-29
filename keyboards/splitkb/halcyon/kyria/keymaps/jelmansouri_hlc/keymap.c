// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "keymap.h"

#define BASE TO(_BASE)
#define BWOHRM MO(_BASE_NO_HRM)
#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define NAV_3D TO(_NAV_3D)

#define KC_GLO LCTL(LGUI(KC_SPC))

#define KC_HELD KC_TRNS

// Base
#define R_CTL MT(MOD_LCTL, KC_R)
#define S_ALT MT(MOD_LALT, KC_S)
#define T_GUI MT(MOD_LGUI, KC_T)

#define N_GUI MT(MOD_LGUI, KC_N)
#define E_ALT MT(MOD_LALT, KC_E)
#define I_CTL MT(MOD_LCTL, KC_I)

// LOWER
#define F6_CTL MT(MOD_LCTL, KC_F6)
#define F7_ALT MT(MOD_LALT, KC_F7)
#define F8_GUI MT(MOD_LGUI, KC_F8)

#define N4_GUI MT(MOD_LGUI, KC_4)
#define N5_ALT MT(MOD_LALT, KC_5)
#define N6_CTL MT(MOD_LCTL, KC_6)

// RAISE
#define AM_CTL MT(MOD_LCTL, KC_AMPR)
#define AS_ALT MT(MOD_LALT, KC_ASTR)
#define LP_GUI MT(MOD_LGUI, KC_LPRN)

#define DSCROLL DRAG_SCROLL

// Modify these values to adjust the scrolling speed
#define SCROLL_DIVISOR_H 32.0
#define SCROLL_DIVISOR_V 32.0

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_5_hlc(
       KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                                        KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
       KC_TAB,    KC_A,   R_CTL,   S_ALT,   T_GUI,    KC_G,                                        KC_M,   N_GUI,   E_ALT,   I_CTL,    KC_O, KC_SCLN,
      KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,  KC_ESC,  KC_ENT,   KC_SPC, KC_BSPC,   KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                  KC_GLO, KC_LGUI,   LOWER, KC_LSFT,  NAV_3D,   CW_TOGG,KC_LSFT,  RAISE,  BWOHRM,  KC_DEL,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_BASE_NO_HRM] = LAYOUT_split_3x6_5_hlc(
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS,    KC_R,    KC_S,    KC_T, KC_TRNS,                                     KC_TRNS,    KC_N,    KC_E,    KC_I, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_LOWER] = LAYOUT_split_3x6_5_hlc(
      KC_TRNS,   KC_F9,  KC_F10,  KC_F11,  KC_F12, DSCROLL,                                       KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
      KC_TRNS,   KC_F5,  F6_CTL,  F7_ALT,  F8_GUI, MS_BTN1,                                       KC_NO,  N4_GUI,  N5_ALT,  N6_CTL,   KC_NO,   KC_NO,
      KC_TRNS,   KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_0,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT_split_3x6_5_hlc(
      KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_CIRC,  AM_CTL,  AS_ALT,  LP_GUI, KC_RPRN,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LCBR, KC_RCBR, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HELD, KC_TRNS, KC_TRNS,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_NAV_3D] = LAYOUT_split_3x6_5_hlc(
        KC_NO,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,                                     RM_TOGG, RM_VALU, RM_VALD, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V, KC_TRNS,  KC_SPC,   KC_NO,   KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_LALT, KC_LGUI,   KC_NO, KC_TRNS,    BASE,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    )
};
// clang-format on

bool set_scrolling = false;

// Handle new Mod Tap shifted keycodes as they are not supported using the MT macro
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AM_CTL:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_AMPR);
                return false;
            }
            -break;
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

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

// Function to handle mouse reports and perform drag scrolling
//
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (set_scrolling) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v -= (float)mouse_report.y / SCROLL_DIVISOR_V;

        // Assign integer parts of accumulated scroll values to the mouse report
        mouse_report.h = (int8_t)scroll_accumulated_h;
        mouse_report.v = (int8_t)scroll_accumulated_v;

        // Update accumulated scroll values by subtracting the integer parts
        scroll_accumulated_h -= (int8_t)scroll_accumulated_h;
        scroll_accumulated_v -= (int8_t)scroll_accumulated_v;

        // Clear the X and Y values of the mouse report
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

// Function to handle layer changes and disable drag scrolling
layer_state_t layer_state_set_user(layer_state_t state) {
    set_scrolling = false;
    return state;
}

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_BLUE);
}
