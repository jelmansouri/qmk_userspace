#pragma once

#include "quantum_keycodes.h"

typedef enum {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_NAV_3D,

    LAYER_COUNT
} layer_id_t;

#define BASE TO(LAYER_BASE)
#define LOWER OSL(LAYER_LOWER)
#define RAISE OSL(LAYER_RAISE)
#define NAV_3D OSL(LAYER_NAV_3D)

// Layer locked by a double tap on its layer key, LAYER_COUNT when none
uint8_t get_locked_layer(void);

#define KC_PRSC LGUI(LSFT(KC_4))
#define KC_RCSC LGUI(LSFT(KC_5))

#define KC_HELD KC_TRNS
