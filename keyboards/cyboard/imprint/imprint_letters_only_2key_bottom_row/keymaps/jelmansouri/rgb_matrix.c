#include <stdint.h>
#include "quantum.h"
#include "lib/lib8tion/lib8tion.h"
#include "keymap.h"
#ifdef SPLIT_KEYBOARD
#    include "transactions.h"
#endif

typedef struct layer_palette_t {
    hsv_t primary, modtap;
} layer_palette_t;

// clang-format off
// --------- Global brightness knob (one place to dim/boost everything)
#define VDEF_MAX 255

// Layer colors
#define HSV_TEAL_NEO          110,240,VDEF_MAX
#define HSV_TANGERINE_NEO      20,255,VDEF_MAX

#define HSV_FUCHSIA_NEO       222,255,VDEF_MAX
#define HSV_CHARTREUSE_NEO     64,255,VDEF_MAX

#define HSV_ELECTRIC_BLUE_NEO 170,255,VDEF_MAX
#define HSV_ULTRAVIOLET_NEO   196,255,VDEF_MAX

#define HSV_NEON_GREEN_NEO     90,255,VDEF_MAX
#define HSV_HOT_PINK_NEO      234,230,VDEF_MAX

// Modifier colors (layer independent). Held modifiers are averaged in RGB space,
// so primaries mix cleanly: Ctrl+Opt = yellow, Ctrl+Cmd = magenta, Opt+Cmd = cyan,
// and Shift (white) lightens whatever it is combined with.
#define HSV_MOD_CTRL            0,255,VDEF_MAX   // RED
#define HSV_MOD_ALT            85,255,VDEF_MAX   // GREEN
#define HSV_MOD_GUI           170,255,VDEF_MAX   // BLUE
#define HSV_MOD_SHIFT           0,  0,VDEF_MAX   // WHITE

// Mouse buttons, so they stand out on the LOWER layer
#define HSV_MOUSE              32,255,VDEF_MAX   // GOLD

// High-contrast thumbs (well away from all primaries)
#define HSV_THUMB_PRIMARY     4,255,VDEF_MAX   // TOMATO

static const layer_palette_t palette[LAYER_COUNT] = {
    [LAYER_BASE]        = {{HSV_TEAL_NEO},          {HSV_TANGERINE_NEO}},
    [LAYER_LOWER]       = {{HSV_FUCHSIA_NEO},       {HSV_CHARTREUSE_NEO}},
    [LAYER_RAISE]       = {{HSV_ELECTRIC_BLUE_NEO}, {HSV_ULTRAVIOLET_NEO}},
    [LAYER_NAV_3D]      = {{HSV_NEON_GREEN_NEO},    {HSV_HOT_PINK_NEO}},
};

static const struct {
    uint8_t mask;
    hsv_t   color;
} mod_colors[] = {
    {MOD_MASK_CTRL,  {HSV_MOD_CTRL}},
    {MOD_MASK_ALT,   {HSV_MOD_ALT}},
    {MOD_MASK_GUI,   {HSV_MOD_GUI}},
    {MOD_MASK_SHIFT, {HSV_MOD_SHIFT}},
};
// clang-format on

// Thumb key positions (row, col) - based on LAYOUT_let
static const uint8_t thumb_keys[][2] = {// Left side thumb keys
                                        {0, 3},
                                        {0, 2},
                                        {0, 1},
                                        {0, 7},
                                        {0, 6},
                                        {0, 5},
                                        // Right side thumb keys
                                        {7, 3},
                                        {7, 2},
                                        {7, 1},
                                        {7, 7},
                                        {7, 6},
                                        {7, 5}};

//
#define NUM_THUMB_KEYS (sizeof(thumb_keys) / sizeof(thumb_keys[0]))

// ----- Types -----
typedef enum {
    LAYER_LED_NONE     = 0,
    LAYER_LED_TAP      = 1,
    LAYER_LED_MOD      = 2,
    LAYER_LED_MODTAP   = 3,
    LAYER_LED_TO_LAYER = 4,
    LAYER_LED_TRANS    = 5,
    LAYER_LED_MOUSE    = 6,
} layer_led_type_t;

typedef enum {
    LED_ZONE_NORMAL = 0,
    LED_ZONE_THUMB  = 1,
    LED_ZONE_UNDER  = 2,
} led_zone_t;

