#include "quantum.h"
#include "keymap.h"

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

bool process_oneshot_layer_key(uint16_t keycode, keyrecord_t *record) {
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
