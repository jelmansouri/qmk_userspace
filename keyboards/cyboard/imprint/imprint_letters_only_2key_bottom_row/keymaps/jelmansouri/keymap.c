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
                          KC_LCTL, KC_LALT,  KC_ESC,  KC_ENT,  NAV_3D,         CW_TOGG,  KC_SPC, KC_BSPC, KC_RALT, KC_RCTL,
                                              LOWER, KC_LSFT, KC_LGUI,         KC_RGUI, KC_RSFT,   RAISE
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
        XXXXXXX,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,                           RM_TOGG, RM_VALU, RM_VALD, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
        XXXXXXX, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,                           XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
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
 * layer key that was taken over does nothing. Tapping a layer key while another is held starts
 * its one-shot, and the held one takes over again once that one-shot is used or times out.
 *
 * A double tap locks a layer, which then acts like a layer key held forever: other layer keys
 * still one-shot or hold their layer on top of it, and it comes back once they are done. Double
 * tapping another layer key moves the lock. Pressing the locked layer key unlocks it, its layer
 * staying active until the key is released. The lock is kept here instead of in the one-shot
 * tracking so that it survives other one-shots. Single taps and holds use the stock handling. */

// Bit per layer whose layer key is physically held
static layer_state_t held_layer_keys = 0;

// Layer locked by a double tap, LAYER_COUNT when none
static uint8_t locked_layer = LAYER_COUNT;

// A layer key's layer is on top of a held or locked layer: that one comes back once it ends
// (see oneshot_layer_changed_user).
static bool resume_after_oneshot = false;

uint8_t get_locked_layer(void) {
    return locked_layer;
}

// Called with layer 0 when the one-shot layer ends: used by the next key, timed out or released.
void oneshot_layer_changed_user(uint8_t layer) {
    if (!layer && resume_after_oneshot) {
        resume_after_oneshot = false;
        if (held_layer_keys) {
            set_oneshot_layer(get_highest_layer(held_layer_keys), ONESHOT_PRESSED);
        } else if (locked_layer < LAYER_COUNT) {
            layer_on(locked_layer);
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LOWER:
        case RAISE:
        case NAV_3D: {
            const uint8_t layer = QK_ONE_SHOT_LAYER_GET_LAYER(keycode);

            if (record->event.pressed) {
                held_layer_keys |= (layer_state_t)1 << layer;
                // A second tap briefly resets the one-shot tracking before locking: don't resume then.
                resume_after_oneshot = false;
                // Turn the previous one-shot layer (held or pending) off before the stock handling
                // starts this one and forgets about it.
                if (get_oneshot_layer_state() && get_oneshot_layer() != layer) {
                    uint8_t previous = get_oneshot_layer();
                    reset_oneshot_layer();
                    layer_off(previous);
                }
                // Unlock: the layer stays on while the key is held, without a one-shot.
                if (locked_layer == layer) {
                    locked_layer = LAYER_COUNT;
                    set_oneshot_layer(layer, ONESHOT_PRESSED);
                    return false;
                }
                // The locked layer stays locked, but is off while this one is active.
                if (locked_layer < LAYER_COUNT) layer_off(locked_layer);
                return true;
            }

            held_layer_keys &= ~((layer_state_t)1 << layer);

            // Double tap: lock this layer, moving the lock from any other one (turned off by the press).
            // Reset the tracking left by the press when it was an unlock (triple tap).
            if (record->tap.count >= ONESHOT_TAP_TOGGLE) {
                reset_oneshot_layer();
                locked_layer = layer;
                layer_on(layer);
                return false;
            }

            // Taken over by another layer key: leave the active layer alone.
            if (!get_oneshot_layer_state() || get_oneshot_layer() != layer) return false;

            // On top of a held or locked layer, which comes back once this one ends: after its one-shot
            // when tapped, right away when held.
            if (held_layer_keys || locked_layer < LAYER_COUNT) {
                resume_after_oneshot = true;
                if (!record->tap.count) {
                    clear_oneshot_layer_state(ONESHOT_START);
                    return false;
                }
            }
            return true;
        }
    }
    return true;
}
