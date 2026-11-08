# Verification gate executed diffs

Tracked diffs are against HEAD; pre-existing documentation content is not attributable to this repair. New test is complete. Startup exact restoration is in the companion repair report.

```diff
diff --git a/docs/audits/2026-11-04-verification-gate-repairs.md b/docs/audits/2026-11-04-verification-gate-repairs.md
new file mode 100644
index 0000000..e81d26e
--- /dev/null
+++ b/docs/audits/2026-11-04-verification-gate-repairs.md
@@ -0,0 +1,61 @@
+# Verification gate repairs
+
+Date: 2026-11-04. Follow-up authorized by “fix the remaining test and anti-cheat failures.”
+
+## Startup CTest
+
+- Fault domain: `our_source` / repository test packaging. `CMakeLists.txt:78–81` registers `tests/repro/startup_working_directory.sh`, but the file was staged for deletion and absent on disk.
+- A/B probe: HEAD contains the complete 15-line script; working-tree invocation failed before application execution. The divergence is test-command ingestion, not application startup behavior.
+- Exit-1 baseline minted in `tests/repro/repro_20261104_012543_startup_ctest_missing_script.sh`. Its driver/build are session-local; the restored CMake-registered script is the durable regression check.
+- Restored only that script from HEAD without changing its assertions or changing the index. Other historical deletions remain untouched.
+- Original assertions require no launch-directory `data/` creation, empty stderr, and the actual startup synchronization message. The script copies the executable and data into its own disposable directory before launching from a different directory.
+- Registered focused CTest passed 1/1. Fresh four-target build passed. Full CTest passed 3/3 (unit, five-second stress, startup), exit 0, in 5.08 seconds using copied-data working directories and an isolated data argument. Live-data SHA-256 values were unchanged. This is not a native source-cwd CTest run.
+
+## Anti-cheat enforcer
+
+- Fault domain: `our_source`, owned by `tools/dev-productivity/src/anti-cheat/main.go`, not the C++ application. `checkDiff` called `analyzeDiff` twice with independent required-test checks; tests in one Git diff group could not satisfy production changes in the other.
+- Exit-1 baseline minted in `tests/repro/repro_20261104_013032_integrity_split_index_coverage.sh`; this wrapper depends on the local tool checkout. Permanent regression: `tools/dev-productivity/src/anti-cheat/main_test.go` in the sibling tools repository.
+- Repair: share production/test filename sets between the two analyses and evaluate coverage once afterward. Assertion balances and all other tampering checks remain local to each diff group. Filename classification and untracked-file discovery are unchanged.
+- Real temporary Git repositories prove both split-index directions accept. Production-only changes still reject; assertion removal still rejects in either group. Each accepted fixture executes its real Python assertion successfully. Original regression assertions were not changed after the failing baseline.
+- `go test -v main.go main_test.go` passed both subtests (exit 0). Candidate build passed; candidate and rebuilt installed CLI both passed `--diff --require-tests --no-revert` on KTLT (exit 0). Installed target: `tools/dev-productivity/bin/anti-cheat-enforcer`, reached by `~/.local/bin/anti-cheat-enforcer`.
+- Installation preserved the prior binary in job scratch as `anti-cheat-enforcer-before-repair`; installed SHA-256 matches verified candidate `cf4a8b6bbf037909b8b9482c894cd933a778be6d8c4a20f53dd1c663a9aa71f0`.
+- Final isolated CTest passed 3/3 in 5.08 seconds, exit 0; live data unchanged. Both repositories passed `git diff --check`. KTLT index hash was unchanged; no staging, rule disabling, test weakening, auto-revert, or bulk historical restoration occurred.
+- Deferred pre-existing limitations: basename-only test classification, untracked files omitted by Git diff, ignored Git collection errors, and misleading strike-log auto-revert wording under `--no-revert`. A gate pass is not proof that those limitations are resolved.
+
+## Final acceptance evidence
+
+- Fresh native runner: 506 passed, 0 failed, exit 0; before/after live-data hashes match the implementation baseline. Expected invalid-BTS fixture diagnostics are not assertion failures.
+- Fresh post-documentation installed enforcer: exit 0 with `--diff --require-tests --no-revert`.
+- Approved closeout rerun: native runner 506/0; isolated CTest 3/3 in 5.10 seconds; installed integrity gate exit 0; both repositories' whitespace checks pass. Live data matches the implementation baseline. Executed-diff record refreshed to include the latest session-memory change.
+- Full executed source/test/documentation diffs: [verification gate diffs](2026-11-04-verification-gate-diffs.md). The startup restoration is shown below. Tracked diffs are against HEAD, not a pristine pre-session snapshot.
+
+- Independent scoped verifier: `go test -v -race main.go main_test.go` passes both real-Git subtests (exit 0); installed gate exit 0; startup working copy matches HEAD (546 bytes), with staged deletion preserved. No concrete regression found in shared coverage/per-group tampering handling. This is not a whole-tool audit.
+
+## Preservation and delivery
+
+The startup script exists in the working tree but its pre-existing staged deletion remains in the index. Inspect that entry before any future commit; no staging or commit was authorized. Other historical deletions, submitted ZIPs, Git history and timestamps remain untouched. The tool source/test/binary changes live in the sibling tools checkout, not KTLT.
+
+## Executed startup diff
+
+The pre-repair working-tree path was absent; the restored contents exactly match HEAD:
+
+```diff
+--- /dev/null
++++ tests/repro/startup_working_directory.sh
+@@ -0,0 +1,15 @@
++#!/usr/bin/env bash
++set -euo pipefail
++
++executable="$1"
++source_data="$2"
++workdir="$(mktemp -d /dev/shm/ktlt-startup-test.XXXXXX)"
++trap 'rm -rf "$workdir"' EXIT
++mkdir -p "$workdir/bin/data" "$workdir/launch"
++cp "$executable" "$workdir/bin/quanlythuebao"
++cp "$source_data"/*.txt "$workdir/bin/data/"
++
++printf '0\n' | (cd "$workdir/launch" && "$workdir/bin/quanlythuebao" >"$workdir/output.txt" 2>"$workdir/error.txt")
++test ! -e "$workdir/launch/data"
++test ! -s "$workdir/error.txt"
++grep -q 'Da dong bo toan bo du lieu he thong' "$workdir/output.txt"
+```

diff --git a/docs/audits/2026-11-04-menu-parity-implementation.md b/docs/audits/2026-11-04-menu-parity-implementation.md
new file mode 100644
index 0000000..019ce59
--- /dev/null
+++ b/docs/audits/2026-11-04-menu-parity-implementation.md
@@ -0,0 +1,80 @@
+# Menu update parity implementation
+
+Date: 2026-11-04. Baseline: [create/update audit](2026-11-03-create-update-parity.md).
+
+## Batch ledger
+
+| Batch | Ownership and behavior | State | Evidence |
+| --- | --- | --- | --- |
+| CP-01/CP-02 | Package menu: optional data/minutes/cycle setters | verified/reviewed; feature closed, aggregate gate exception retained | Exit-1 baseline: 447 passed/14 failed; after fix: 461 passed/0 failed |
+| META-01 | Device menu: action 5 corrects labels only | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 479 passed/0 failed; PTY and quick stress exit 0 |
+| META-02 | Customer menu: optional issue date/authority | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 506 passed/0 failed; PTY, quick stress and lifecycle exit 0 |
+
+## Package evidence
+
+- Reproducer: `tests/repro/repro_20261104_004646_package_update_parity.sh`; mint confirmed exit 1 before production edits. Its command uses this session's isolated build driver; durable assertions live in `tests/modules/test_menu_parity.cpp` and are registered in CMake.
+- Debug C++11 application/unit/stress/lifecycle build: exit 0.
+- Unit runner in copied-data cwd with source symlink: 461 passed, 0 failed, exit 0. Cases cover independent fields, unchanged values, zero, invalid numeric retries, cycle 1/365, EOF cancellation and missing ID. Existing assertions retained.
+- Quick stress `--duration 5`: exit 0; 7,340 menu invocations, not a successful-update count. Debug build is not sanitizer qualification despite the runner's static banner.
+- POSIX PTY probes: package persisted `AUDIT|Original|60000|10|250|60`; associated contract serialization retained price 50000 and expiry 31/01/2024. Failed-save case obstructed only an isolated fixture parent, reported an error, restored memory and retained the original disk record. Empty lookup created no package. All exited 0.
+- Before/after menu classifier: 58 functions, no placement violations/duplicates. YAGNI auditor: exit 0.
+- Adversarial inspection: zero remains a value, setters receive validated values, EOF returns before save, one repository update, no identity/history propagation.
+- Live data hashes unchanged throughout isolated runs.
+
+## Device evidence
+
+- Reproducer `tests/repro/repro_20261104_005228_device_metadata_update.sh` confirmed exit 1 before production changes; direct setters were exercised by expected-record construction while the existing menu rejected action 5.
+- Registered real menu/reload cases cover both labels, blanks, 50/51-character boundaries, pipe retry, EOF, cancellation and missing ID; final runner: 479 passed, zero failed.
+- PTY reload: `860123456789014|New Model|New Brand|0981234567|15/01/2024|KhoaMang|BTS-HN-001`. Identity, mapping, activation date, blacklist and BTS preserved.
+- Quick stress exit 0 (7,572 menu invocations); live data hashes unchanged. Existing SIM/blacklist tests retained and passed in unit runner.
+- Review: action numbers 1–4 unchanged; validated inputs finish before setters and one repository save; EOF cannot publish the first label. Classifier/YAGNI/whitespace pass; aggregate integrity exception unchanged.
+
+## Customer evidence
+
+- Reproducer `tests/repro/repro_20261104_005414_customer_issue_metadata.sh` confirmed exit 1 before implementation. Existing menu saved after address without reading issue metadata; direct setters/serialized reload supported the expected fields.
+- Final units: 506 passed, zero failed. Real reload cases cover leap day, authority-only edit, blank preservation, delimiter retry, cancellation of earlier name edits, invalid leap day/trailing text, EOF before authority, missing ID and `now` across midnight.
+- PTY reload: `KH0001|Nguyen Van An|CCCD|001201004567|29/02/2024|Cuc C08|0981234567|Ha Noi`.
+- Debug C++11 application/unit/stress/lifecycle build exit 0. Quick stress exit 0 (7,598 menu invocations); lifecycle `--quick` exit 0, all five repository counts returned to baseline, zero residual records. Counts are runner metrics, not independently proven successful-update totals.
+- Review: date is explicitly parsed because model setters only assign; ID/CCCD/type remain immutable; one save after all inputs; invalid date/cancel/EOF leaves pending copy unpublished. Approved three lifecycle input payload corrections applied without assertion changes.
+- Classifier/YAGNI/whitespace pass. Live data hashes unchanged. Aggregate anti-cheat exception retained.
+
+## Final verification and limitations
+
+Follow-up gate repair (2026-11-04) supersedes the historical red-gate results below: startup restored with original assertions; isolated CTest now passes 3/3; rebuilt installed anti-cheat passes with `--no-revert`. See [verification gate repairs](2026-11-04-verification-gate-repairs.md) for root causes, real-Git regressions and staging caveat. Historical failures remain recorded, not erased.
+
+- Fresh isolated unit rerun on 2026-11-04: 506 passed, 0 failed, exit 0; live-data SHA-256 values match both the before-run values and implementation baseline. Expected invalid-BTS-fixture diagnostic is not a failed assertion.
+- Isolated CTest: 2/3 passed, exit 8. `unit_tests` and `stress_test_quick` passed. `startup_from_other_directory` failed with `/usr/bin/bash: /home/vgbn/Documents/codeptit/KTLT/tests/repro/startup_working_directory.sh: No such file or directory`. The script's staged deletion predates implementation and remains untouched. The harness retained registered commands but changed their working directories and live-data argument to a copied fixture; this is not an untouched source-cwd CTest execution.
+- Documentation referee reported 45 stale-reference candidates across six existing documents. Shorthand paths, historical archive paths and missing historical files require individual screening; do not label all candidates confirmed defects. No broad documentation repair was attempted.
+- `graphify update .` exited 0: 1011 nodes, 2203 edges, 43 communities; regenerated graph JSON/HTML, report and one callflow. Saved labels describe 61 communities, so 25 current communities received hub-derived names. No LLM relabeling performed. Tool-created backup retained under `graphify-out/2026-11-04/`: `.graphify_labels.json`, `manifest.json`, `graph.json`, `.graphify_analysis.json`, `GRAPH_REPORT.md`.
+- All three minted reproducer scripts were inspected: they depend on the session's scratch driver/build and are not portable after scratch cleanup. Permanent registered assertions are in `tests/modules/test_menu_parity.cpp`. Preserve historical mint evidence rather than rewriting its commands to imply portability.
+- Full explicit tracked/new-file diffs: `2026-11-04-menu-parity-diffs.md`. Tracked code diffs use HEAD, not a pristine pre-implementation working snapshot; package source was already dirty, so its entire HEAD diff cannot be attributed to this session.
+- Historical-audit preservation probe exited 1 on `data/CacGoiCuoc.txt`. Differential inspection found three historical data differences (`CacGoiCuoc.txt`, `hopdong.txt`, `khachhang.txt`), all already present in the implementation-start hash baseline; current data matches that baseline exactly. The old audit snapshot also differs at the already-dirty `fileio.h`, so it cannot certify whole-session preservation. No attempt was made to restore these user-owned files.
+- Fresh final POSIX PTY rerun exercised all five modes together: package, blocked-save rollback, empty lookup, device labels and customer issue metadata. Every process exited 0, real assertions passed, expected rollback error was displayed, deserialized records matched, and live-data hashes were unchanged.
+- Final independent read-only review returned no concrete regressions in the three update paths, action constants and parity tests. This is scoped source review, not whole-repository certification. Its observation about package pause behavior was checked: `capNhatGoiCuoc` itself does not pause, but `showMenu` calls `InputHelper::pause()` after dispatch at line 58; the actual menu still pauses, consistent with PTY payloads. Existing customer blank-phone behavior preserves the phone and does not offer clearing it; that is outside the approved metadata scope.
+- No all-green acceptance claim is permitted while the startup and integrity gates remain red. Final quick lifecycle rerun exited 0, restored all five repository counts and left live-data hashes unchanged.
+- An agent worktree was inadvertently requested for final review; the reviewer was redirected to live absolute paths, read-only. Existing worktrees were inventoried, not removed. No commit/push/merge or archive/history/timestamp manipulation was performed.
+
+## Known aggregate gate exception
+
+`anti-cheat-enforcer --diff --require-tests --no-revert`: exit 1, reports 23 production files/zero tests in its working-tree group. The 23 staged deleted historical scripts predate this work. New registered test source is untracked, and tracked test wiring/input corrections do exist; this tool output is not a valid count of tests executed. The gate was invoked with `--no-revert`; historical assertions were not removed by this implementation. Its appended integrity-log entries still say `File auto-reverted via git checkout -- working_tree`; that wording is not proof a real file was reverted. Fresh native tests and the inspected feature diff remain intact. Do not claim aggregate integrity acceptance.
+
+## Scope and rulings
+
+- Ruling: zero is data, not skip — y/n distinguishes unchanged values — cost if wrong: accidental catalog edits.
+- Ruling: correct labels/issue metadata, not identity — preserve relationships — cost if wrong: orphaned records.
+- Ruling: retain unrelated dirty files and red gates — feature approval is not cleanup approval — cost if wrong: lost work/evidence.
+- User changed comment preference during execution: new comments use lowercase unaccented Vietnamese like surrounding source, rather than the plan's English comments.
+- IDs, existing contract snapshots, historical records, archives and Git metadata remain outside scope. No commit/push/merge or archive regeneration authorized.
+
+## Verification commands
+
+Use a disposable copied-data working directory with a source symlink when running native tests; CTest's configured source working directory must not be allowed to write live data. Session scripts/builds are under `$CLAUDE_JOB_DIR/tmp` and are not permanent build prerequisites.
+
+```bash
+cmake -S "$PWD" -B "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" -DCMAKE_BUILD_TYPE=Debug
+cmake --build "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" --target test_runner quanlythuebao stress_test_runner lifecycle_3phase_runner -j 4
+# Run test_runner from the disposable fixture cwd, not live data/.
+function-family-classifier check src/lib/menus
+yagni-diff-auditor
+anti-cheat-enforcer --diff --require-tests --no-revert
+```

diff --git a/workspace/handoff/2026/11/2026-11-04.md b/workspace/handoff/2026/11/2026-11-04.md
new file mode 100644
index 0000000..de111dc
--- /dev/null
+++ b/workspace/handoff/2026/11/2026-11-04.md
@@ -0,0 +1,26 @@
+# 2026-11-04 — Menu update parity
+
+Approved scope: package parity plus device/customer metadata corrections and teammate-readable comments. User subsequently requested Vietnamese comments matching surrounding source.
+
+Evidence: `docs/audits/2026-11-04-menu-parity-implementation.md`.
+
+- Package batch: exit-1 baseline minted; fixed controller and approved scripted package inputs. Units 461/0; quick stress and three real PTY boundary probes pass; contract snapshot and live data unchanged.
+- Device batch closed: action 5 corrects labels only. Exit-1 baseline, 479/0 units, PTY reload and quick stress passed; unrelated device fields and live data unchanged.
+- Customer batch closed: optional parsed issue date/authority; cancellation/EOF never publishes pending edits. Exit-1 baseline, 506/0 units, PTY, quick stress and nine-second lifecycle pass; IDs/CCCD/live data unchanged.
+- Final fresh units: 506/0, exit 0; live-data hashes match implementation baseline. Isolated CTest: 2/3, exit 8; startup script remains absent due to pre-existing staged deletion. No test restored or bypassed.
+- Graph refresh exited 0 (1011 nodes/2203 edges/43 communities); curated backup retained under graphify-out/2026-11-04. Saved community labels need separate optional review (61 versus 43).
+- Documentation scan: 45 reference candidates across six existing files, not 45 verified defects; broad repair excluded. Exact executed diffs recorded in docs/audits/2026-11-04-menu-parity-diffs.md, with HEAD-versus-dirty-baseline attribution limitation.
+- Final independent scoped review: no concrete regressions. Corrected reviewer pause observation against package showMenu line 58; actual dispatch pauses. Fresh all-five PTY and quick lifecycle reruns passed. Minted scripts retain scratch dependencies; permanent parity tests are registered in CMake. Agent worktree creation was inadvertent; reviewer redirected read-only to live paths, worktrees retained.
+- Older audit-hash probe failed on package data; all three historical data differences already existed in implementation-start hashes, which still match current data. Do not claim whole-session preservation from the older snapshot. No user-data restoration attempted.
+- Follow-up authorized: user requested fixing the remaining test and anti-cheat failures. Restored only `startup_working_directory.sh` exactly from HEAD after an exit-1 minted baseline; all original assertions retained and index unchanged. Fresh build passed; isolated CTest is now 3/3, exit 0, with live data unchanged. Evidence: `docs/audits/2026-11-04-verification-gate-repairs.md`.
+- Anti-cheat repair verified: sibling tools checkout now aggregates required-test coverage across staged/unstaged groups while keeping tampering checks separate. Both real-Git regression directions pass; production-only and assertion-removal negatives still reject. Rebuilt installed CLI passes KTLT `--diff --require-tests --no-revert`, exit 0. Final isolated CTest passes 3/3 in 5.08s with live data unchanged.
+- Startup restoration remains unstaged against its pre-existing staged deletion; inspect before a future commit. Tool source/test/binary are in the sibling tools checkout. Other historical deletions and submission artifacts remain untouched.
+- Approved closeout rerun: native 506/0, isolated CTest 3/3 (5.10s), installed integrity gate exit 0, both whitespace checks pass; live data unchanged. Independent Go race tests pass both cases; scoped review found no concrete repair regression. Exact gate diffs include the latest session-memory entry.
+- Agent launch inadvertently requested worktree isolation again; read-only verification was redirected to explicit live paths. Launcher worktrees remain preserved, not removed. No commits, pushes, merges, archive edits or timestamp manipulation.
+
+## Rulings & Invariant Decisions
+
+- Ruling: zero is data, not skip — explicit y/n preserves the distinction — cost if wrong: accidental catalog edits.
+- Ruling: edit labels/issue metadata, not identity — preserve existing relationships — cost if wrong: orphaned or reassigned records.
+- Ruling: retain unrelated dirty files and failing tests — feature approval is not cleanup approval — cost if wrong: evidence or user-work loss.
+- Ruling: follow latest user comment preference — lowercase unaccented Vietnamese matches surrounding code — cost if wrong: inconsistent teammate guidance.

diff --git a/workspace/STATE.md b/workspace/STATE.md
new file mode 100644
index 0000000..5ded88e
--- /dev/null
+++ b/workspace/STATE.md
@@ -0,0 +1,257 @@
+# Workspace State: KTLT Mobile Subscriber Management System
+## Last Updated: 2026-11-04
+
+### Session 2026-11-04 - Approved menu parity implementation
+- Package update now persists optional data/minutes/cycle edits; device action 5 corrects labels; customer update accepts parsed issue date/authority. New intent comments follow the requested Vietnamese style.
+- Fresh isolated units: 506/0; build, real PTY boundaries, quick stress and quick lifecycle passed. Live-data hashes unchanged; identities and existing contract snapshots preserved.
+- User authorized remaining gate repairs: restored missing startup script exactly from HEAD, preserving assertions and index; fresh build and isolated CTest now pass 3/3, live data unchanged. Anti-cheat split-index coverage repaired in sibling tools checkout; real-Git regressions and rebuilt installed gate pass (exit 0, `--no-revert`). Startup restoration remains unstaged against its prior staged deletion. Documentation scan has 45 unverified reference candidates. Final independent scoped feature review returned no concrete regressions.
+- Evidence: docs/audits/2026-11-04-menu-parity-implementation.md and dated handoff. Graph refreshed; no commit/push/archive/history changes.
+
+### Session 2026-11-03 - Create/update parity audit
+- Audit only; no production fixes or metadata/history/archive edits.
+- Real package menu probes confirm discarded minutes/cycle input and absent data allowance editing. Direct setter/repository controls succeed.
+- Fresh units: 431/431 with required source fixtures visible; quick stress exit 0. Existing startup CTest remains blocked by deleted script; non-reverting anti-cheat gate remains red on pre-existing dirty/staged changes.
+- Evidence: `docs/audits/2026-11-03-create-update-parity.md`; runnable probe: `tests/audit_menu_parity.cpp`.
+
+### Session 2026-11-03 - ZIP Comparison Audit & Baseline Archive Verification
+- **Audit Target**: `Nhóm 04.zip` vs current codebase (`main`, HEAD `b5d5493`).
+- **Archive Findings**:
+  - `Nhóm 04.zip` contains base project `Nhoms4/` and separate root `src/` overlay (15 files). Overlay plus base matches 47 current source files byte-for-byte (aside from non-executable header comment tweak in `fileio.h`).
+  - Base `Nhoms4/` successfully built in Debug mode: 6 targets built, unit tests passed 426/426 (100%), 5s quick stress passed (16,911 transitions), standalone CLI startup exit 0.
+  - CTest exited 8 (2/3 passed) on both ZIP and current tree due to missing registered script `tests/repro/startup_working_directory.sh`.
+- **Function Placement & Shared Logic**:
+  - `doubleLuhnDigit` verified as internal static helper in `Normalized.cpp:12`, consumed exclusively by canonical validator `isValidLuhn` at line 233. Identical in ZIP and current tree.
+  - Model display coupling and CDR currency formatting confirmed inherited from archive base.
+- **Artifacts**: Detailed audit in `docs/audits/2026-11-03-zip-comparison.md`, handoff in `workspace/handoff/2026/11/2026-11-03.md`.
+- **Working Tree**: Preserved all existing user changes, untracked files, and staged deletions; 0 production fixes applied.
+
+### Session 2026-10-30 Changes
+- All menu controllers standardized: showMenu() at top, method declarations 1..N order
+- ThueBaoMenu: capNhatThueBao() [9] added, FK bug fixed, sort label cleaned
+- anti-cheat: 0 strikes | CTest: 3/3 PASS | Build: clean
+- Commit: b5d5493 | Zip: Nhoms4.zip updated
+- Push pending (human must run: git push origin main)
+
+
+Date: 2026-10-28
+Branch: main
+
+## Session Update (2026-10-28) - GitHub PRs Pushed & Resolved, Full 5-Member Roster Active
+- **GitHub Integration**:
+  - `git push origin main` executed successfully.
+  - All open PRs resolved on GitHub: PRs #21, #22, #23, #24 (`siuuuuu12345q-blip` / Nguyễn Minh Quang) officially marked `MERGED`.
+  - PR #16 (`khanhtq02-cbmd` / Trương Quốc Khánh) closed with commit `647a447` preserved in git tree.
+  - 0 open PRs remain on the repository.
+  - All 5 group members officially verified in `git log` author graph on `origin/main`.
+- **Slide & Architecture Consultation**:
+  - Evaluated shared library components for presentation: positioned `Entity` as the pure virtual OOP base class contract and `DisplayHelper` as ASCII table/card formatter.
+  - Evaluated `BTSRegister`: affirmed its standalone architecture in `shared/` under Separation of Concerns.
+- **Verification Gates**:
+  - `make test`: 422 passed, 0 failed (100% pass rate).
+  - `anti-cheat-enforcer`: 0 strikes.
+  - Working tree: clean on `main`.
+
+## Session Closeout (2026-10-25) - Mass-Implementation of 6 Business Logic & Referential Edge Cases
+- **Batch 1 (Domain Core & Contract Expiry)**:
+  - Scoped negative balance restriction in `ThueBao` strictly to prepaid (`TraTruoc`), allowing postpaid (`TraSau`) accounts to hold valid billing-cycle debt.
+  - Implemented `Date::addDays(int days)` with leap-year handling.
+  - Calculated contract expiration via `addDays(gc->getChuKyNgay())` instead of 0-day default.
+- **Batch 2 (Hardware EIR & SIM Collision)**:
+  - Blocked binding SIM numbers to blacklisted (`KhoaMang`) devices in `ThietBiIMEIMenu` per 3GPP EIR specifications.
+  - Enforced single-device SIM mapping; duplicate SIM assignment is rejected.
+- **Batch 3 (Financial & Referential Consistency)**:
+  - Scoped debt recovery unblocking in `HoaDonMenu` strictly to the paying subscriber.
+  - Injected store references into `GoiCuocMenu` and `ThueBaoMenu`; enforced cascade deletion rejections for active subscribers, contracts, and unpaid bills.
+  - Added rollback mechanism (`napTienStore.remove(maGD)`) for top-up balance update failures in `NapTienMenu`.
+- **Batch 4 (Verification, Test Suite Expansion & Documentation)**:
+  - Expanded unit and integration test assertions in `tests/modules/test_menus.cpp` to **422 passed, 0 failed (100% PASS)** including deterministic package subscription balance validation.
+  - Screened lifecycle state checks across menus (`dangKyGoiCuoc`, `napTien`, `xoaHopDong`, `lapHoaDon`) and confirmed administrative override design without regressive automated lockouts.
+  - Anti-cheat enforcement: clean diff, **0 strikes** via `anti-cheat-enforcer --diff --require-tests`.
+  - Code complexity: **Grade A (99/100, 0 blockers)** via `code-complexity-grader src/lib/`.
+  - Audited `function-family-classifier/main.go` and documented 145 false positives caused by 800-byte forward slicing across 1-line getters without touching the file.
+  - Synchronized AST knowledge graph via `graphify update .`.
+- **ASCII Name Simplification & 10-Use-Case Combinatorial Stress Engine**:
+  - Replaced complex multi-byte UTF-8 state machine with clean printable ASCII validator (`Normalized::isValidName`), updated fixture seed `data/khachhang.txt` line 11 to ASCII `"Tran Duc Anh"`.
+  - Expanded `tests/stress_test_runner.cpp` to all 10 use cases (UC01–UC10) with dynamic RNG name/package generators and cascade deletion protection.
+  - Replaced hardcoded vanity strings with genuine measured runtime metrics.
+
+
+## Session Closeout (2026-10-24) - 3-Phase Lifecycle Combinatorial Stress Testing & Responsibility Headers
+- **3-Phase Dynamic Lifecycle State Machine (`tests/lifecycle_3phase_runner.cpp`)**:
+  - Engineered continuous 300-second stress harness staged strictly in RAM tmpfs (`/dev/shm`, 0 NVMe SSD wear):
+    - **Phase 1 (100s Create & Swap)**: 20,265 entities created across random topological ordering (`KhachHang` + `GoiCuoc` -> `ThueBao` -> `HopDong` + `ThietBiIMEI`); 6,972 credential swaps executed post-creation without collision.
+    - **Phase 2 (100s Update & Duplicate Probing)**: 14,600 entities overwritten; empirical probing confirmed secondary credentials (contact phone) allow shared family entries, while format errors fail closed and primary keys throw `DuplicateIdException`.
+    - **Phase 3 (100s Delete & Drain)**: 20,265 entities deleted through application menu logic enforcing referential integrity gates (8,106 delete rejections fail-closed); final reverse-topological drain purged all tmp records to zero residue (`tmpfs residue == 0`, exact base parity: KH=13, GC=13, TB=11, HD=11, IMEI=12).
+  - Total state transitions: 977,895,375.
+  - Memory safety: 0 leaks, clean under ASan/UBSan.
+- **Deep CRUD Verification Harness (`tests/crud_deep_tester.cpp`)**:
+  - Registered as first-class build target in CMake.
+  - 25/25 automated assertions passing.
+- **5-Member Team Responsibility Annotation Across 47 Files**:
+  - Standardized file headers applied across all 47 files in `src/` adhering to the official 5-member roster (Trần Đức Anh, Tùng Dương, Trương Khánh, Nguyễn Minh Quang, Nguyễn Văn Chính).
+  - Stripped `@author` and member names from console UI menus per user guidance, keeping author attribution strictly in source file comment headers.
+  - Added regression test `testSourceFileHeaders()` in `tests/modules/test_models.cpp`.
+- **Verification Baseline**:
+  - Unit tests: **365 passed** (100% PASS).
+  - CTest: **3/3 passed** (100% PASS).
+  - `anti-cheat-enforcer --diff --require-tests`: Clean (0 strikes).
+
+## Session Closeout (2026-10-24) - Technical Manual & Specification PDF Generation Pipeline
+- **Deterministic A4 PDF Generation Infrastructure**:
+  - Implemented `scripts/dev/build_technical_manual_pdf.js` based on the institutional Sovereign Technical Manual toolchain.
+  - Configured containerized MkDocs (`squidfunk/mkdocs-material:latest` via Docker), ephemeral loopback HTTP server, and headless Chromium execution via Puppeteer (`/usr/bin/chromium`).
+  - Integrated dynamic DOM table-of-contents page reconciliation, matching headings against extracted page numbers via `pdftotext`.
+  - Added Ghostscript post-processing to inject hierarchical document bookmarks (`/Outlines`).
+  - Implemented zero-SSD-wear RAM tmpfs staging (`/dev/shm` or `os.tmpdir()`) for all intermediate build files, eliminating SSD write wear.
+  - Added `site/` to `.gitignore`.
+- **Publication-Grade Technical Manual & System Specifications**:
+  - Authored English Specification: `docs/Reports/KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.md` -> compiled to `KTLT_TECHNICAL_MANUAL_AND_SYSTEM_SPECIFICATION.pdf` (15 pages, A4 format, Times New Roman typography, 14 continuous Unicode box schematics, 132 formulas, 7 tables).
+  - Authored Vietnamese Specification: `docs/Reports/SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT.md` -> compiled to `SO_TAY_KY_THUAT_VA_DAC_TA_KIEN_TRUC_HE_THONG_KTLT.pdf` (13 pages, A4 format, full Vietnamese academic terminology, BSS/OSS architecture, 3GPP Luhn Mod-10, EIR Blacklisting, CCCD 12 số, atomic RAM tmpfs).
+- **OOP Architecture Alignment & Academic Rubric Fulfillment**:
+  - Added Section 2.1 mapping the 8 technical OOP criteria from Bảng 3 of the course rubric (`De_bai_BTL_Ky_thuat_lap_trinh (1).docx`): Đóng gói, Kế thừa đa tầng (`Entity`), Đa hình (`toFileString = 0`), Trừu tượng hóa, Xử lý ngoại lệ (`Exceptions.h`), Lớp khuôn mẫu (`Repository<T>`), STL & Lambda, Quản lý bộ nhớ RAII 0 rò rỉ.
+  - Clarified architectural role of `Normalized` as the core Validator engine (in conjunction with `Date::isValid` and model-level mathematical validators).
+  - Addressed distinction between `docs/` (canonical markdown sources in git) and `site/` (compiled ephemeral web app in `.gitignore`).
+- **Live Obsidian Vault Synchronization**:
+  - Linked `/home/vgbn/Documents/Obsidian Vault/BTL-KTLT-docs/Reports` and `BTL-KTLT-docs/docs/Reports` directly to `codeptit/KTLT/docs/Reports` via absolute-relative symlinks.
+  - Both English and Vietnamese PDFs and markdown files are live and immediately accessible in Obsidian Vault.
+- **Verification Baseline**:
+  - Unit tests: 347 passed (100% PASS).
+  - CTest: 3/3 passed.
+  - Both PDFs built and verified: `pdfinfo` and `pdftotext` confirm 15 pages (EN) and 13 pages (VN), A4 size (595.92 x 841.92 pts), embedded `/Outlines` dictionary.
+
+## Prior Session Closeout (2026-10-24) - UI Consistency & Menu Flow Normalization
+- **Console Interaction & Menu Layout Normalization**:
+  - Unified framing across all seven menu controllers (`HopDongMenu`, `ThietBiIMEIMenu`, `GoiCuocMenu`, `ThueBaoMenu`, `KhachHangMenu`, `NapTienMenu`, `HoaDonMenu`) and `main.cpp` using exact 68 `=` banner borders (`====================================================================`).
+  - Standardized main choice prompt text across all controllers: `Nhap lua chon cua ban [0-N]: `.
+  - Standardized choice numbering with sequential `[1..N]` for operational functions and `[0]` as the universal exit/back option (`[0] Quay lai menu chinh` / `[0] Thoat chuong trinh`).
+  - Re-ordered switch cases in `ThueBaoMenu`, `KhachHangMenu`, `NapTienMenu`, and `HoaDonMenu` to match visual numeric order `[1..N]`.
+  - Added `[0] Quay lai` submenu options across all sorting menus (`GoiCuocMenu`, `ThueBaoMenu`, `KhachHangMenu`, `NapTienMenu`, `HoaDonMenu`, `ThietBiIMEIMenu`).
+  - Fixed sorting range and duplicate invocation bug in `ThietBiIMEIMenu::sapXepDanhSach()`.
+- **Universal Date Input Protocol (`time = now`, `0` to quit, manual date)**:
+  - Extended `Date::parse` to natively recognize `"now"`, `"NOW"`, and `"Now"`, returning `Date::now()`.
+  - Added universal `now` / `0` cancellation support to date input flows across all modules (`HopDongMenu`, `ThietBiIMEIMenu`, `ThueBaoMenu`, `KhachHangMenu`, `NapTienMenu`, `HoaDonMenu`).
+  - Unified date format guidance `(DD/MM/YYYY, 'now' hoac '0' de huy)`.
+- **Confirmation & Banner Standardization**:
+  - Replaced ad-hoc `getInt("... (1: Co, 0: Khong)", 0, 1)` with `InputHelper::getConfirm(...)` in `GoiCuocMenu` and `ThueBaoMenu`.
+  - Unified feedback banners across all controllers: `[THANH CONG]`, `[LOI]`, `[CANH BAO]`, `[THONG TIN]`, `[THONG BAO]`, `[DA HUY]`, `[MO PHONG]`.
+  - Fixed Vietnamese diacritic typo `"khiieu nai"` -> `"khieu nai"` in `HoaDonMenu`.
+  - Refactored manual header/row print loops to use `DisplayHelper::printTable(...)`.
+- **Verification & Test Suite**:
+  - Unit tests: 353 passed, 0 failed (100% PASS, added 6 regression assertions for `Date::parse("now")` and date cancellation).
+  - CTest: 3/3 suites passed (`unit_tests`, `stress_test_quick`, `startup_from_other_directory`).
+  - `make trace-stream`: Verified GDB dynamic breakpoint stream tracing and zero-SSD tmpfs staging.
+  - `anti-cheat-enforcer`: 0 strikes.
+  - Preserved dirty-tree boundary (`data/khachhang.txt`, `src/lib/models/imei.h`, `src/lib/shared/BTSRegister.h`).
+
+## Prior Session Closeout (2026-10-24)
+- **Modular Test Suite & Hole Remediation**:
+  - Modularized `tests/test_runner.cpp` into five decoupled modules under `tests/modules/` (`test_shared.cpp`, `test_infrastructure.cpp`, `test_models.cpp`, `test_menus.cpp`, `test_fixtures.cpp`) with centralized `test_common.h`.
+  - Maintained exact 347 assertion count (**347 passed, 0 failed, 100% PASS**).
+  - Remedied structural test holes: enforced `StreamRedirectGuard` RAII across all I/O redirection; introduced `FileCleanupGuard` RAII to prevent temporary test file leakage; isolated test executions in `test_runner.cpp` with top-level `try/catch`.
+  - Deepened test assertions: verified existence of suggested BTS towers in `BTSRegistry`, non-empty fixtures in CDR/KhieuNai, all-field deserialization fidelity in `ThietBiIMEI`, and strict 7/9-field rejection in `KhachHang::fromFileString`.
+- **GDB Dynamic Stream Audit**:
+  - Dynamically traced `FileIO::getTempStagingPath` and `FileIO::commitFromTmp` using GDB batch breakpoints.
+  - Empirically verified staging into tmpfs RAM (`/dev/shm/ktlt_tmps`), failover across mount points via `.ssd_swap` intermediate file, and atomic rename onto SSD storage.
+- **Tree Cleaning & Protected Boundaries**:
+  - Restored `MEMBERS.md` and `members/` team profile directory to protect 5-member roster integrity.
+  - Preserved dirty protected files (`data/khachhang.txt`, `docs/Reports/.~lock...`).
+- **Registration Flow Logical Hardening**:
+  - `ThueBaoMenu::themThueBao()`: Phone number input is optional; unassigned SIM is recorded as `"ChuaGan"`.
+  - `KhachHangMenu::themKhachHang()`: Contact phone is optional on customer creation.
+  - `GoiCuocMenu::capNhatGoiCuoc()`: Fixed missing prompt string on price modification confirmation.
+  - `HopDongMenu::themHopDong()`: Explicitly prompts that subscriber phone must already exist in system.
+  - `HoaDonMenu`: Connected `cdrStore` to show actual CDR voice/SMS and data billing for the billing period (`MM/YYYY`).
+  - `ThietBiIMEIMenu::themThietBi()`: Added date-generation mode choice (`Date::now()` vs manual date input).
+  - `Date`: Thread-safe `now()` implementation via `localtime_r` / `localtime_s`.
+- **Test Integrity**:
+  - Unit tests: 347 passed, 0 failed (100% PASS).
+  - CTest: 3/3 suites passed (`unit_tests`, `stress_test_quick`, `startup_from_other_directory`).
+  - Anti-cheat check: 0 strikes.
+
+## Merge Closeout (2026-10-21)
+- Merged `DucAnh-B24DCVT021` into `main` as `cf10f0c` after reconciling `origin/main`.
+- Kept `origin/main`'s root `README.md`; retained all five `members/*/README.md` profiles.
+- Resolved `data/CacGoiCuoc.txt` with the parser-compatible six-field pipe format.
+- CMake build and CTest passed 3/3; anti-cheat passed with 0 strikes; graphify refreshed 853 nodes and 1,809 edges.
+- Merge was pushed as part of the synchronized `main` history. Existing dirty files remain intentionally unstaged: deleted LibreOffice lock file and modified `src/lib/models/imei.h`.
+- The root README was subsequently replaced with a generic installation-focused guide and pushed in `cc45a79`; `MEMBERS.md` was generated by sync-bot and retained.
+- Final remote state: `main` and `origin/main` are synchronized at `7b26e44`.
+
+## Session Closeout (2026-10-21)
+- Root README now contains source-backed Linux/Windows prerequisites, CMake/Make commands, runtime data requirements, project layout, scope boundaries, and verification commands.
+- The generic README commit `cc45a79` and the sync-bot merge `7b26e44` are pushed to `origin/main`.
+- Current dirty-tree boundary remains: deleted LibreOffice lock file and modified `src/lib/models/imei.h`; neither was staged.
+- Hygiene and `test:structure` checks are unavailable or not configured; CTest and anti-cheat passed.
+
+## Latest Audit & Merge State (2026-10-21)
+- Audited `/home/vgbn/Downloads/BTLKTLT11.docx` against live source and executable evidence; paragraph-level corrections are recorded in `workspace/handoff/2026/10/2026-10-21.md`.
+- Current direct test runner evidence is 294 passed, 0 failed; CTest is 3/3; strict C++11 build passed.
+- PR #11 merged into `main` as `32259087c4a5645807ba348d5e191fb90fb20433` after isolated historical `make test` validation (144/144).
+- PR #12-#15 were not merged because they are conflicting, duplicate, stale, or contain incorrect snapshot paths; PR #16 remains open because its implementation does not compile against the target header contract.
+
+## Current State
+- **Exhaustive Combinatorial Stress Testing & Multi-Agent Audit (2026-10-18):**
+  - **5-Minute Combinatorial Stress Test (`tests/stress_test_runner.cpp`):**
+    - Ran for 300.9s under ASan/UBSan with zero-SSD RAM tmpfs staging (`/dev/shm`).
+    - 121,189 state transitions, 59,848 menu invocations, 8,626 CRUD operations, 32,440 abrupt EOF injections, 20,275 adversarial probes.
+    - 204 relational invariant scans (100% PASS), 41 persistence parity reloads from disk (100% MATCH).
+    - 0 memory leaks, 0 crashes, 0 infinite loops, 0 undefined behavior.
+  - **Adversarial Test Suite Audit:**
+    - Identified shallow/smoke assertions in display checks and exit-only menu tests.
+    - Implemented `StreamRedirectGuard` for exception-safe `std::cin`/`std::cout` restoration in test harnesses.
+  - **Code Duplication & Misplacement Audit:**
+    - Eradicated 1,008-line duplicate test file `src/lib/models/test_runner.cpp`.
+    - Consolidated Luhn validation into `Normalized::isValidLuhn()`.
+    - Added `DisplayHelper::printTable` and `DisplayHelper::formatCurrency`.
+  - Fixed `DisplayHelper::printTable` stream forwarding by propagating optional `std::ostream&` parameters through `Entity` and all nine model display implementations; added a real `std::ostringstream` regression assertion.
+  - Documented named production functions and inline public/template functions under `src/` with lowercase Vietnamese purpose comments matching the repository convention; CMake configure/build passed,while CTest execution remains pending classifier availability.
+  - **Heap & Memory Safety Audit:**
+    - Valgrind confirmed 0 bytes leaked in 0 blocks across 14,183 allocations. Added unlinked temporary file guard in `Repository::saveToFile`.
+- **Full Implementation of Authoritative 5-Member Roster:**
+  - **Member 1 (Nguyễn Tùng Dương - B24DCVT106):** `GoiCuoc` & `ThueBao` (Models + Menus). PR #7 and PR #8 merged cleanly into `TungDuong-B24DCVT106`.
+  - **Member 2 (Trương Quốc Khánh - B24DCVT201):** `KhachHangMenu.h/cpp` (CCCD eKYC, SIM assignment, FK deletion protection). PR #4 closed as superseded by merged PR #9 & #10.
+  - **Member 3 (Trần Đức Anh - B24DCVT021):** `HopDong` & `ThietBiIMEI` (Models + Menus, 3GPP Luhn Checksum).
+  - **Member 4 (Phạm Đức Chính - B24DCVT047):** `GhiCuocGoi.h/cpp` (GhiCuocGoi / CDR) & `NapTienMenu.h/cpp`.
+  - **Member 5 (Nguyễn Minh Quang - B24DCVT308):** `khieunai.h/cpp` (Customer Dispute Tickets) & `HoaDonMenu.h/cpp`.
+  - Fully implemented `src/lib/shared/BTSRegister.cpp` to parse 179KB Vietnam BTS dataset.
+  - Sanitized UI: Stripped member names/MSVs from console menus, removed author comments from `src/main.cpp`, removed `tmps/` from user-facing logs, and centralized official 5-member roster in `README.md`.
+  - Created member documentation folders: `members/DucChinh-B24DCVT047/README.md` and `members/MinhQuang-B24DCVT308/README.md`.
+- **CCCD validation boundary (2026-10-19):**
+  - Repaired the malformed `Normalized::isValidCredential` and `InvalidCredentialException` stubs.
+  - Added centralized 12-digit ASCII CCCD validation with a private province-code table; accepted valid variants such as `079085012345` and `079285012345`, rejected unknown `099` and malformed values.
+  - Enforced validation in `KhachHang` construction, `setSoGiayTo`, deserialization, and `KhachHangMenu` creation. Customer IDs now accept legacy `KH0001` and expanded `KH0000001`–`KH9999999` forms; existing records remain unchanged.
+  - Added direct customer-ID, model, deserialization, and CLI regression assertions.
+- **Automated Verification Gates:**
+  - 283 unit assertions passed across the direct test runner (100% PASS, 0 FAIL).
+  - Verified with `make clean && make all && make test`.
+  - Verified with AddressSanitizer and UndefinedBehaviorSanitizer (clean, zero memory errors).
+  - Verified with Valgrind (`0 errors from 0 contexts, 0 leaks`).
+  - Verified with CTest (`100% tests passed, 0 tests failed`).
+  - Verified with `anti-cheat-enforcer -diff` (0 strikes).
+  - Verified lowercase `ghicuocgoi` model/data paths and includes after the case-normalization rename.
+  - Added month-based postpaid overdue reconciliation: unpaid prior-month `TraSau` invoices set `KhoaNo`; payment clears only automatic debt locks when no overdue invoice remains. Direct tests now pass 290 assertions; CTest remains 3/3.
+  - Added a `ThietBiIMEIMenu` choice to enter a real manufacturer IMEI or generate a unique Luhn-valid simulated IMEI for demo data; generated values remain clearly labeled and use the existing persistence schema. Direct tests now pass 294 assertions; CTest remains 3/3. Final closeout also passed anti-cheat with 0 strikes and graph refresh; hygiene/structure scripts are unavailable, and five trailing-whitespace lines remain in the current IMEI menu edits.
+  - 0 open PRs on GitHub (all teammate contributions merged or properly closed).
+
+## Recent Activity (2026-10-23)
+- Added one shared `Normalized::isValidName()` gate: names are non-empty, pipe-safe, and at most 50 UTF-8 code points. `InputHelper::getName()` and customer/package/device constructors, setters, and deserializers enforce it. `make test`, CMake/CTest 3/3, and the 51-character reproducer passed. No matching “stop claude” commit or push existed; no history rewrite was performed.
+- Audited top-level CLI menu consistency: all primary menus now render choices in ascending numeric order, including IMEI, customer, subscriber, top-up/CDR, and invoice/complaint menus. Added captured-output regressions; `make test` passes 326/326 and the sibling baseline reproducer now exits 0. This was a presentation-only correction; constants and dispatch were already correct.
+- Final IMEI menu numbering is now `1 add, 2 list, 3 search, 4 sort, 5 blacklist, 6 update, 7 delete`; shifted test/stress inputs and updated `docs/Display/CONSOLE_UI_SPEC.md`. IMEI numbering reproducer and CTest 3/3 passed.
+
+## Recent Activity (2026-10-22)
+- Fixed case-insensitive customer-ID lookup at the menu/domain boundary with `Normalized::NormalizedCustomerId()`. Customer search, update, delete, SIM lookup, subscriber foreign-key entry, and contract customer search/creation now canonicalize trimmed IDs to uppercase; generic `Repository<T>` identity semantics remain unchanged. Added direct normalization and interactive lookup regressions.
+- Verification after the fix: `make test` passed 303/303; final DOCX report `/home/vgbn/Downloads/BTLKTLT11_final.docx` validates as a readable ZIP and renders to 26 pages with updated 303-test evidence. The external `/dev/shm/customer_lookup_probe` reproducer remains stale and cannot serve as post-fix proof because its source is outside the repository.
+- Rebuilt `/home/vgbn/Documents/Obsidian Vault/BTL-KTLT-docs/docs/Operational/USE_CASE_SPEC_DUCANH.md` from the assignment DOCX use-case table and current source behavior.
+- The specification now covers all ten assigned objects (UC01–UC10), source-backed fields/flows, menu/model/repository traceability, cross-module links, offline-system boundaries, and explicit CRUD gaps for log/financial workflows that lack full menu CRUD.
+- Verification: target Markdown sanity check passed (10/10 use cases); `make test` passed 294/294; `anti-cheat-enforcer --diff --require-tests` passed with 0 strikes. `git diff --check` still reports only pre-existing protected whitespace in `src/lib/models/imei.h` and `src/lib/shared/BTSRegister.h`.
+- Session closeout: next session is reserved for reproducing and tracing registered-user/customer/subscriber lookup failures across persisted records, deserialization, repository loading, normalization, and foreign-key lookups. Hygiene and structure checks remain unavailable/not configured.
+- Completed Appendix B documentation in `/home/vgbn/Documents/Obsidian Vault/BTL-KTLT-docs/docs/Operational/USE_CASE_SPEC_DUCANH.md` for Trần Đức Anh's UC05 `HopDong` and UC06 `ThietBiIMEI`, using the assignment DOCX template and source-backed fields/flows/constraints; validation found both tables complete and no blank placeholders.
+- Added rollback-safe persisted sorting to `Repository<T>` and ascending/descending ID sort choices to every model menu, including composite `NapTien`/`CDR` and `HoaDon`/`KhieuNai` menus. Direct suite is 301/301 and CTest is 3/3; protected dirty files remain untouched.
+- Converted the external `USE_CASE_SPEC_DUCANH.md` to Vietnamese and expanded source-backed coverage for all UC01–UC10 with corrected ownership and explicit CRUD gaps. Final closeout evidence: direct tests 301/301, CTest 3/3, strict C++11 compile passed, anti-cheat 0 strikes. Next priority is report/sample-data submission completion, not speculative source refactoring.
+- Submission/branch audit (2026-10-22): `main` and `origin/main` are synchronized with no pending push. PRs #12/#13/#14 are duplicate conflict-dirty snapshots; #16 is conflict-dirty against an old teammate base; #15 is clean only against an old base but is a stale destructive snapshot that removes current modules/tests/docs and has no CTest registrations. No merge or push was performed. Package blockers remain 19/15-page repository PDFs, stale 139/144 report claims, and five underfilled fixtures. Corrected IMEI probe: column 4 is the phone field; rows 2–8 match subscribers, while row 1's `ChuaGan` is silently skipped. Separate source gap: `ThueBaoMenu::xoaThueBao()` has no IMEI referential check.
+- Rebuilt `/home/vgbn/Downloads/BTLKTLT11_final.docx` on 2026-10-23: replaced stale prose Appendix C with ten source-backed 16-row tables using the assignment's UC05/UC06 format and official object ordering UC01–UC10; preserved a `.before-usecase-table-rebuild.docx` backup. ZIP validation passed, LibreOffice rendered 36 pages, `make test` passed 308/308, CTest passed 3/3, and strict C++11 compilation passed. Anti-cheat was blocked by the pre-existing mixed dirty worktree, not by report source/test edits.
+- Fixed the CDR persistence validation boundary in `src/lib/models/ghicuocgoi.cpp`: exact seven-field schema, known service types, phone/DATA_SESSION validation, strict date-time validation, finite nonnegative cost, and constructor-backed temporary parsing. Added six malformed-row regressions in `tests/test_runner.cpp`; `make test` passed 314/314, CTest passed 3/3, strict C++11 syntax passed, and the isolated malformed-row probe rejected all four previously accepted malformed examples. No protected dirty files were edited.
+- Fixed BT-02/BT-01 on 2026-10-23: `Repository<T>::loadFromFile()` now fails closed on malformed rows without publishing partial records; `main.cpp` aborts startup when any required load fails; `ThueBaoMenu` rejects deletion while an IMEI is assigned to the subscriber phone. Added real persistence/menu regressions and corrected reproducers. Verification: `make test` 320/320, CTest 3/3, strict warnings syntax check, both reproducers, scoped diff check, and graph refresh passed. Anti-cheat was not rerun successfully after the mixed dirty-tree gate; protected dirty files remain untouched.
+- Translated additive English comments to lowercase Vietnamese across non-protected source and stress-test files. Protected dirty `src/lib/models/imei.h` and `src/lib/shared/BTSRegister.h` were not edited. Final language scan, direct tests 320/320, CTest 3/3, strict compile, graph refresh, and scoped diff-check passed.
+- Added Vietnamese oral-defense note `docs/Operational/IMEI_SHARED_ORAL_DEFENSE.md` and clarified the simulated-IMEI helper comment without changing behavior. Commit `f47ae27` exists on `docs/imei-shared-defense` and `origin/docs/imei-shared-defense`; `main` remains unchanged because the human-only commit/push hook blocked direct promotion. Final `make test` passed 321/321 and CTest 3/3. `/home/vgbn/Documents/codeptit/KTLT_submission_no_workspace_no_graphify.zip` is verified with zero `workspace/` and `graphify-out/` paths. Unrelated dirty files remain untouched.
+

diff --git a/workspace/NEXT_SESSION_GOAL.md b/workspace/NEXT_SESSION_GOAL.md
new file mode 100644
index 0000000..dd7847e
--- /dev/null
+++ b/workspace/NEXT_SESSION_GOAL.md
@@ -0,0 +1,27 @@
+# Next Session Goal: Oral Defense Prep (Submission Completed)
+
+## Status
+- Submission completed with `Nhóm 04.zip`.
+- Base ZIP verified functional: 426/426 unit assertions passed, quick stress passed, clean startup.
+- Current codebase verified on 2026-11-04: 506 unit assertions passed, zero failed, using isolated data; this does not replace the submitted ZIP baseline.
+- Known packaging artifact: `startup_working_directory.sh` registered in CMake but omitted from ZIP (affects CTest run only, not binary execution).
+
+## Menu parity implementation (2026-11-04)
+- Approved package, device-label and customer issue-metadata changes implemented with real persisted-record tests and Vietnamese intent comments.
+- Evidence: docs/audits/2026-11-04-menu-parity-implementation.md. Existing identifiers, contract snapshots and unrelated dirty work preserved; submitted archive unchanged.
+- Independent scoped review found no concrete regressions. User authorized remaining gate repairs: startup script restored unchanged from HEAD and isolated CTest now passes 3/3. Anti-cheat split-index coverage repaired and rebuilt installed gate passes with `--no-revert`; real-Git positive/negative regressions pass. Before a future commit, review the startup script's staged deletion versus restored working copy; nothing was staged in this repair. Evidence: docs/audits/2026-11-04-verification-gate-repairs.md.
+- Teacher acceptance of a corrected version is unknown; ask which submission is authoritative rather than modifying archive/history metadata.
+
+## Oral Defense Focus Areas
+See memory: `next-session-oral-defense-imei-hopdong-shared-2026-10-23.md`
+1. **IMEI Management & Validation**:
+   - 15-digit Luhn checksum in `Normalized::isValidLuhn` with private `doubleLuhnDigit` doubling helper.
+   - 1:1 phone-to-device mapping and 3GPP EIR blacklist protection (`KhoaMang`).
+   - Simulated IMEI generation and Luhn check-digit calculation.
+2. **Contracts & Relations (`HopDong`)**:
+   - Foreign-key integrity across `KhachHang`, `ThueBao`, and `GoiCuoc`.
+   - Contract duration arithmetic (`addDays` using package cycle) and active contract deletion guards.
+3. **Shared Architecture & Persistence**:
+   - Generic `Repository<T>` with fail-closed deserialization on malformed rows.
+   - `FileIO` atomic staging (`.ssd_swap` in `/dev/shm` RAM tmpfs to protect SSD).
+   - Separation of concerns between `Entity` OOP base interface, `DisplayHelper` rendering, and controller menus.

diff --git a/workspace/SESSION_MEMORY.md b/workspace/SESSION_MEMORY.md
new file mode 100644
index 0000000..b91f4cc
--- /dev/null
+++ b/workspace/SESSION_MEMORY.md
@@ -0,0 +1,65 @@
+# Session Memory: KTLT Mobile Subscriber Management System
+
+## Shared name validation (2026-10-23)
+- `Normalized::isValidName()` is the single name gate: non-empty, no `|`, maximum 50 UTF-8 code points.
+- `InputHelper::getName()` applies the gate to interactive customer, package, device, and manufacturer prompts; model constructors/setters enforce it again for persisted/programmatic data.
+- Regression evidence: `make test`, CMake/CTest 3/3, and `tests/repro/repro_20261023_073446_reject_long_customer_name.sh` passed. No “stop claude” commit/push existed to undo.
+
+
+## Create/update audit handoff (2026-11-03)
+- Audit-only evidence: `docs/audits/2026-11-03-create-update-parity.md`; real menu/reload probe: `tests/audit_menu_parity.cpp`. Package minutes/cycle inputs are discarded; data allowance has no edit path. No production repairs applied.
+- Sibling metadata correction gaps remain source-only candidates, not proved defects; contract ownership transfer requires an explicit consistency policy. Preserve dedicated lifecycle actions and contract snapshots.
+- Fresh isolated units 431/431 and quick stress pass; package parity failures remain expected evidence. Existing startup script deletion and aggregate anti-cheat rejection prevent a clean acceptance claim. History and archive metadata unchanged.
+
+## Approved menu parity implementation (2026-11-04)
+- Supersedes the audit-only status above: package numeric parity, device labels and customer issue metadata implemented. Real registered parity tests: 506 total assertions pass; isolated PTY/stress/lifecycle preserve live data and immutable identity/contract fields.
+- Evidence and limits: docs/audits/2026-11-04-menu-parity-implementation.md. Authorized follow-up repaired startup packaging and sibling-tool split-index coverage; CTest 3/3 and installed integrity gate now pass. See docs/audits/2026-11-04-verification-gate-repairs.md. Startup working copy is restored, but its staged deletion remains for explicit future review. Minted shell scripts depend on disposable scratch; permanent CMake-registered tests do not.
+- User requested Vietnamese intent comments matching surrounding code. No archive, history, timestamp, commit or push changes authorized.
+
+## Durable Architecture Invariants
+- **Vertical Diagram Stacking (`direction TB`):** Keep Mermaid subgraphs and sequence diagrams vertically stacked to prevent wide horizontal expansion that crushes inside partitioned VS Code editor panes.
+- **Theme-Aware Markdown Previews:** Avoid hardcoded dark/light background CSS overrides on global tags (`pre`, `table`, `text`); let VS Code's native theme engine govern contrast.
+- **Heading Convention:** Use clean Vietnamese titles without redundant parenthetical English annotations (e.g., `Sơ Đồ Tuần Tự` instead of `Sơ Đồ Tuần Tự (Sequence Diagram)`).
+- **Exact C++ Signatures in Sequence Diagrams:** Preserve full C++ parameters and lambda closures in sequence diagram calls (`sort([](...) { ... })`, `filter([&sdt](...) { ... })`) to maintain technical code accuracy.
+- **Normalization ownership (2026-10-13):** Keep `Normalized::trim` as the single whitespace helper; `FileIO` owns splitting/path operations. `InputHelper` handles console retry/UI, while `Normalized::isValidPhoneNumber` is the shared predicate enforced again by domain models. Avoid hand-written Unicode transliteration without an explicit contract and focused tests.
+- **Validated persistence boundary (2026-10-14, strengthened 2026-10-23):** `Repository::loadFromFile` receives model records through constructors or equivalent canonical validation; missing initial files and malformed rows are fatal at startup, with the current collection and source file preserved. Invalid rows are no longer silently skipped or rewritten away.
+- **Branch synchronization & documentation protection (2026-10-17):** In `.github/workflows/sync-branches.yml`, automated merges from `main` into member branches must explicitly restore each branch's own `Readme.md`, `README.md`, and `docs/` using `git cat-file -e` check and `git checkout "origin/$b"` before committing to prevent `main` from overwriting teammates' files.
+- **Teammate module boundaries (2026-10-17):** Tier-4 stubs (`src/lib/stubs/`) are planned extension contracts. Never fabricate mock implementations or alter member branches without verified pull requests and real unit tests.
+- **KhachHang model boundary (2026-10-17 / resolved 2026-10-18):** `src/lib/models/khachhang.cpp` is fully implemented and tested with 8-field pipe serialization, CCCD/phone validation, and table/card display methods.
+- **FileIO tmps/ staging architecture (2026-10-18, audited & resolved):** Staging is synchronous. Dangling symlink recovery via `initTmpDir` self-heals `/dev/shm/ktlt_tmps` (0777) before local fallback. `commitFromTmp` preserves target and staging on failed rename; guards against 0-byte stream inserter `failbit`. `Repository<T>` provides strong exception safety (rollback in-memory state on persistence failure).
+- **Model Deserialization & Relational Integrity (2026-10-18):** `Date::parse` rejects trailing/mixed separators. Deserializers strictly validate field count and types. Menus use copy-then-update and reject absent foreign keys before `store.add()`.
+- **CTest & Platform Parity (2026-10-18):** `CMakeLists.txt` registers `unit_tests` with source directory working path. `run_windows.bat` matches all 16 sources. `compile_commands.json` untracked.
+- **Authentic Comment Style ("mystyle"):** Always lowercase, Vietnamese (ASCII/telex), no space after commas/slashes, no space after `//`. No vertical or horizontal excess stacking.
+- **Exhaustive Combinatorial Testing Invariant (2026-10-18):** `stress_test_runner` executes a 5-minute (300s) multi-stage combinatorial engine across all menus, abrupt EOF inputs, adversarial fuzz probes, and relational CRUD state transitions, verifying memory safety under ASan/UBSan with zero-SSD RAM tmpfs staging (`/dev/shm`).
+- **Official 5-Member Roster & Use Case Architecture (2026-10-18):**
+  1. Nguyễn Tùng Dương (`B24DCVT106`): Gói cước (`GoiCuoc`) & Thuê bao (`ThueBao`).
+  2. Trương Quốc Khánh (`B24DCVT201`): Khách hàng (`KhachHang`) & SIM eKYC.
+  3. Trần Đức Anh (`B24DCVT021`): Hợp đồng (`HopDong`) & Thiết bị đầu cuối (`ThietBiIMEI`, 3GPP Luhn Checksum).
+  4. Phạm Đức Chính (`B24DCVT047`): Nạp tiền (`NapTien`) & Bản ghi cuộc gọi (`CDR`).
+  5. Nguyễn Minh Quang (`B24DCVT308`): Hóa đơn (`HoaDon`) & Khiếu nại (`KhieuNai`).
+- **Menu UI & Clean Presentation Invariant (2026-10-18):** Keep user-facing CLI menus strictly clean of embedded author names, student IDs, and internal staging paths (`tmps/`). Maintain author attribution cleanly within `README.md` and module documentation.
+- **Stream Redirection Safety Invariant (2026-10-18):** Test harnesses manipulating `std::cin` and `std::cout` must use RAII guard (`StreamRedirectGuard`) to prevent dangling stream buffers upon assertion failures or thrown exceptions.
+- **Next-session deletion audit (2026-10-19):** Do not delete candidates on sight. First verify references,build ownership,graph edges,tests,and handoff value; present keep/delete recommendations and wait for explicit approval before removal.
+- **CCCD/customer-ID validation boundary (2026-10-19):** `Normalized::isValidCredential` is the single CCCD validator: exactly 12 ASCII digits, private accepted province-code table including `079`,`034`,`036`, and no real-date inference from birth/gender digits. `KhachHang` construction, setter, deserialization, and creation menu enforce it. `Normalized::isValidCustomerId` accepts legacy `KH0001` and expanded `KH0000001`–`KH9999999`; existing records remain unchanged.
+- **2026-10-22 closeout:** Persisted sorting and all-member Vietnamese UC01–UC10 documentation are complete. Final native evidence is 301/301 direct assertions, CTest 3/3, anti-cheat 0 strikes, and strict C++11 compile passed. Submission gaps remain: 19-page report versus required 20–30 pages, stale report test counts, underfilled financial/event fixtures, silent malformed-line skipping, and missing per-file author headers. Do not modify the protected dirty files without explicit approval.
+- **Submission/PR audit (2026-10-22):** `main` has no pending push (`origin/main...main = 0 0`). Open PRs #12/#13/#14 are duplicate conflict-dirty snapshots; #15 is a stale destructive snapshot despite `mergeable_state=clean` against its old base; #16 is conflict-dirty against an old teammate branch. Do not merge any without explicit rework/approval. Package fixture correction: `data/imei.txt` field 4 is `soDienThoai`, field 7 is BTS; rows 2–8 match `data/thuebao.txt`, while row 1's `ChuaGan` is an intentional unassigned sentinel.
+- **BT-02/BT-01 fixes (2026-10-23):** `Repository<T>::loadFromFile()` now fails closed on the first malformed row without publishing partial records; `main.cpp` aborts startup when any required load fails, preventing the exit save from dropping bad rows. `ThueBaoMenu` now receives `Repository<ThietBiIMEI>&` and rejects subscriber deletion while any IMEI is assigned to that phone. Real regressions live in `tests/test_runner.cpp` and `tests/repro/repro_20261023_014422_bt02_malformed_row_loss.sh` / `repro_20261023_014428_bt01_imei_delete_orphan.sh`. Post-fix evidence: direct suite 320/320, CTest 3/3, strict compile, and disposable probes passed.
+- **Modular Test Suite & GDB Stream Tracing (2026-10-24):** Monolithic `test_runner.cpp` is modularized into 5 files in `tests/modules/` under `tests/modules/test_common.h`, with top-level `runTest` exception isolation and RAII `FileCleanupGuard` and `StreamRedirectGuard`. Exact 347 assertions preserved. GDB batch mode dynamically verifies staging in `/dev/shm/ktlt_tmps`, `.ssd_swap` intermediate writing, and atomic `rename` on SSD storage (shortcut documented in `README.md` and `make trace-stream` in `Makefile`).
+
+
+- **3-Phase Lifecycle & Credential Probing Invariant (2026-10-24):** `tests/lifecycle_3phase_runner.cpp` executes 300s stress testing (100s Create & Swap, 100s Update & Duplicate Probing, 100s Delete & Drain) across 977M transitions in RAM tmpfs with zero SSD wear. Probing verified that secondary personal credentials (contact phones) permit shared household contacts, invalid formats roll back fail-closed, and primary keys uniquely enforce `DuplicateIdException`. Reverse-topological drain purges all temporary records to zero residue.
+- **5-Member File Responsibility Comments (2026-10-24):** Standardized file header comments applied across all 47 source/header files in `src/` attributing work to the official 5-member team roster (Trần Đức Anh, Tùng Dương, Trương Khánh, Nguyễn Minh Quang, Nguyễn Văn Chính). User-facing UI menus remain clean of author tags. Verified by `testSourceFileHeaders` in `tests/modules/test_models.cpp` (365/365 unit assertions passing).
+
+- **ZIP comparison audit (2026-11-03):** Report: `docs/audits/2026-11-03-zip-comparison.md`; handoff: `workspace/handoff/2026/11/2026-11-03.md`. ZIP base plus separate root source overlay matches audited executable code; final `fileio.h` difference is comment-only. Both ZIP/current lack the registered startup test script: isolated unit runner 431/0, CTest 2/3 (exit 8). Integrity gate rejects pre-existing test deletions. No fixes authorized or performed. Placement concerns are inherited; Luhn private helper/wrapper is not duplicate validation.
+
+- **Business Logic & Referential Integrity Invariants (2026-10-25):**
+  - **Postpaid Balances**: `ThueBao` enforces negative balance restrictions strictly on prepaid (`TraTruoc`); postpaid (`TraSau`) accounts legally accumulate negative balances pending monthly invoicing.
+  - **Subscriber Debt Scope**: `HoaDonMenu::thanhToanHoaDon` checks overdue unpaid invoices scoped to the paying subscriber phone (`updated.getSoDienThoai()`), rather than checking `kiemTraNo() == 0` globally.
+  - **EIR Blacklisting & SIM Mapping**: `ThietBiIMEIMenu` forbids assigning SIM numbers to blacklisted (`KhoaMang`) terminals per 3GPP EIR specifications, and enforces 1:1 phone-to-device mapping.
+  - **Contract Expiry Arithmetic**: `HopDong` expiration is calculated via `Date::addDays(gc->getChuKyNgay())` rather than defaulting to `ngayDK`.
+  - **Referential Deletion Protection**: `GoiCuocMenu` and `ThueBaoMenu` reject deletions when associated active subscribers, contracts, or unpaid invoices exist.
+  - **Top-Up Transaction Atomicity**: `NapTienMenu` rolls back persisted vouchers (`napTienStore.remove(maGD)`) if updating the subscriber balance fails.
+  - **Classifier False Positives**: `extractCpp` in `tools/dev-productivity/cmd/function-family-classifier/main.go` uses an 800-byte forward slice that overruns 1-line getters, capturing downstream methods into false positives.
+  - **ASCII Name Gate & 10-Use-Case Combinatorial Stress Test**: `Normalized::isValidName` enforces simple printable ASCII (32-126), <= 50 characters, non-empty, and no `|`. Combinatorial stress engine in `tests/stress_test_runner.cpp` covers all 10 use cases across the 5 member pairs with dynamic name/package generators, cascade deletion protection, and genuine measured metric reporting (no vanity strings).
+
+- **Teammate PR Integration & Roster Attribution (2026-10-26):** Teammate PRs (#19, #20 by Phạm Đức Chính) retargeted to member branch `DucChinh-B24DCVT047` and merged to protect `main` stability and prevent CDR test breakages. PR #16 by Trương Quốc Khánh merged locally into `QuocKhanh-B24DCVT201` with canonical validation preserved (3/3 test suites passing). GitHub contribution audit revealed 3 active teammate contributors + team lead; Nguyễn Minh Quang (`B24DCVT308`) currently has 0 PRs/commits and requires a personal PR to `MinhQuang-B24DCVT308` for course grading credit.

diff --git a/dev-productivity/src/anti-cheat/main.go b/dev-productivity/src/anti-cheat/main.go
index 5f7a87f..7ff9ceb 100644
--- a/dev-productivity/src/anti-cheat/main.go
+++ b/dev-productivity/src/anti-cheat/main.go
@@ -31,8 +31,8 @@ var (
 	reShellSetPlusE = regexp.MustCompile(`^\s*set\s+\+e\b`)
 
 	// Early Return / Process Exit in Tests
-	reEarlyReturn   = regexp.MustCompile(`^\s*(return|return\s+(True|False|0|1|None))\s*$`)
-	reProcessExit0  = regexp.MustCompile(`(?i)\b(sys\.exit\(\s*0\s*\)|os\._exit\(\s*0\s*\)|process\.exit\(\s*0\s*\))`)
+	reEarlyReturn  = regexp.MustCompile(`^\s*(return|return\s+(True|False|0|1|None))\s*$`)
+	reProcessExit0 = regexp.MustCompile(`(?i)\b(sys\.exit\(\s*0\s*\)|os\._exit\(\s*0\s*\)|process\.exit\(\s*0\s*\))`)
 
 	// Monkeypatching Assertion Primitives
 	reMonkeypatchAssert = regexp.MustCompile(`(?i)(TestCase\.(assert\w+|assertEqual|assertTrue)\s*=|global\.expect\s*=|window\.expect\s*=)`)
@@ -44,7 +44,7 @@ var (
 	reTargetInventoryDeletion = regexp.MustCompile(`(?i)(["'][a-zA-Z0-9_\-]+:(?:100\.\d+\.\d+\.\d+|192\.168\.\d+\.\d+|\d+\.\d+\.\d+\.\d+|none).*["']|\b(?:dell|hpdesk|ideapad|latitude|optiplex)\b.*(?:100\.|192\.168\.|\d+:dl-|\d+:hp-|\d+:dp-)|(?:ctid|vmid):\s*\d+,\s*name:\s*['"][a-zA-Z0-9_\-]+['"])`)
 
 	// Mocks
-	reShallowMock   = regexp.MustCompile(`(?i)(mockFetch\s*=\s*\(\)|mockImplementation\(\s*\(\)\s*=>\s*\(?\{|\bmock\.MagicMock\(return_value=\{\}\))`)
+	reShallowMock = regexp.MustCompile(`(?i)(mockFetch\s*=\s*\(\)|mockImplementation\(\s*\(\)\s*=>\s*\(?\{|\bmock\.MagicMock\(return_value=\{\}\))`)
 
 	// Exploits (ImpossibleBench)
 	reMagicEq      = regexp.MustCompile(`(?i)def\s+__eq__\s*\(self,\s*[^)]+\):\s*(return\s+True|return\s+1\s*==\s*1)`)
@@ -108,7 +108,7 @@ func logStrike(repoRoot string, v Violation) {
 	f.WriteString(entry)
 }
 
-func analyzeDiff(diffText string, requireTests bool) ([]Violation, map[string]bool) {
+func analyzeDiff(diffText string, prodFilesChanged, testFilesChanged map[string]bool) ([]Violation, map[string]bool) {
 	lines := strings.Split(diffText, "\n")
 	var currentFile string
 	var violations []Violation
@@ -117,8 +117,6 @@ func analyzeDiff(diffText string, requireTests bool) ([]Violation, map[string]bo
 	deletedAsserts := make(map[string]int)
 	addedAsserts := make(map[string]int)
 	isNewFile := make(map[string]bool)
-	prodFilesChanged := make(map[string]bool)
-	testFilesChanged := make(map[string]bool)
 
 	var addedBlock []string
 
@@ -410,28 +408,21 @@ func analyzeDiff(diffText string, requireTests bool) ([]Violation, map[string]bo
 		}
 	}
 
-	// ZERO UNIT TESTS FAIL: If production code modified without any test file touched
-	if requireTests && len(prodFilesChanged) > 0 && len(testFilesChanged) == 0 {
-		violations = append(violations, Violation{
-			Category: "NO_UNIT_TESTS_COVERAGE",
-			File:     "working_tree",
-			Line:     fmt.Sprintf("%d production files modified, 0 test files touched", len(prodFilesChanged)),
-			Detail:   "ZERO UNIT TESTS FAIL: Production code was modified without any accompanying unit tests or assertions. Write or update unit tests before completing task.",
-		})
-	}
-
 	return violations, filesToRevert
 }
 
 func checkDiff(repoRoot string, autoRevert, requireTests bool) int {
 	var allViolations []Violation
 	allFilesToRevert := make(map[string]bool)
+	// Share coverage across both groups, but keep tampering analysis independent.
+	prodFilesChanged := make(map[string]bool)
+	testFilesChanged := make(map[string]bool)
 
 	// Check 1: Unstaged diff (working tree vs index)
 	cmdUnstaged := exec.Command("git", "diff", "-U0")
 	cmdUnstaged.Dir = repoRoot
 	if out, err := cmdUnstaged.Output(); err == nil {
-		v, f := analyzeDiff(string(out), requireTests)
+		v, f := analyzeDiff(string(out), prodFilesChanged, testFilesChanged)
 		allViolations = append(allViolations, v...)
 		for k, val := range f {
 			allFilesToRevert[k] = val
@@ -442,13 +433,22 @@ func checkDiff(repoRoot string, autoRevert, requireTests bool) int {
 	cmdStaged := exec.Command("git", "diff", "--cached", "-U0")
 	cmdStaged.Dir = repoRoot
 	if out, err := cmdStaged.Output(); err == nil {
-		v, f := analyzeDiff(string(out), requireTests)
+		v, f := analyzeDiff(string(out), prodFilesChanged, testFilesChanged)
 		allViolations = append(allViolations, v...)
 		for k, val := range f {
 			allFilesToRevert[k] = val
 		}
 	}
 
+	if requireTests && len(prodFilesChanged) > 0 && len(testFilesChanged) == 0 {
+		allViolations = append(allViolations, Violation{
+			Category: "NO_UNIT_TESTS_COVERAGE",
+			File:     "working_tree",
+			Line:     fmt.Sprintf("%d production files modified, 0 test files touched", len(prodFilesChanged)),
+			Detail:   "ZERO UNIT TESTS FAIL: Production code was modified without any accompanying unit tests or assertions. Write or update unit tests before completing task.",
+		})
+	}
+
 	if len(allViolations) == 0 {
 		fmt.Println("[anti-cheat-enforcer] PASS: Diff is clean of cheating patterns (0 strikes).")
 		return 0

diff --git a/src/anti-cheat/main_test.go b/src/anti-cheat/main_test.go
new file mode 100644
index 0000000..78f41b4
--- /dev/null
+++ b/src/anti-cheat/main_test.go
@@ -0,0 +1,60 @@
+package main
+
+import (
+	"os"
+	"os/exec"
+	"path/filepath"
+	"testing"
+)
+
+func TestCheckDiffCoverageAcrossIndex(t *testing.T) {
+	for _, staged := range []string{"value.py", "test_value.py"} {
+		t.Run(staged, func(t *testing.T) {
+			repo := t.TempDir()
+			git := func(args ...string) {
+				t.Helper()
+				cmd := exec.Command("git", args...)
+				cmd.Dir = repo
+				if out, err := cmd.CombinedOutput(); err != nil {
+					t.Fatalf("git %v: %v\n%s", args, err, out)
+				}
+			}
+			write := func(name, text string) {
+				t.Helper()
+				if err := os.WriteFile(filepath.Join(repo, name), []byte(text), 0644); err != nil {
+					t.Fatal(err)
+				}
+			}
+			git("init", "-q")
+			write("value.py", "value = 1\n")
+			write("test_value.py", "from value import value\nassert value == 1\n")
+			git("add", ".")
+			git("-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "-c", "commit.gpgsign=false", "commit", "-qm", "fixture baseline")
+
+			write("value.py", "value = 2\n")
+			if got := checkDiff(repo, false, true); got != 1 {
+				t.Fatalf("production-only change: got %d, want rejection", got)
+			}
+
+			write("test_value.py", "from value import value\nassert value == 2\n")
+			git("add", staged)
+			if got := checkDiff(repo, false, true); got != 0 {
+				t.Fatalf("split production/test changes: got %d, want acceptance", got)
+			}
+			cmd := exec.Command("python3", "-B", "test_value.py")
+			cmd.Dir = repo
+			if out, err := cmd.CombinedOutput(); err != nil {
+				t.Fatalf("real fixture test: %v\n%s", err, out)
+			}
+
+			// Assertion removal must still fail in either diff group.
+			write("test_value.py", "from value import value\n")
+			if staged == "test_value.py" {
+				git("add", "test_value.py")
+			}
+			if got := checkDiff(repo, false, true); got != 1 {
+				t.Fatalf("deleted assertion: got %d, want rejection", got)
+			}
+		})
+	}
+}

```
