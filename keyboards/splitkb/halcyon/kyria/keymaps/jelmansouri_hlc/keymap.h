#pragma once

enum layers {
    _BASE = 0,
    _BASE_NO_HRM,
    _LOWER,
    _RAISE,
    _NAV_3D,

    _UNDEFINED,
    _COUNT
};

enum custom_keycodes {
    DRAG_SCROLL = SAFE_RANGE,
};

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
