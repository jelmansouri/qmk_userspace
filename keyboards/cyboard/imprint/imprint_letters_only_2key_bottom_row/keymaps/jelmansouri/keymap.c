#include <stdint.h>
#include QMK_KEYBOARD_H
#include <cyboard.h>

#include "keymap.h"

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_let(
         KC_GRV,    KC_Q,    KC_W,    KC_F,    KC_P,    KC_B,                              KC_J,    KC_L,    KC_U,    KC_Y, KC_MINS,  KC_EQL,
         KC_TAB,    KC_A,   R_CTL,   S_ALT,   T_GUI,    KC_G,                              KC_M,   N_GUI,   E_ALT,   I_CTL,    KC_O, KC_SCLN,
        KC_BSLS,    KC_Z,    KC_X,    KC_C,    KC_D,    KC_V,                              KC_K,    KC_H, KC_COMM,  KC_DOT, KC_SLSH, KC_QUOT,
                          MS_BTN2, MS_BTN1,  KC_ESC,  KC_ENT,  NAV_3D,          KC_DEL,  KC_SPC, KC_BSPC, KC_LBRC, KC_RBRC,
                                              LOWER, KC_LSFT, KC_LGUI,         CW_TOGG, KC_LSFT,   RAISE
    ),

    [LAYER_BASE_NO_HRM] = LAYOUT_let(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                           KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS,    KC_R,    KC_S,    KC_T, KC_TRNS,                           KC_TRNS,    KC_N,    KC_E,    KC_I, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,                           KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [LAYER_LOWER] = LAYOUT_let(
        KC_PRSC,   KC_F9,  KC_F10,  KC_F11,  KC_F12, KC_TRNS,                             KC_NO,    KC_7,    KC_8,    KC_9, KC_TRNS, KC_TRNS,
        KC_RCSC,   KC_F5,  F6_CTL,  F7_ALT,  F8_GUI, KC_TRNS,                             KC_NO,  N4_GUI,  N5_ALT,  N6_CTL,   KC_NO,   KC_NO,
        KC_TRNS,   KC_F1,   KC_F2,   KC_F3,   KC_F4, KC_TRNS,                             KC_NO,    KC_1,    KC_2,    KC_3, KC_TRNS,   KC_NO,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_TRNS,    KC_0,  KC_DOT,
                                            KC_HELD, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_TRNS
    ),

    [LAYER_RAISE] = LAYOUT_let(
        KC_TRNS, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                           KC_HOME, KC_PGDN, KC_PGUP,  KC_END,   KC_NO,   KC_NO,
        KC_TRNS, KC_CIRC,  AM_CTL,  AS_ALT,  LP_GUI, KC_RPRN,                           KC_LEFT, KC_DOWN,   KC_UP, KC_RGHT,   KC_NO,   KC_NO,
        KC_TRNS,   KC_NO,   KC_NO,   KC_NO, KC_LCBR, KC_RCBR,                           KC_LBRC, KC_RBRC,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_TRNS, KC_LBRC, KC_RBRC,
                                            KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS, KC_TRNS, KC_HELD
    ),

    [LAYER_NAV_3D] = LAYOUT_let(
        KC_NO,  KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,                             RM_TOGG, RM_VALU, RM_VALD,   KC_NO,   KC_NO,   KC_NO,
        KC_NO, KC_LSFT,    KC_A,    KC_S,    KC_D,    KC_F,                               KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO, KC_LCTL,    KC_Z,    KC_X,    KC_C,    KC_V,                               KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                          KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HELD,           KC_NO,  KC_SPC, KC_BSPC, KC_TRNS, KC_TRNS,
                                            KC_TRNS, KC_TRNS, KC_TRNS,           KC_NO,  KC_ENT,  KC_ESC
    )
};

// clang-format on

const uint16_t PROGMEM middle_click_combo[] = {MS_BTN1, MS_BTN2, COMBO_END};
combo_t                key_combos[]         = {
    COMBO(middle_click_combo, MS_BTN3),
};

