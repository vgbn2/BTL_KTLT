#!/usr/bin/env bash
set -euo pipefail

executable="$1"
source_data="$2"
workdir="$(mktemp -d /dev/shm/ktlt-startup-test.XXXXXX)"
trap 'rm -rf "$workdir"' EXIT
mkdir -p "$workdir/bin/data" "$workdir/launch"
cp "$executable" "$workdir/bin/quanlythuebao"
cp "$source_data"/*.txt "$workdir/bin/data/"

printf '0\n' | (cd "$workdir/launch" && "$workdir/bin/quanlythuebao" >"$workdir/output.txt" 2>"$workdir/error.txt")
test ! -e "$workdir/launch/data"
test ! -s "$workdir/error.txt"
grep -q 'Da dong bo toan bo du lieu he thong thanh cong' "$workdir/output.txt"