// One byte: [layer:4 | type:4]
// For LAYER_LED_MOD the upper nibble holds the index into mod_colors instead of a layer.
typedef uint8_t layer_led_info_t;

// ----- Packing helpers -----
#define LAYER_LED_TYPE_MASK 0x0F
#define LAYER_LED_LAYER_MASK 0xF0
#define LAYER_LED_LAYER_SHIFT 4

static inline layer_led_info_t layer_led_make(uint8_t type, uint8_t layer) {
    return (uint8_t)((type & LAYER_LED_TYPE_MASK) | ((layer << LAYER_LED_LAYER_SHIFT) & LAYER_LED_LAYER_MASK));
}
static inline uint8_t layer_led_type(layer_led_info_t v) {
    return v & LAYER_LED_TYPE_MASK;
}
static inline uint8_t layer_led_layer(layer_led_info_t v) {
    return (v & LAYER_LED_LAYER_MASK) >> LAYER_LED_LAYER_SHIFT;
}

// ----- LAYER_BASE descriptor -----
typedef struct {
    led_zone_t       zone;                    // physical grouping
    layer_led_info_t layer_info[LAYER_COUNT]; // per-layer mapping
} led_info_t;

// ----- Compile-time sanity checks -----
#if __STDC_VERSION__ >= 201112L
_Static_assert(LAYER_COUNT <= 16, "LAYER_COUNT must fit in 4 bits (<=16).");
_Static_assert(LAYER_LED_MOUSE < 16, "layer_led_type_t values must fit in 4 bits (<16).");
_Static_assert(sizeof(layer_led_info_t) == 1, "layer_led_info_t must be 1 byte.");
#endif

// Lookup tables populated in keyboard_post_init_user
static led_info_t led_info[RGB_MATRIX_LED_COUNT];

// Layer locked by a one-shot tap-toggle, LAYER_COUNT when none. One-shot state only lives on
// the master half, so it is mirrored to the other half to pulse the layer key there too.
static uint8_t locked_layer = LAYER_COUNT;

static uint8_t current_locked_layer(void) {
    return (get_oneshot_layer_state() & ONESHOT_TOGGLED) ? get_oneshot_layer() : LAYER_COUNT;
}

#ifdef SPLIT_KEYBOARD
static void locked_layer_sync_handler(uint8_t in_buflen, const void *in_data, uint8_t out_buflen, void *out_data) {
    if (in_buflen == sizeof(locked_layer)) {
        locked_layer = *(const uint8_t *)in_data;
    }
}
#endif

void housekeeping_task_user(void) {
    if (!is_keyboard_master()) return;
    locked_layer = current_locked_layer();
#ifdef SPLIT_KEYBOARD
    static uint8_t  last_sent = LAYER_COUNT;
    static uint32_t last_sync = 0;
    // Send on change, and every 500ms in case a transfer was lost.
    if (locked_layer != last_sent || timer_elapsed32(last_sync) > 500) {
        if (transaction_rpc_send(RPC_ID_USER_LOCKED_LAYER, sizeof(locked_layer), &locked_layer)) {
            last_sent = locked_layer;
            last_sync = timer_read32();
        }
    }
#endif
}

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(0, 0, MIN(rgb_matrix_get_val(), VDEF_MAX));
#ifdef SPLIT_KEYBOARD
    transaction_register_rpc(RPC_ID_USER_LOCKED_LAYER, locked_layer_sync_handler);
