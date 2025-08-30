#include <stdint.h>
#include "quantum.h"
#include "keymap.h"

typedef struct layer_palette_t {
    hsv_t primary, modtap, accent;
} layer_palette_t;

// clang-format off
static const layer_palette_t palette[LAYER_COUNT] = {
    [LAYER_BASE]        = {{HSV_CYAN},    {HSV_CORAL},  {HSV_ORANGE}},
    [LAYER_BASE_NO_HRM] = {{HSV_CYAN},    {HSV_CORAL},  {HSV_ORANGE}},
    [LAYER_LOWER]       = {{HSV_MAGENTA}, {HSV_YELLOW}, {HSV_CYAN}},
    [LAYER_RAISE]       = {{HSV_ORANGE},  {HSV_AZURE},  {HSV_PURPLE}},
    [LAYER_NAV_3D]      = {{HSV_GREEN},   {HSV_PINK},   {HSV_RED}},
};
// clang-format on

#define THUMB_COLOR HSV_GOLDENROD

// Thumb key positions (row, col) - based on LAYOUT_split_3x6_5
static const uint8_t thumb_keys[][2] = {
    // Left side thumb keys
    {2, 0},
    {3, 3},
    {3, 4},
    {3, 2},
    {3, 1},
    {3, 5},
    {3, 0},
    // Right side thumb keys
    {6, 0},
    {7, 3},
    {7, 0},
    {7, 5},
    {7, 1},
    {7, 2},
    {3, 4}};
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
} layer_led_type_t;

typedef enum {
    LED_ZONE_NORMAL = 0,
    LED_ZONE_THUMB  = 1,
    LED_ZONE_UNDER  = 2,
} led_zone_t;

// One byte: [layer:4 | type:4]
typedef uint8_t layer_led_info_t;

// ----- Packing helpers -----
#define LAYER_LED_TYPE_MASK 0x0F
#define LAYER_LED_LAYER_MASK 0xF0
#define LAYER_LED_LAYER_SHIFT 4

static inline layer_led_info_t layer_led_make(uint8_t type, uint8_t layer) {
    return (uint8_t)((type & LAYER_LED_TYPE_MASK) | ((layer & LAYER_LED_TYPE_MASK) << LAYER_LED_LAYER_SHIFT));
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
_Static_assert(LAYER_LED_TRANS < 16, "layer_led_type_t values must fit in 4 bits (<16).");
_Static_assert(sizeof(layer_led_info_t) == 1, "layer_led_info_t must be 1 byte.");
#endif

// Lookup tables populated in keyboard_post_init_user
static led_info_t led_info[RGB_MATRIX_LED_COUNT];

void keyboard_post_init_user(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(HSV_OFF);

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
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_MOD, LAYER_COUNT);
                                break;
                            case R_CTL:
                            case S_ALT:
                            case T_GUI:
                            case N_GUI:
                            case E_ALT:
                            case I_CTL:
                            case F6_CTL:
                            case F7_ALT:
                            case F8_GUI:
                            case N4_GUI:
                            case N5_ALT:
                            case N6_CTL:
                            case AM_CTL:
                            case AS_ALT:
                            case LP_GUI:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_MODTAP, LAYER_COUNT);
                                break;
                            case BASE:
                                led_info[led_index].layer_info[layer] = layer_led_make(LAYER_LED_TO_LAYER, LAYER_BASE);
                                break;
                            case BWOHRM:
                                led_info[led_index].layer_info[layer] =
                                    layer_led_make(LAYER_LED_TO_LAYER, LAYER_BASE_NO_HRM);
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

