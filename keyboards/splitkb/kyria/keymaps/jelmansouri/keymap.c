#include QMK_KEYBOARD_H

#include "keymap.h"

#define BASE TO(_BASE)
#define FN MO(_FN)
#define SYM MO(_SYM)
#define NUM MO(_NUM)
#define NAV MO(_NAV)
#define NAV_3D TO(_NAV_3D)

#define LCTL_R MT(MOD_LCTL, KC_R)
#define LALT_S MT(MOD_LALT, KC_S)
#define LGUI_T MT(MOD_LGUI, KC_T)

#define LGUI_N MT(MOD_LGUI, KC_N)
#define LALT_E MT(MOD_LALT, KC_E)
#define LCTL_I MT(MOD_LCTL, KC_I)

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
       KC_TAB,    KC_A,  LCTL_R,  LALT_S,  LGUI_T,    KC_G,                                        KC_M,  LGUI_N,  LALT_E,  LCTL_I,    KC_O, KC_SCLN,
      KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,  KC_ESC,  KC_ENT,   KC_SPC, KC_BSPC,   KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                                 KC_LGUI, KC_LGUI,      FN,     SYM, KC_LSFT,   KC_LSFT,    NUM,    NAV, CW_TOGG,  KC_DEL
    ),

    [_FN] = LAYOUT_split_3x6_5(
       NAV_3D,   KC_F9,  KC_F10,  KC_F11,  KC_F12, DSCROLL,                                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS,   KC_F5,   KC_F6,   KC_F7,   KC_F8, MS_BTN1,                                     KC_TRNS, KC_LGUI, KC_LALT, KC_LCTL, KC_TRNS, KC_TRNS,
      KC_TRNS,   KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_SYM] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                                     KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,                                     KC_TRNS, KC_LGUI, KC_LALT, KC_LCTL, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_NUM] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                       KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_LCTL, KC_LALT, KC_LGUI, KC_TRNS,                                       KC_NO,    KC_4,    KC_5,    KC_6,   KC_NO,   KC_NO,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_0,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_NAV] = LAYOUT_split_3x6_5(
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                                     KC_HOME, KC_PGDN, KC_PGUP,  KC_END, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_LCTL, KC_LALT, KC_LGUI, KC_TRNS,                                     KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, KC_TRNS, KC_TRNS,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [_NAV_3D] = LAYOUT_split_3x6_5(
         BASE, KC_CAPS,    KC_Q,    KC_W,    KC_E,    KC_R,                                     RGB_TOG, RGB_MOD, RGB_HUI, RGB_VAI, RGB_SPI, RGB_M_K,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                                     RGB_TOG, RGB_RMOD, RGB_HUD, RGB_VAD, RGB_SPD, RGB_M_X,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,  KC_ESC,  KC_SPC,   KC_NO,   KC_NO, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_G,
                                   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    )
};
// clang-format on