#endif

    // Initialize lookup tables
    for (uint8_t led_index = 0; led_index < RGB_MATRIX_LED_COUNT; led_index++) {
        if (HAS_FLAGS(g_led_config.flags[led_index], LED_FLAG_UNDERGLOW)) {
            led_info[led_index].zone = LED_ZONE_UNDER;
        } else {
            // gonna get promoted later on to thumb key if necessary
            led_info[led_index].zone = LED_ZONE_NORMAL;
        }
        for (uint8_t layer = 0; layer < LAYER_COUNT; layer++) {
            led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_NONE, LAYER_COUNT); // Invalid layer
        }
    }

    // Populate keycode lookup table and identify key types
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led_index = g_led_config.matrix_co[row][col];
            if (led_index != NO_LED && led_index < RGB_MATRIX_LED_COUNT) {
                // Check if this is a thumb key
                for (uint8_t i = 0; i < NUM_THUMB_KEYS; i++) {
                    if (thumb_keys[i][0] == row && thumb_keys[i][1] == col) {
                        led_info[led_index].zone = LED_ZONE_THUMB;
                        break;
                    }
                }

                // Get keycodes for each layer and check for special keys
                for (uint8_t layer = 0; layer < LAYER_COUNT; layer++) {
                    uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col, row});
                    if (keycode == KC_TRNS) {
                        led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_TRANS, LAYER_COUNT);
                    } else if (keycode == KC_NO) {
                        led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_NONE, LAYER_COUNT);
                    } else {
                        switch (keycode) {
                            case KC_LCTL:
                            case KC_LALT:
                            case KC_LGUI:
                            case KC_LSFT:
                            case KC_RCTL:
                            case KC_RALT:
                            case KC_RGUI:
                            case KC_RSFT:
                                for (uint8_t m = 0; m < ARRAY_SIZE(mod_colors); m++) {
                                    if (mod_colors[m].mask & MOD_BIT(keycode)) {
                                        led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_MOD, m);
                                        break;
                                    }
                                }
                                break;
                            case MS_BTN1 ... MS_BTN8:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_MOUSE, LAYER_COUNT);
                                break;
                            case LOWER:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_TO_LAYER, LAYER_LOWER);
                                break;
                            case RAISE:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_TO_LAYER, LAYER_RAISE);
                                break;
                            case NAV_3D:
                                led_info[led_index].layer_info[layer] =
                                    layer_led_make(LAYER_LED_TO_LAYER, LAYER_NAV_3D);
                                break;
                            default:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_TAP, LAYER_COUNT);
                                break;
                        }
                    }
                }
            }
        }
    }
}

static inline rgb_t hsv_to_rgb_at(hsv_t color, uint8_t brightness) {
    color.v = brightness;
    return hsv_to_rgb(color);
}

// Average the colors of all held modifiers, then rescale so the brightest
// channel sits at `brightness` (averaging alone would dim mixed colors).
static bool mods_mix_color(uint8_t mods, uint8_t brightness, rgb_t *out) {
    uint16_t r = 0, g = 0, b = 0;
    uint8_t  count = 0;
    for (uint8_t m = 0; m < ARRAY_SIZE(mod_colors); m++) {
        if (mods & mod_colors[m].mask) {
            rgb_t c = hsv_to_rgb_at(mod_colors[m].color, brightness);
            r += c.r;
            g += c.g;
            b += c.b;
            count++;
        }
    }
    if (count == 0) return false;

    r /= count;
    g /= count;
    b /= count;
    uint16_t peak = MAX(r, MAX(g, b));
    if (peak > 0) {
        r = r * brightness / peak;
        g = g * brightness / peak;
        b = b * brightness / peak;
    }
    *out = (rgb_t){.r = r, .g = g, .b = b};
    return true;
}

