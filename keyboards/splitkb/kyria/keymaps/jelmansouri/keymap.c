#include QMK_KEYBOARD_H

#include "keymap.h"

#define LOWER MO(_LOWER)
#define RAISE MO(_RAISE)
#define NAV_3D MO(_NAV_3D)

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

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/*
 * Kyria Layer template
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
 */
    [_BASE] = LAYOUT_split_3x6_5(
       KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                                        KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
       KC_TAB,    KC_A,   R_CTL,   S_ALT,   T_GUI,    KC_G,                                        KC_M,   N_GUI,   E_ALT,   I_CTL,    KC_O, KC_SCLN,
      KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,  KC_ESC,  KC_ENT,   KC_SPC, KC_BSPC,   KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                 KC_LGUI, KC_LGUI,   LOWER, KC_LSFT,  NAV_3D,   CW_TOGG,KC_LSFT,  RAISE, KC_LGUI,  KC_DEL
    ),

    [_LOWER] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_F9,  KC_F10,  KC_F11,  KC_F12, DSCROLL,                                       KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_F5,  F6_CTL,  F7_ALT,  F8_GUI, MS_BTN1,                                       KC_NO,  N4_GUI,  N5_ALT,  N6_CTL,   KC_NO,   KC_NO,
      KC_TRNS, KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_0,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_RAISE] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_CIRC,  AM_CTL,  AS_ALT,  LP_GUI, KC_RPRN,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_NAV_3D] = LAYOUT_split_3x6_5(
        KC_NO, KC_CAPS,    KC_Q,    KC_W,    KC_E,    KC_R,                                     RGB_TOG, RGB_MOD, RGB_HUI, RGB_VAI, RGB_SPI, RGB_M_K,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                                     RGB_TOG, RGB_RMOD, RGB_HUD, RGB_VAD, RGB_SPD, RGB_M_X,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,   KC_NO,   KC_NO,   KC_NO,   KC_NO, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_G,
                                   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    )
};
// clang-format on

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case AM_CTL:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_AMPR); // Send KC_DQUO on tap
                return false;        // Return false to ignore further processing of key
            }
            break;
        case AS_ALT:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_ASTR); // Send KC_DQUO on tap
                return false;        // Return false to ignore further processing of key
            }
            break;
        case LP_GUI:
            if (record->tap.count && record->event.pressed) {
                tap_code16(KC_LPRN); // Send KC_DQUO on tap
                return false;        // Return false to ignore further processing of key
            }
            break;
    }
    return true;
}

bool is_flow_tap_key(uint16_t keycode) {
    if ((get_mods() & (MOD_MASK_CG | MOD_BIT_LALT)) != 0) {
        return false; // Disable Flow Tap on hotkeys.
    }
    switch (get_tap_keycode(keycode)) {
        // case KC_SPC:
        case KC_A ... KC_Z:
        case KC_DOT:
        case KC_COMM:
        case KC_SCLN:
        case KC_SLSH:
            return true;
    }
    return false;
}
