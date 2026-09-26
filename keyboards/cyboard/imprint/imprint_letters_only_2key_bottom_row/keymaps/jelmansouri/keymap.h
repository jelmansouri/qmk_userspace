#pragma once

#include "quantum_keycodes.h"

typedef enum {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_RAISE,
    LAYER_NAV_3D,

    LAYER_COUNT
} layer_id_t;

typedef enum {
    SCROLL_CPI_200 = SAFE_RANGE,
    SCROLL_CPI_300,
    SCROLL_CPI_400,
    SCROLL_CPI_500,
    SCROLL_CPI_600,

    MOUSE_CPI_600,
    MOUSE_CPI_800,
    MOUSE_CPI_1000,
    MOUSE_CPI_1200,
    MOUSE_CPI_1600,
} custom_keycodes_t;

#define BASE TO(LAYER_BASE)
#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define NAV_3D MO(LAYER_NAV_3D)

#define KC_PRSC LGUI(LSFT(KC_4))
#define KC_RCSC LGUI(LSFT(KC_5))

#define KC_HELD KC_TRNS

#define LCPI_1 SCROLL_CPI_200
#define LCPI_2 SCROLL_CPI_300
#define LCPI_3 SCROLL_CPI_400
#define LCPI_4 SCROLL_CPI_500
#define LCPI_5 SCROLL_CPI_600

#define RCPI_1 MOUSE_CPI_600
#define RCPI_2 MOUSE_CPI_800
#define RCPI_3 MOUSE_CPI_1000
#define RCPI_4 MOUSE_CPI_1200
#define RCPI_5 MOUSE_CPI_1600