// Color of a non-thumb key of the given type on `layer`
static rgb_t key_color(uint8_t key_type, uint8_t layer, uint8_t target_layer, bool mods_held, rgb_t mods_color,
                       uint8_t brightness) {
    switch (key_type) {
        case LAYER_LED_TAP:
        case LAYER_LED_MODTAP:
            if (mods_held) return mods_color;
            return hsv_to_rgb_at(key_type == LAYER_LED_TAP ? palette[layer].primary : palette[layer].modtap, brightness);
        case LAYER_LED_MOD:
            return hsv_to_rgb_at(mod_colors[target_layer].color, brightness);
        case LAYER_LED_MOUSE:
            return hsv_to_rgb_at((hsv_t){HSV_MOUSE}, brightness);
        case LAYER_LED_TO_LAYER:
            return hsv_to_rgb_at(palette[target_layer < LAYER_COUNT ? target_layer : layer].primary, brightness);
        default:
            return (rgb_t){RGB_BLACK};
    }
}

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    const layer_state_t layer_state_combined = layer_state | default_layer_state;
    const uint8_t       current_layer        = get_highest_layer(layer_state_combined);
    const uint8_t       brightness           = MIN(rgb_matrix_get_val(), VDEF_MAX);

    uint8_t mods = get_mods() | get_oneshot_mods() | get_oneshot_locked_mods();
    if (current_layer == LAYER_BASE && is_caps_word_on()) mods |= MOD_BIT(KC_LSFT);
    rgb_t      mods_color = {RGB_BLACK};
    const bool mods_held  = mods_mix_color(mods, brightness, &mods_color);

    // While a layer is locked, its keys pulse between 1/4 and full brightness, about once a second.
    const bool    pulsing = locked_layer < LAYER_COUNT;
    const uint8_t pulse   = 64 + scale8(sin8((uint8_t)(timer_read() / 4)), 255 - 64);

    for (uint8_t i = led_min; i < led_max; i++) {
        rgb_t color = {RGB_BLACK}; // off by default

        // ZONE: UNDERGLOW
        if (led_info[i].zone == LED_ZONE_UNDER) {
            color = hsv_to_rgb_at(palette[current_layer].primary, brightness);
        }
        // ZONE: THUMB
        else if (led_info[i].zone == LED_ZONE_THUMB) {
            layer_led_info_t info = led_info[i].layer_info[current_layer != LAYER_NAV_3D ? LAYER_BASE : LAYER_NAV_3D];
            uint8_t          key_type     = layer_led_type(info);
            uint8_t          target_layer = layer_led_layer(info);

            switch (key_type) {
                case LAYER_LED_NONE:
                    break;
                case LAYER_LED_TO_LAYER:
                    if (target_layer < LAYER_COUNT) {
                        color = hsv_to_rgb_at(palette[target_layer].primary, brightness);
                    }
                    break;
                case LAYER_LED_MOD:
                    color = hsv_to_rgb_at(mod_colors[target_layer].color, brightness);
                    break;
                case LAYER_LED_MOUSE:
                    color = hsv_to_rgb_at((hsv_t){HSV_MOUSE}, brightness);
                    break;
                default:
                    color = hsv_to_rgb_at((hsv_t){HSV_THUMB_PRIMARY}, brightness);
            }
        }
        // ZONE: NORMAL
        else {
            layer_led_info_t info     = led_info[i].layer_info[current_layer];
            uint8_t          key_type = layer_led_type(info);

            if (key_type == LAYER_LED_TRANS) {
                // Walk lower *active* layers, highest to lowest, stop at first concrete mapping
                for (int8_t fallback_layer = (int8_t)current_layer - 1; fallback_layer >= 0; --fallback_layer) {
                    if (!(layer_state_combined & (1UL << fallback_layer))) continue; // skip inactive layer
                    layer_led_info_t fallback_info = led_info[i].layer_info[fallback_layer];
                    uint8_t          fallback_type = layer_led_type(fallback_info);
                    if (fallback_type == LAYER_LED_TRANS) continue;
                    color = key_color(fallback_type, fallback_layer, layer_led_layer(fallback_info), mods_held,
                                      mods_color, brightness);
                    break;
                }
            } else {
                color = key_color(key_type, current_layer, layer_led_layer(info), mods_held, mods_color, brightness);
            }
        }

        // Pulse the layer's keys and the locked layer's own key. The layer keys live on the
        // base layer (transparent elsewhere), so look them up there whatever layer is showing.
        const layer_led_info_t base_info = led_info[i].layer_info[LAYER_BASE];
        const bool             is_locked_layer_key =
            layer_led_type(base_info) == LAYER_LED_TO_LAYER && layer_led_layer(base_info) == locked_layer;
        if (pulsing && (led_info[i].zone == LED_ZONE_NORMAL || (led_info[i].zone == LED_ZONE_THUMB && is_locked_layer_key))) {
            color.r = scale8(color.r, pulse);
            color.g = scale8(color.g, pulse);
            color.b = scale8(color.b, pulse);
        }

        // Power budget: whites and pastels light all three channels and, across the whole
        // board, draw enough current to brown out the LEDs. Cap the channel sum at 1.8x a
        // single full channel, which every layer color already fits within.
        const uint16_t budget = brightness * 9 / 5;
        const uint16_t sum    = color.r + color.g + color.b;
        if (sum > budget) {
            color.r = color.r * budget / sum;
            color.g = color.g * budget / sum;
            color.b = color.b * budget / sum;
        }

        rgb_matrix_set_color(i, color.r, color.g, color.b);
    }
    return false;
}
