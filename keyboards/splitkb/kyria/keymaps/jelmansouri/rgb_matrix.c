#include <stdint.h>
#include "quantum.h"
#include "keymap.h"

typedef struct layer_palette_t {
    hsv_t primary, modtap, accent;
} layer_palette_t;

// clang-format off
// --------- Global brightness knob (one place to dim/boost everything)
#define VDEF_MAX 255

// --------- Choose a palette style
#define PALETTE_STYLE_NEO    1  // vivid/modern
#define PALETTE_STYLE_NOIR   2  // refined/jewel-toned
#define PALETTE_STYLE_PASTEL 3  // soft/pastel

#ifndef PALETTE_STYLE
#define PALETTE_STYLE PALETTE_STYLE_NEO
#endif

// =======================================================
// NEO (vivid) — unique primaries; thumbs are warm & far
// =======================================================
#if PALETTE_STYLE == PALETTE_STYLE_NEO

// Layer colors
#define HSV_TEAL_NEO          110,240,VDEF_MAX
#define HSV_TANGERINE_NEO      20,255,VDEF_MAX
#define HSV_VIOLET_NEO        200,255,VDEF_MAX

#define HSV_FUCHSIA_NEO       222,255,VDEF_MAX
#define HSV_CHARTREUSE_NEO     64,255,VDEF_MAX
#define HSV_AQUA_NEO          140,230,VDEF_MAX

#define HSV_ELECTRIC_BLUE_NEO 170,255,VDEF_MAX
#define HSV_ULTRAVIOLET_NEO   196,255,VDEF_MAX
#define HSV_AMBER_NEO          32,255,VDEF_MAX

#define HSV_NEON_GREEN_NEO     90,255,VDEF_MAX
#define HSV_HOT_PINK_NEO      234,230,VDEF_MAX
#define HSV_CRIMSON_NEO       248,255,VDEF_MAX

// High-contrast thumbs (well away from all primaries)
#define HSV_THUMB_PRIMARY     4,255,VDEF_MAX   // TOMATO
#define HSV_THUMB_SECONDARY  32,255,VDEF_MAX   // GOLD

static const layer_palette_t palette[LAYER_COUNT] = {
    [LAYER_BASE]        = {{HSV_TEAL_NEO},          {HSV_TANGERINE_NEO}, {HSV_VIOLET_NEO}},
    [LAYER_BASE_NO_HRM] = {{HSV_TEAL_NEO},          {HSV_TANGERINE_NEO}, {HSV_VIOLET_NEO}},
    [LAYER_LOWER]       = {{HSV_FUCHSIA_NEO},       {HSV_CHARTREUSE_NEO},{HSV_AQUA_NEO}},
    [LAYER_RAISE]       = {{HSV_ELECTRIC_BLUE_NEO}, {HSV_ULTRAVIOLET_NEO},{HSV_AMBER_NEO}},
    [LAYER_NAV_3D]      = {{HSV_NEON_GREEN_NEO},    {HSV_HOT_PINK_NEO},  {HSV_CRIMSON_NEO}},
};

#endif // NEO

// =======================================================
// NOIR (classy) — jewel tones; thumbs: burgundy/bronze
// =======================================================
#if PALETTE_STYLE == PALETTE_STYLE_NOIR

#define HSV_TEAL_NOIR          110,200,VDEF_MAX
#define HSV_COPPER_NOIR         18,210,VDEF_MAX
#define HSV_ROYAL_PURPLE_NOIR  195,230,VDEF_MAX

#define HSV_PLUM_NOIR          206,220,VDEF_MAX
#define HSV_OLIVE_GOLD_NOIR     52,170,VDEF_MAX
#define HSV_SMOKE_AQUA_NOIR    140,170,VDEF_MAX

#define HSV_COBALT_NOIR        170,230,VDEF_MAX
#define HSV_STEEL_BLUE_NOIR    160,160,VDEF_MAX
#define HSV_AMBER_NOIR          30,230,VDEF_MAX

