#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

KEYMAP_C="${REPO_ROOT}/keyboards/cyboard/imprint/imprint_letters_only_2key_bottom_row/keymaps/jelmansouri/keymap.c"
CONFIG_YAML="${SCRIPT_DIR}/keymap-config.yaml"
INFO_JSON="${SCRIPT_DIR}/imprint_info.json"
OUT_DIR="${SCRIPT_DIR}/generated"
TMP_JSON="$(mktemp -t imprint_keymap).json"

cleanup() {
    rm -f "${TMP_JSON}"
}
trap cleanup EXIT

mkdir -p "${OUT_DIR}"

qmk c2json --no-cpp \
    -kb cyboard/imprint/imprint_letters_only_2key_bottom_row \
    -km jelmansouri \
    "${KEYMAP_C}" \
    -o "${TMP_JSON}"

uvx --from keymap-drawer keymap -c "${CONFIG_YAML}" parse \
    -q "${TMP_JSON}" \
    -l Base BaseNoHRM Lower Raise Nav3D \
    -o "${OUT_DIR}/imprint_keymap.yaml"

uvx --from keymap-drawer keymap -c "${CONFIG_YAML}" draw \
    -j "${INFO_JSON}" \
    -l LAYOUT_let \
    "${OUT_DIR}/imprint_keymap.yaml" \
    -o "${OUT_DIR}/imprint_keymap.svg"

for layer in Base BaseNoHRM Lower Raise Nav3D; do
    uvx --from keymap-drawer keymap -c "${CONFIG_YAML}" draw \
        -j "${INFO_JSON}" \
        -l LAYOUT_let \
        "${OUT_DIR}/imprint_keymap.yaml" \
        -s "${layer}" \
        -o "${OUT_DIR}/imprint_keymap_${layer}.svg"
done

echo "Generated imprint drawings in ${OUT_DIR}"
