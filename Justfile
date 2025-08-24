# Use bash with strict flags
set shell := ["bash", "-eu", "-o", "pipefail", "-c"]

# ---------- Config ----------
# Either set KEYBOARD and KEYMAP (recommended) ...
#   just KEYBOARD=ferris/sweep KEYMAP=default all
# ...or point KEYMAP_C at a specific keymap.c (then KEYBOARD/KEYMAP are optional)
#   just KEYMAP_C=../qmk_firmware/keyboards/ferris/sweep/keymaps/default/keymap.c all
KEYBOARD := "splitkb/kyria/rev3"
KEYMAP   := "jelmansouri"

# Column count for keymap-drawer parsing
COLS := "10"

# Output directory
OUT := "assets"

# Layers to draw (space-separated)
LAYERS := "Base BaseNoHRM Lower Raise Nav3D"

# ---------- Phonies ----------
default: all

# End-to-end
all: draw

# 1) keymap.c -> keymap.json
json:
    mkdir -p "{{OUT}}"
    # Use --no-cpp by default (fewer surprises). Drop it if you need macros expanded.
    qmk c2json --no-cpp -kb "{{KEYBOARD}}" -km "{{KEYMAP}}" > "{{OUT}}/keymap.json"

# 2) keymap.json -> keymap.yaml
parse: json
    echo "Parsing JSON -> YAML (cols={{COLS}})"
    keymap -c "{{OUT}}/keymap-config.yaml" parse --layer-names {{LAYERS}} -c {{COLS}} -q "{{OUT}}/keymap.json" > "{{OUT}}/keymap.yaml"

# 3) Draw each layer to its own SVG
draw: parse
    #!/usr/bin/env bash
    set -euo pipefail
    keymap draw "{{OUT}}/keymap.yaml" -o "{{OUT}}/keymap.svg";
    for L in {{LAYERS}}; do
        echo "Drawing layer: $L"
        keymap draw "{{OUT}}/keymap.yaml" -s "$L" -o "{{OUT}}/keymap_$L.svg";
        echo "SVG written to {{OUT}}/keymap_$L.svg"
    done

# Cleanup
clean:
    rm -rf "{{OUT}}"
