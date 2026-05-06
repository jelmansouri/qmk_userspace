#pragma once

typedef enum {
    LAYER_BASE = 0,
    LAYER_BASE_NO_HRM,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_NAV_3D,

    LAYER_COUNT
} layer_id_t;

typedef enum {
    LEFT_CPI_200 = QK_USER_0,
    LEFT_CPI_400,
    LEFT_CPI_800,
    LEFT_CPI_1600,
    LEFT_CPI_3200,

    RIGHT_CPI_200,
    RIGHT_CPI_400,
    RIGHT_CPI_800,
    RIGHT_CPI_1600,
    RIGHT_CPI_3200,
} custom_keycodes_t;

#define BASE TO(LAYER_BASE)
#define BWOHRM MO(LAYER_BASE_NO_HRM)
#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define NAV_3D MO(LAYER_NAV_3D)

#define KC_PRSC LGUI(LSFT(KC_4))
#define KC_RCSC LGUI(LSFT(KC_5))
#define KC_LCK LGUI(LCTL(KC_Q))

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

#define HRM_MOD_TAP_PER_SIDE 3

#define LCPI_1 LEFT_CPI_200
#define LCPI_2 LEFT_CPI_400
#define LCPI_3 LEFT_CPI_800
#define LCPI_4 LEFT_CPI_1600
#define LCPI_5 LEFT_CPI_3200

#define RCPI_1 RIGHT_CPI_200
#define RCPI_2 RIGHT_CPI_400
#define RCPI_3 RIGHT_CPI_800
#define RCPI_4 RIGHT_CPI_1600
#define RCPI_5 RIGHT_CPI_3200

extern uint8_t right_mod_hold_count;
extern uint8_t left_mod_hold_count;
extern bool    right_hold_registered_as_press_state[HRM_MOD_TAP_PER_SIDE];
extern bool    left_hold_registered_as_press_state[HRM_MOD_TAP_PER_SIDE];
