// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "keymap.h"

#define BASE TO(_BASE)
#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define NAV_3D TO(_NAV_3D)

#define LALT_F4 LALT_T(KC_F4)
#define DSCROLL DRAG_SCROLL

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
 * Halcyon Layer template
 *
 * ,-------------------------------------------.                              ,-------------------------------------------.
 * |    `   |  q   |  w   |  f   |  p   |  b   |                              |  j   |  l   |  u   |  y   |  -   |  =     |
 * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
 * |   tab  |  a   |  r   |  s   |  t   |  g   |                              |  m   |  n   |  e   |  i   |  o   |  ;     |
 * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
 * |    \   |  z   |  x   |  c   |  d   |  v   |LShift|  Esc |  |BSpace|LCtrl |  k   |  h   |  ,   |  .   |  /   |  '     |
 * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
 *                        |  Cmd | LAlt |Lower |LCtrl |Enter |  |Space |LShift| Raise| LAlt | Del  |
 *                        `----------------------------------'  `----------------------------------'
 * ,-----------------------------------.                                              ,-----------------------------------.
 * |      |      |       |      |      |                                              |      |      |       |      |      |
 * `-----------------------------------'                                              `-----------------------------------'
 */
    [_BASE] = LAYOUT_split_3x6_5_hlc(
       KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                                        KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
       KC_TAB,    KC_A,    KC_R,    KC_S,    KC_T,    KC_G,                                        KC_M,    KC_N,    KC_E,    KC_I,    KC_O, KC_SCLN,
      KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V, KC_LSFT,  KC_ESC, KC_BSPC, KC_LCTL,    KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                 KC_LGUI, KC_LALT,   LOWER, KC_LCTL, KC_ENT,   KC_SPC, KC_LSFT,   RAISE, KC_LALT,  KC_DEL,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_LOWER] = LAYOUT_split_3x6_5_hlc(
       NAV_3D,   KC_F9,  KC_F10,  KC_F11,  KC_F12, KC_PSCR,                                       KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
      KC_TRNS,   KC_F5,   KC_F6,   KC_F7,   KC_F8, MS_BTN1,                                       KC_NO,    KC_4,    KC_5,    KC_6,   KC_NO,   KC_NO,
      LALT_F4,   KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_0,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_RAISE] = LAYOUT_split_3x6_5_hlc(
      KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LCBR, KC_RCBR, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    ),

    [_NAV_3D] = LAYOUT_split_3x6_5_hlc(
         BASE, KC_CAPS,    KC_Q,    KC_W,    KC_E,    KC_R,                                     RGB_TOG, RGB_MOD, RGB_HUI, RGB_VAI, RGB_SPI, RGB_M_K,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                                     RGB_TOG, RGB_RMOD, RGB_HUD, RGB_VAD, RGB_SPD, RGB_M_X,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,  KC_ESC,   KC_NO,   KC_NO,   KC_NO, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_G,
                                   KC_NO,   KC_NO,   KC_NO,  KC_SPC,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,

      _______, _______, _______, _______, _______,                                                       _______, _______, _______, _______, _______
    )
};
// clang-format on

bool set_scrolling = false;

// Modify these values to adjust the scrolling speed
#define SCROLL_DIVISOR_H 8.0
#define SCROLL_DIVISOR_V 8.0

// Variables to store accumulated scroll values
float scroll_accumulated_h = 0;
float scroll_accumulated_v = 0;

// Function to handle mouse reports and perform drag scrolling
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    // Check if drag scrolling is active
    if (set_scrolling) {
        // Calculate and accumulate scroll values based on mouse movement and divisors
        scroll_accumulated_h += (float)mouse_report.x / SCROLL_DIVISOR_H;
        scroll_accumulated_v += (float)mouse_report.y / SCROLL_DIVISOR_V;

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

// Function to handle key events and enable/disable drag scrolling
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DRAG_SCROLL:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            set_scrolling = record->event.pressed;
            break;
        default:
            break;
    }
    return true;
}

// Function to handle layer changes and disable drag scrolling when not in AUTO_MOUSE_DEFAULT_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    // Disable set_scrolling if the current layer is not the AUTO_MOUSE_DEFAULT_LAYER
    if (get_highest_layer(state) != AUTO_MOUSE_DEFAULT_LAYER) {
        set_scrolling = false;
    }
    return state;
}