bool rgb_matrix_indicators_advanced_user(uint8_t led_min, uint8_t led_max) {
    uint8_t current_layer = get_highest_layer(layer_state | default_layer_state);
    uint8_t brightness    = rgb_matrix_get_val();
    bool    modifier_held = get_mods() || get_oneshot_mods() || get_weak_mods();

    for (uint8_t i = led_min; i < led_max; i++) {
        hsv_t color = {0, 0, 0}; // Off by default

        // Handle underglow
        if (led_info[i].zone == LED_ZONE_UNDER) {
            color   = palette[current_layer].primary;
            color.v = brightness;
        }
        // Handle thumb keys
        else if (led_info[i].zone == LED_ZONE_THUMB) {
            layer_led_info_t info         = led_info[i].layer_info[current_layer];
            uint8_t          led_type     = layer_led_type(info);
            uint8_t          target_layer = layer_led_layer(info);

            if (led_type == LAYER_LED_TO_LAYER && target_layer < LAYER_COUNT) {
                // Layer transition key - use target layer color
                color = palette[target_layer].primary;
            } else {
                // Regular thumb key - use constant color
                color = (hsv_t){THUMB_COLOR};
            }
            color.v = brightness;
        }
        // Handle normal keys
        else if (led_info[i].zone == LED_ZONE_NORMAL) {
            layer_led_info_t info         = led_info[i].layer_info[current_layer];
            uint8_t          led_type     = layer_led_type(info);
            uint8_t          target_layer = layer_led_layer(info);

            switch (led_type) {
                case LAYER_LED_NONE:
                    // Key is off - keep default {0, 0, 0}
                    break;

                case LAYER_LED_TRANS:
                    // Transparent - find the first non-transparent layer below
                    for (int8_t fallback_layer = current_layer - 1; fallback_layer >= 0; fallback_layer--) {
                        layer_led_info_t fallback_info   = led_info[i].layer_info[fallback_layer];
                        uint8_t          fallback_type   = layer_led_type(fallback_info);
                        uint8_t          fallback_target = layer_led_layer(fallback_info);

                        if (fallback_type != LAYER_LED_TRANS && fallback_type != LAYER_LED_NONE) {
                            switch (fallback_type) {
                                case LAYER_LED_TAP:
                                    if (modifier_held) {
                                        color = palette[fallback_layer].accent;
                                    } else {
                                        color = palette[fallback_layer].primary;
                                    }
                                    break;
                                case LAYER_LED_MODTAP:
                                case LAYER_LED_MOD:
                                    if (modifier_held) {
                                        color = palette[fallback_layer].accent;
                                    } else {
                                        color = palette[fallback_layer].modtap;
                                    }
                                    break;
                                case LAYER_LED_TO_LAYER:
                                    if (fallback_target < LAYER_COUNT) {
                                        color = palette[fallback_target].primary;
                                    } else {
                                        color = palette[fallback_layer].primary;
                                    }
                                    break;
                            }
                            color.v = brightness;
                            break; // Found non-transparent key, stop searching
                        }
                    }
                    break;

                case LAYER_LED_TAP:
                    if (modifier_held) {
                        // When modifier held, use accent color
                        color   = palette[current_layer].accent;
                        color.v = brightness;
                    } else {
                        // Regular key - use layer primary color
                        color   = palette[current_layer].primary;
                        color.v = brightness;
                    }
                    break;

                case LAYER_LED_MODTAP:
                case LAYER_LED_MOD:
                    if (modifier_held) {
                        // When modifier held, use accent color
                        color   = palette[current_layer].accent;
                        color.v = brightness;
                    } else {
                        // Pure modifier - use mod-tap color
                        color   = palette[current_layer].modtap;
                        color.v = brightness;
                    }
                    break;

                case LAYER_LED_TO_LAYER:
                    // Layer transition key
                    if (target_layer < LAYER_COUNT) {
                        // Normal state - show target layer color
                        color = palette[target_layer].primary;
                    } else {
                        // Fallback to layer primary
                        color = palette[current_layer].primary;
                    }
                    color.v = brightness;
                    break;
            }
        }

        rgb_matrix_set_color(i, color.h, color.s, color.v);
    }
    return false;
}
