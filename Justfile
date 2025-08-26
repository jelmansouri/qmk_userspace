# Use bash with strict flags
set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

KEYBOARD := "splitkb/kyria/rev3"
KEYMAP   := "jelmansouri"

LAYOUT_DRAWINGS_OUT := "assets/layout_drawings/generated"
LAYOUT_DRAWINGS_CONFIG := "assets/layout_drawings/keymap-config.yaml"

LAYER_NAMES := "Base BaseNoHRM Lower Raise Nav3D"

MODIFIERS_ART := "assets/screen_art/modifiers/*.png"
MODIFIERS_ART_OUT := "keyboards/splitkb/halcyon/kyria/keymaps/jelmansouri_hlc/graphics/modifiers"
LAYERS_ART := "assets/screen_art/layers/*.png"
LAYERS_ART_OUT := "keyboards/splitkb/halcyon/kyria/keymaps/jelmansouri_hlc/graphics/layers"

# ---------- Phonies ----------
default: all

all: draw modifiers layers

json:
    mkdir -p "{{LAYOUT_DRAWINGS_OUT}}"
    # Use --no-cpp by default (fewer surprises). Drop it if you need macros expanded.
    qmk c2json --no-cpp -kb "{{KEYBOARD}}" -km "{{KEYMAP}}" > "{{LAYOUT_DRAWINGS_OUT}}/keymap.json"

parse: json
    keymap -c "{{LAYOUT_DRAWINGS_CONFIG}}" parse --layer-names {{LAYER_NAMES}} -c 10 -q "{{LAYOUT_DRAWINGS_OUT}}/keymap.json" > "{{LAYOUT_DRAWINGS_OUT}}/keymap.yaml"

draw: parse
    #!/usr/bin/env bash
    set -euo pipefail
    keymap draw "{{LAYOUT_DRAWINGS_OUT}}/keymap.yaml" -o "{{LAYOUT_DRAWINGS_OUT}}/keymap.svg";
    for L in {{LAYER_NAMES}}; do
        echo "Drawing layer: $L"
        keymap draw "{{LAYOUT_DRAWINGS_OUT}}/keymap.yaml" -s "$L" -o "{{LAYOUT_DRAWINGS_OUT}}/keymap_$L.svg";
        echo "SVG written to {{LAYOUT_DRAWINGS_OUT}}/keymap_$L.svg"
    done

modifiers:
    #!/usr/bin/env bash
    set -euo pipefail
    shopt -s nullglob dotglob
    for MA in {{MODIFIERS_ART}}; do
        echo "Converting graphics for: $MA"
        qmk painter-convert-graphics -o "{{MODIFIERS_ART_OUT}}" -f mono16 -i "$MA"
    done

layers:
    #!/usr/bin/env bash
    set -euo pipefail
    for LA in {{LAYERS_ART}}; do
        echo "Converting graphics for: $LA"
        qmk painter-convert-graphics -o "{{LAYERS_ART_OUT}}" -f pal16 -i "$LA"
    done

# Cleanup
clean:
    rm "{{LAYOUT_DRAWINGS_OUT}}/*"
    rm "{{MODIFIERS_ART_OUT}}/*"
    rm "{{LAYERS_ART_OUT}}/*"
