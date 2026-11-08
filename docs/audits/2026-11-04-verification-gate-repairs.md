# Verification gate repairs

Date: 2026-11-04. Follow-up authorized by “fix the remaining test and anti-cheat failures.”

## Startup CTest

- Fault domain: `our_source` / repository test packaging. `CMakeLists.txt:78–81` registers `tests/repro/startup_working_directory.sh`, but the file was staged for deletion and absent on disk.
- A/B probe: HEAD contains the complete 15-line script; working-tree invocation failed before application execution. The divergence is test-command ingestion, not application startup behavior.
- Exit-1 baseline minted in `tests/repro/repro_20261104_012543_startup_ctest_missing_script.sh`. Its driver/build are session-local; the restored CMake-registered script is the durable regression check.
- Restored only that script from HEAD without changing its assertions or changing the index. Other historical deletions remain untouched.
- Original assertions require no launch-directory `data/` creation, empty stderr, and the actual startup synchronization message. The script copies the executable and data into its own disposable directory before launching from a different directory.
- Registered focused CTest passed 1/1. Fresh four-target build passed. Full CTest passed 3/3 (unit, five-second stress, startup), exit 0, in 5.08 seconds using copied-data working directories and an isolated data argument. Live-data SHA-256 values were unchanged. This is not a native source-cwd CTest run.

## Anti-cheat enforcer

- Fault domain: `our_source`, owned by `tools/dev-productivity/src/anti-cheat/main.go`, not the C++ application. `checkDiff` called `analyzeDiff` twice with independent required-test checks; tests in one Git diff group could not satisfy production changes in the other.
- Exit-1 baseline minted in `tests/repro/repro_20261104_013032_integrity_split_index_coverage.sh`; this wrapper depends on the local tool checkout. Permanent regression: `tools/dev-productivity/src/anti-cheat/main_test.go` in the sibling tools repository.
- Repair: share production/test filename sets between the two analyses and evaluate coverage once afterward. Assertion balances and all other tampering checks remain local to each diff group. Filename classification and untracked-file discovery are unchanged.
- Real temporary Git repositories prove both split-index directions accept. Production-only changes still reject; assertion removal still rejects in either group. Each accepted fixture executes its real Python assertion successfully. Original regression assertions were not changed after the failing baseline.
- `go test -v main.go main_test.go` passed both subtests (exit 0). Candidate build passed; candidate and rebuilt installed CLI both passed `--diff --require-tests --no-revert` on KTLT (exit 0). Installed target: `tools/dev-productivity/bin/anti-cheat-enforcer`, reached by `~/.local/bin/anti-cheat-enforcer`.
- Installation preserved the prior binary in job scratch as `anti-cheat-enforcer-before-repair`; installed SHA-256 matches verified candidate `cf4a8b6bbf037909b8b9482c894cd933a778be6d8c4a20f53dd1c663a9aa71f0`.
- Final isolated CTest passed 3/3 in 5.08 seconds, exit 0; live data unchanged. Both repositories passed `git diff --check`. KTLT index hash was unchanged; no staging, rule disabling, test weakening, auto-revert, or bulk historical restoration occurred.
- Deferred pre-existing limitations: basename-only test classification, untracked files omitted by Git diff, ignored Git collection errors, and misleading strike-log auto-revert wording under `--no-revert`. A gate pass is not proof that those limitations are resolved.

## Final acceptance evidence

- Fresh native runner: 506 passed, 0 failed, exit 0; before/after live-data hashes match the implementation baseline. Expected invalid-BTS fixture diagnostics are not assertion failures.
- Fresh post-documentation installed enforcer: exit 0 with `--diff --require-tests --no-revert`.
- Approved closeout rerun: native runner 506/0; isolated CTest 3/3 in 5.10 seconds; installed integrity gate exit 0; both repositories' whitespace checks pass. Live data matches the implementation baseline. Executed-diff record refreshed to include the latest session-memory change.
- Full executed source/test/documentation diffs: [verification gate diffs](2026-11-04-verification-gate-diffs.md). The startup restoration is shown below. Tracked diffs are against HEAD, not a pristine pre-session snapshot.

- Independent scoped verifier: `go test -v -race main.go main_test.go` passes both real-Git subtests (exit 0); installed gate exit 0; startup working copy matches HEAD (546 bytes), with staged deletion preserved. No concrete regression found in shared coverage/per-group tampering handling. This is not a whole-tool audit.

## Preservation and delivery

The startup script exists in the working tree but its pre-existing staged deletion remains in the index. Inspect that entry before any future commit; no staging or commit was authorized. Other historical deletions, submitted ZIPs, Git history and timestamps remain untouched. The tool source/test/binary changes live in the sibling tools checkout, not KTLT.

## Executed startup diff

The pre-repair working-tree path was absent; the restored contents exactly match HEAD:

```diff
--- /dev/null
+++ tests/repro/startup_working_directory.sh
@@ -0,0 +1,15 @@
+#!/usr/bin/env bash
+set -euo pipefail
+
+executable="$1"
+source_data="$2"
+workdir="$(mktemp -d /dev/shm/ktlt-startup-test.XXXXXX)"
+trap 'rm -rf "$workdir"' EXIT
+mkdir -p "$workdir/bin/data" "$workdir/launch"
+cp "$executable" "$workdir/bin/quanlythuebao"
+cp "$source_data"/*.txt "$workdir/bin/data/"
+
+printf '0\n' | (cd "$workdir/launch" && "$workdir/bin/quanlythuebao" >"$workdir/output.txt" 2>"$workdir/error.txt")
+test ! -e "$workdir/launch/data"
+test ! -s "$workdir/error.txt"
+grep -q 'Da dong bo toan bo du lieu he thong' "$workdir/output.txt"
```