#define HSV_FOREST_NOIR         92,220,VDEF_MAX
#define HSV_RASPBERRY_NOIR     236,210,VDEF_MAX
#define HSV_RUBY_NOIR            0,230,VDEF_MAX

// High-contrast thumbs
#define HSV_THUMB_PRIMARY     248,220,VDEF_MAX   // BURGUNDY
#define HSV_THUMB_SECONDARY    28,200,VDEF_MAX   // BRONZE

static const layer_palette_t palette[LAYER_COUNT] = {
    [LAYER_BASE]        = {{HSV_TEAL_NOIR},   {HSV_COPPER_NOIR},      {HSV_ROYAL_PURPLE_NOIR}},
    [LAYER_BASE_NO_HRM] = {{HSV_TEAL_NOIR},   {HSV_COPPER_NOIR},      {HSV_ROYAL_PURPLE_NOIR}},
    [LAYER_LOWER]       = {{HSV_PLUM_NOIR},   {HSV_OLIVE_GOLD_NOIR},  {HSV_SMOKE_AQUA_NOIR}},
    [LAYER_RAISE]       = {{HSV_COBALT_NOIR}, {HSV_STEEL_BLUE_NOIR},  {HSV_AMBER_NOIR}},
    [LAYER_NAV_3D]      = {{HSV_FOREST_NOIR}, {HSV_RASPBERRY_NOIR},   {HSV_RUBY_NOIR}},
};

#endif // NOIR

// =======================================================
// PASTEL (soft) — low-mid S; thumbs: cool indigo/cerulean
// =======================================================
#if PALETTE_STYLE == PALETTE_STYLE_PASTEL

#define HSV_MINT_PASTEL         115,  90,VDEF_MAX
#define HSV_PEACH_PASTEL         22, 120,VDEF_MAX
#define HSV_LILAC_PASTEL        200, 110,VDEF_MAX

#define HSV_BABY_PINK_PASTEL    238,  80,VDEF_MAX
#define HSV_PASTEL_LIME          58, 110,VDEF_MAX
#define HSV_SKY_PASTEL          160, 110,VDEF_MAX

#define HSV_APRICOT_PASTEL       28, 120,VDEF_MAX
#define HSV_POWDER_BLUE_PASTEL  170, 100,VDEF_MAX
#define HSV_PERIWINKLE_PASTEL   185, 120,VDEF_MAX

#define HSV_SAGE_PASTEL          90,  80,VDEF_MAX
#define HSV_DUSTY_ROSE_PASTEL   244,  90,VDEF_MAX
#define HSV_SOFT_RED_PASTEL       5, 110,VDEF_MAX

// High-contrast thumbs (far from all primaries: 115,238,28,90)
#define HSV_THUMB_PRIMARY      196,  95,VDEF_MAX   // INDIGO
#define HSV_THUMB_SECONDARY    170,  95,VDEF_MAX   // CERULEAN

static const layer_palette_t palette[LAYER_COUNT] = {
    [LAYER_BASE]        = {{HSV_MINT_PASTEL},   {HSV_PEACH_PASTEL},      {HSV_LILAC_PASTEL}},
    [LAYER_BASE_NO_HRM] = {{HSV_MINT_PASTEL},   {HSV_PEACH_PASTEL},      {HSV_LILAC_PASTEL}},
    [LAYER_LOWER]       = {{HSV_BABY_PINK_PASTEL}, {HSV_PASTEL_LIME},    {HSV_SKY_PASTEL}},
    [LAYER_RAISE]       = {{HSV_APRICOT_PASTEL},{HSV_POWDER_BLUE_PASTEL},{HSV_PERIWINKLE_PASTEL}},
    [LAYER_NAV_3D]      = {{HSV_SAGE_PASTEL},   {HSV_DUSTY_ROSE_PASTEL}, {HSV_SOFT_RED_PASTEL}},
};

