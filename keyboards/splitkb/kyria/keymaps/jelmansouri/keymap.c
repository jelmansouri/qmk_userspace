#include QMK_KEYBOARD_H

#include "keymap.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_split_3x6_5(
        KC_GRV, KC_Q, KC_W, KC_F, KC_P, KC_B,                                                        KC_J, KC_L, KC_U, KC_Y, KC_MINS, KC_EQL, 
        KC_TAB, KC_A, KC_R, KC_S, KC_T, KC_G,                                                        KC_M, KC_N, KC_E, KC_I, KC_O, KC_SCLN, 
        KC_BSLS, KC_Z, KC_X, KC_C, KC_D, KC_V, KC_LSFT, KC_ESC,                    KC_BSPC, KC_LCTL, KC_K, KC_H, KC_COMM, KC_DOT, KC_SLSH, KC_QUOT, 
                      KC_LGUI, KC_LALT, MO(1), KC_LCTL, KC_ENT,                    KC_SPC, KC_LSFT, MO(2), KC_LALT, KC_DEL),

    [_LOWER] = LAYOUT_split_3x6_5(
        TO(3), KC_F9, KC_F10, KC_F11, KC_F12, KC_PSCR,                                                KC_NO, KC_7, KC_8, KC_9, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_F5, KC_F6, KC_F7, KC_F8, KC_NO,                                                   KC_NO, KC_4, KC_5, KC_6, KC_NO, KC_NO, 
        KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, LALT_T(KC_F4), KC_TRNS, KC_TRNS,       KC_TRNS, KC_TRNS, KC_0, KC_1, KC_2, KC_3, KC_TRNS, KC_NO, 
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
    
    [_RAISE] = LAYOUT_split_3x6_5(
        KC_TRNS, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC,                                            KC_HOME, KC_PGDN, KC_PGUP, KC_END, KC_TRNS, KC_TRNS, 
        KC_TRNS, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,                                         KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_TRNS, KC_TRNS, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LCBR, KC_RCBR, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, 
                                   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),

    [_NAV_3D] = LAYOUT_split_3x6_5(
        TO(0), KC_CAPS, KC_Q, KC_W, KC_E, KC_R,                                                       RGB_TOG, RGB_MOD, RGB_HUI, RGB_VAI, RGB_SPI, RGB_M_K, 
        KC_NO, KC_LSFT, KC_A, KC_S, KC_D, KC_F,                                                       RGB_TOG, RGB_RMOD, RGB_HUD, RGB_VAD, RGB_SPD, RGB_M_X, 
        KC_NO, KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_ESC, KC_NO,                          KC_NO, KC_NO, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_G, 
                           KC_NO, KC_NO, KC_NO, KC_SPC, KC_NO,                          KC_NO, KC_NO, KC_NO, KC_NO, KC_NO)
};

