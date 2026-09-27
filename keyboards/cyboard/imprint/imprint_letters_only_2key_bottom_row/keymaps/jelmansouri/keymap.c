#include <stdint.h>
#include QMK_KEYBOARD_H
#include <cyboard.h>

#include "keymap.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_let(
         KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                              KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
         KC_TAB,    KC_A,    KC_R,    KC_S,    KC_T,    KC_G,                              KC_M,    KC_N,    KC_E,    KC_I,    KC_O, KC_SCLN,
        KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                              KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                          KC_LCTL, KC_LALT,  KC_ESC,  KC_ENT,  NAV_3D,         CW_TOGG,  KC_SPC, KC_BSPC, KC_LALT, KC_LCTL,
                                              LOWER, KC_LSFT, KC_LGUI,         KC_LGUI, KC_LSFT,   RAISE
    ),

    [LAYER_LOWER] = LAYOUT_let(
        KC_PRSC,   KC_F9,  KC_F10,  KC_F11,  KC_F12, MS_BTN3,                           XXXXXXX,    KC_7,    KC_8,    KC_9, _______, _______,
        KC_RCSC,   KC_F5,   KC_F6,   KC_F7,   KC_F8, MS_BTN1,                           XXXXXXX,    KC_4,    KC_5,    KC_6, XXXXXXX, XXXXXXX,
        _______,   KC_F1,   KC_F2,   KC_F3,   KC_F4, MS_BTN2,                              KC_0,    KC_1,    KC_2,    KC_3, _______, XXXXXXX,
                          _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
                                            KC_HELD, _______, _______,         _______, _______, _______
    ),

    [LAYER_RAISE] = LAYOUT_let(
        _______, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                           KC_HOME, KC_PGDN, KC_PGUP,  KC_END, XXXXXXX, XXXXXXX,
        _______, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN,                           KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT, XXXXXXX, XXXXXXX,
        _______, XXXXXXX, XXXXXXX, XXXXXXX, KC_LCBR, KC_RCBR,                           KC_LBRC, KC_RBRC, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                          _______, _______, _______, _______, _______,         _______, _______, _______, _______, _______,
                                            _______, _______, _______,         _______, _______, KC_HELD
    ),

    [LAYER_NAV_3D] = LAYOUT_let(
      XXXXXXX,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,                             RM_TOGG, RM_VALU, RM_VALD, XXXXXXX, XXXXXXX, XXXXXXX,
      XXXXXXX, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                              LCPI_1,  LCPI_2,  LCPI_3,  LCPI_4,  LCPI_5, XXXXXXX,
      XXXXXXX, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,                              RCPI_1,  RCPI_2,  RCPI_3,  RCPI_4,  RCPI_5, XXXXXXX,
                          _______, _______, _______, _______, KC_HELD,         MS_BTN1,  KC_SPC, KC_BSPC, _______, _______,
                                            _______, _______, _______,         MS_BTN2,  KC_ENT,  KC_ESC
    )
};
// clang-format on

/* QMK tracks a single one-shot layer, so its stock OSL handling breaks down when layer keys
 * overlap: pressing a second layer key overwrites the first one's tracking and leaves that layer
 * on, stuck, and releasing either key clears the "held" state of whichever layer is tracked.
 * Layer keys are handled here so that they behave like momentary layers when overlapping:
 * the last pressed wins, releasing it hands over to a layer key still held, and releasing a
 * layer key that was taken over does nothing. Taps, one-shots and tap-toggle locks are left to
 * the stock handling. */

// Bit per layer whose layer key is physically held
static layer_state_t held_layer_keys = 0;

static bool is_tracked(uint8_t layer) {
    return get_oneshot_layer_state() && get_oneshot_layer() == layer;
}

static bool process_oneshot_layer_key(uint16_t keycode, keyrecord_t *record) {
    const uint8_t layer = QK_ONE_SHOT_LAYER_GET_LAYER(keycode);

    if (record->event.pressed) {
        held_layer_keys |= (layer_state_t)1 << layer;
        // Turn the previous one-shot layer (held, pending or locked) off before the stock
        // handling starts this one and forgets about it.
        if (get_oneshot_layer_state() && get_oneshot_layer() != layer) {
            uint8_t previous = get_oneshot_layer();
            reset_oneshot_layer();
            layer_off(previous);
        }
        return true;
    }

    held_layer_keys &= ~((layer_state_t)1 << layer);

    // Tap-toggle: the stock handling locks this layer on this release (the one-shot tracking
    // was already reset by the press).
    if (record->tap.count >= ONESHOT_TAP_TOGGLE) return true;

    // Taken over by another layer key: leave the active layer alone.
    if (!is_tracked(layer)) return false;

    // Another layer key is still held: it takes over as a momentary layer.
    if (held_layer_keys) {
        reset_oneshot_layer();
        layer_off(layer);
        set_oneshot_layer(get_highest_layer(held_layer_keys), ONESHOT_PRESSED);
        return false;
    }
    return true;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LOWER:
        case RAISE:
        case NAV_3D:
            return process_oneshot_layer_key(keycode, record);

        case SCROLL_CPI_200 ... SCROLL_CPI_600:
            if (record->event.pressed) {
                uint16_t cpi = 200;
                switch (keycode) {
                    case SCROLL_CPI_200:
                        cpi = 200;
                        break;
                    case SCROLL_CPI_300:
                        cpi = 300;
                        break;
                    case SCROLL_CPI_400:
                        cpi = 400;
                        break;
                    case SCROLL_CPI_500:
                        cpi = 500;
                        break;
                    case SCROLL_CPI_600:
                        cpi = 600;
                        break;
                }
                pointing_device_set_cpi_on_side(true, cpi);
            }
            return false;

        case MOUSE_CPI_600 ... MOUSE_CPI_1600:
            if (record->event.pressed) {
                uint16_t cpi = 600;
                switch (keycode) {
                    case MOUSE_CPI_600:
                        cpi = 600;
                        break;
                    case MOUSE_CPI_800:
                        cpi = 800;
                        break;
                    case MOUSE_CPI_1000:
                        cpi = 1000;
                        break;
                    case MOUSE_CPI_1200:
                        cpi = 1200;
                        break;
                    case MOUSE_CPI_1600:
                        cpi = 1600;
                        break;
                }
                pointing_device_set_cpi_on_side(false, cpi);
            }
            return false;
    }
    return true;
}