#endif // PASTEL
// clang-format on

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
    {7, 4}};
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
    rgb_matrix_sethsv_noeeprom(0, 0, rgb_matrix_get_val());

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
    layer_state_t layer_state_combined = layer_state | default_layer_state;
    uint8_t       current_layer        = get_highest_layer(layer_state_combined);
    uint8_t       brightness           = rgb_matrix_get_val();
    bool          modifier_held = (get_mods() | get_weak_mods() | get_oneshot_mods() | get_oneshot_locked_mods()) != 0;

    for (uint8_t i = led_min; i < led_max; i++) {
        hsv_t color = (hsv_t){HSV_BLACK}; // off by default

        // ZONE: UNDERGLOW
        if (led_info[i].zone == LED_ZONE_UNDER) {
            color   = palette[current_layer].primary; // use .accent here if you prefer
            color.v = brightness;
        }
        // ZONE: THUMB
        else if (led_info[i].zone == LED_ZONE_THUMB) {
            layer_led_info_t info = led_info[i].layer_info[current_layer != LAYER_NAV_3D ? LAYER_BASE : LAYER_NAV_3D];
            uint8_t          key_type     = layer_led_type(info);
            uint8_t          target_layer = layer_led_layer(info);

            if (key_type == LAYER_LED_TO_LAYER && target_layer < LAYER_COUNT) {
                color = palette[target_layer].primary;
            } else if (key_type == LAYER_LED_MOD) {
                color = (hsv_t){HSV_THUMB_SECONDARY};
            } else {
                color = (hsv_t){HSV_THUMB_PRIMARY};
            }
            color.v = brightness;
        }
        // ZONE: NORMAL
        else {
            layer_led_info_t info         = led_info[i].layer_info[current_layer];
            uint8_t          key_type     = layer_led_type(info);
            uint8_t          target_layer = layer_led_layer(info);

            switch (key_type) {
                case LAYER_LED_NONE:
                    // keep off
                    break;

                case LAYER_LED_TRANS: {
                    // Walk lower *active* layers, highest to lowest
                    if (current_layer > 0) {
                        for (int8_t fallback_layer = (int8_t)current_layer - 1; fallback_layer >= 0; --fallback_layer) {
                            if (!(layer_state_combined & (1UL << fallback_layer))) continue; // skip inactive layer
                            layer_led_info_t fallback_info   = led_info[i].layer_info[fallback_layer];
                            uint8_t          fallback_type   = layer_led_type(fallback_info);
                            uint8_t          fallback_target = layer_led_layer(fallback_info);
                            if (fallback_type == LAYER_LED_NONE) break;
                            if (fallback_type == LAYER_LED_TRANS) continue;

                            switch (fallback_type) {
                                case LAYER_LED_TAP:
                                    color = modifier_held ? palette[fallback_layer].accent
                                                          : palette[fallback_layer].primary;
                                    break;
                                case LAYER_LED_MOD:
                                case LAYER_LED_MODTAP:
                                    color =
                                        modifier_held ? palette[fallback_layer].accent : palette[fallback_layer].modtap;
                                    break;
                                case LAYER_LED_TO_LAYER:
                                    color = (fallback_target < LAYER_COUNT) ? palette[fallback_target].primary
                                                                            : palette[fallback_layer].primary;
                                    break;
                            }
                            color.v = brightness;
                            break; // stop at first concrete mapping
                        }
                    }
                } break;

                case LAYER_LED_TAP:
                    color   = modifier_held ? palette[current_layer].accent : palette[current_layer].primary;
                    color.v = brightness;
                    break;

                case LAYER_LED_MOD:
                case LAYER_LED_MODTAP:
                    color   = modifier_held ? palette[current_layer].accent : palette[current_layer].modtap;
                    color.v = brightness;
                    break;

                case LAYER_LED_TO_LAYER:
                    color =
                        (target_layer < LAYER_COUNT) ? palette[target_layer].primary : palette[current_layer].primary;
                    color.v = brightness;
                    break;
            }
        }

        // Single HSV->RGB conversion at the end per LED (no helper function)
        RGB rgb = hsv_to_rgb(color);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return false;
}