bool set_scrolling = false;

uint8_t right_mod_hold_count                                       = 0;
bool    right_hold_registered_as_press_state[HRM_MOD_TAP_PER_SIDE] = {0};
uint8_t left_mod_hold_count                                        = 0;
bool    left_hold_registered_as_press_state[HRM_MOD_TAP_PER_SIDE]  = {0};

// This function allows held keys on one side of the keyboard to behave as long tap key press if the other side
// registered a hold beforehand, so instead of relying on quick tap, you can hold CTL + R (on Colemak-DH) to do a
// continuous redo for example, othewise CTL would be held on both sides
static inline bool register_mod_hold_as_tap_hold(uint16_t keycode, keyrecord_t *record,
                                                 uint8_t *this_side_mod_hold_count, uint8_t *other_side_mod_hold_count,
                                                 bool *registered_state) {
    // We only special-case holds (tap.count == 0). Taps fall through to QMK.
    if (record->tap.count == 0 && record->event.pressed) {
        if (*other_side_mod_hold_count > 0) {
            // Other side is holding a mod-tap: emit the tap key instead of the mod.
            register_code(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
            *registered_state = true;
            return true;
        } else {
            (*this_side_mod_hold_count)++;
        }
    } else if (record->tap.count == 0 && !record->event.pressed) {
        if (*registered_state) {
            // We previously registered the tap key on press; release it now.
            unregister_code(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
            *registered_state = false;
            return true;
        } else if (*this_side_mod_hold_count > 0) {
            (*this_side_mod_hold_count)--;
        }
    }
    return false;
}

#if __STDC_VERSION__ >= 201112L
// To no to forget to update the fallthrough logic
_Static_assert(HRM_MOD_TAP_PER_SIDE == 3, "Update index mapping if count changes");
#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    int state_idx = HRM_MOD_TAP_PER_SIDE;
    switch (keycode) {
        // Right-hand mod-taps (map to indices 0..2)
        case R_CTL:
        case F6_CTL:
            state_idx--; /* fallthrough */
        case S_ALT:
        case F7_ALT:
            state_idx--; /* fallthrough */
        case T_GUI:
        case F8_GUI:
            state_idx--;
            if (register_mod_hold_as_tap_hold(keycode, record, &left_mod_hold_count, &right_mod_hold_count,
                                              &left_hold_registered_as_press_state[state_idx])) {
                return false;
            }
            break;

        // Left-hand mod-taps (map to indices 0..2)
        case I_CTL:
        case N6_CTL:
            state_idx--; /* fallthrough */
        case E_ALT:
        case N5_ALT:
            state_idx--; /* fallthrough */
        case N_GUI:
        case N4_GUI:
            state_idx--;
            if (register_mod_hold_as_tap_hold(keycode, record, &right_mod_hold_count, &left_mod_hold_count,
                                              &right_hold_registered_as_press_state[state_idx])) {
                return false;
            }
            break;

        case AM_CTL:
        case AS_ALT:
        case LP_GUI:
            if (record->tap.count && record->event.pressed) {
                tap_code16(S(QK_MOD_TAP_GET_TAP_KEYCODE(keycode)));
                return false;
            }
            break;
        case DRAG_SCROLL:
            // Toggle set_scrolling when DRAG_SCROLL key is pressed or released
            set_scrolling = record->event.pressed;
            break;
    }
    return true;
}

// Function to handle layer changes and disable drag scrolling
layer_state_t layer_state_set_user(layer_state_t state) {
    set_scrolling = false;
    return state;
}

bool is_flow_tap_key(uint16_t keycode) {
    if ((get_mods() & (MOD_MASK_CG | MOD_BIT_LALT)) != 0) {
        return false; // Disable Flow Tap on hotkeys.
    }
    switch (get_tap_keycode(keycode)) {
        case KC_A ... KC_Z:
        case KC_DOT:
        case KC_COMM:
        case KC_SCLN:
        case KC_SLSH:
            return true;
    }
    return false;
}
