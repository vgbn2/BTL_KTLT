#!/usr/bin/env bash
set -euo pipefail

# Package clean submission ZIP under single 'Nhoms4' project root
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUTPUT_ZIP="${1:-$ROOT_DIR/Nhóm 04.zip}"
TMP_DIR="$(mktemp -d /dev/shm/ktlt_pkg.XXXXXX)"
trap 'rm -rf "$TMP_DIR"' EXIT

PKG_DIR="$TMP_DIR/Nhoms4"
mkdir -p "$PKG_DIR"

# Copy source tree, build manifests, tests, and data fixtures
cp -r "$ROOT_DIR/src" "$PKG_DIR/"
cp -r "$ROOT_DIR/data" "$PKG_DIR/"
cp -r "$ROOT_DIR/tests" "$PKG_DIR/"
cp "$ROOT_DIR/CMakeLists.txt" "$PKG_DIR/"
cp "$ROOT_DIR/Makefile" "$PKG_DIR/"
cp "$ROOT_DIR/MEMBERS.md" "$PKG_DIR/"
cp "$ROOT_DIR/README.md" "$PKG_DIR/"
cp "$ROOT_DIR/run_windows.bat" "$PKG_DIR/"

# Clean up transient swap and temporary files from packaged copy
find "$PKG_DIR" -type f \( -name "*.ssd_swap" -o -name "*.tmp" -o -name "test_repo_sort.txt" \) -delete

# Archive with standard compression
cd "$TMP_DIR"
zip -qr "$OUTPUT_ZIP" Nhoms4/

echo "[SUCCESS] Packaged submission archive: $OUTPUT_ZIP"
