# Menu update parity implementation

Date: 2026-11-04. Baseline: [create/update audit](2026-11-03-create-update-parity.md).

## Batch ledger

| Batch | Ownership and behavior | State | Evidence |
| --- | --- | --- | --- |
| CP-01/CP-02 | Package menu: optional data/minutes/cycle setters | verified/reviewed; feature closed, aggregate gate exception retained | Exit-1 baseline: 447 passed/14 failed; after fix: 461 passed/0 failed |
| META-01 | Device menu: action 5 corrects labels only | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 479 passed/0 failed; PTY and quick stress exit 0 |
| META-02 | Customer menu: optional issue date/authority | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 506 passed/0 failed; PTY, quick stress and lifecycle exit 0 |

## Package evidence

- Reproducer: `tests/repro/repro_20261104_004646_package_update_parity.sh`; mint confirmed exit 1 before production edits. Its command uses this session's isolated build driver; durable assertions live in `tests/modules/test_menu_parity.cpp` and are registered in CMake.
- Debug C++11 application/unit/stress/lifecycle build: exit 0.
- Unit runner in copied-data cwd with source symlink: 461 passed, 0 failed, exit 0. Cases cover independent fields, unchanged values, zero, invalid numeric retries, cycle 1/365, EOF cancellation and missing ID. Existing assertions retained.
- Quick stress `--duration 5`: exit 0; 7,340 menu invocations, not a successful-update count. Debug build is not sanitizer qualification despite the runner's static banner.
- POSIX PTY probes: package persisted `AUDIT|Original|60000|10|250|60`; associated contract serialization retained price 50000 and expiry 31/01/2024. Failed-save case obstructed only an isolated fixture parent, reported an error, restored memory and retained the original disk record. Empty lookup created no package. All exited 0.
- Before/after menu classifier: 58 functions, no placement violations/duplicates. YAGNI auditor: exit 0.
- Adversarial inspection: zero remains a value, setters receive validated values, EOF returns before save, one repository update, no identity/history propagation.
- Live data hashes unchanged throughout isolated runs.

## Device evidence

- Reproducer `tests/repro/repro_20261104_005228_device_metadata_update.sh` confirmed exit 1 before production changes; direct setters were exercised by expected-record construction while the existing menu rejected action 5.
- Registered real menu/reload cases cover both labels, blanks, 50/51-character boundaries, pipe retry, EOF, cancellation and missing ID; final runner: 479 passed, zero failed.
- PTY reload: `860123456789014|New Model|New Brand|0981234567|15/01/2024|KhoaMang|BTS-HN-001`. Identity, mapping, activation date, blacklist and BTS preserved.
- Quick stress exit 0 (7,572 menu invocations); live data hashes unchanged. Existing SIM/blacklist tests retained and passed in unit runner.
- Review: action numbers 1–4 unchanged; validated inputs finish before setters and one repository save; EOF cannot publish the first label. Classifier/YAGNI/whitespace pass; aggregate integrity exception unchanged.

## Customer evidence

- Reproducer `tests/repro/repro_20261104_005414_customer_issue_metadata.sh` confirmed exit 1 before implementation. Existing menu saved after address without reading issue metadata; direct setters/serialized reload supported the expected fields.
- Final units: 506 passed, zero failed. Real reload cases cover leap day, authority-only edit, blank preservation, delimiter retry, cancellation of earlier name edits, invalid leap day/trailing text, EOF before authority, missing ID and `now` across midnight.
- PTY reload: `KH0001|Nguyen Van An|CCCD|001201004567|29/02/2024|Cuc C08|0981234567|Ha Noi`.
- Debug C++11 application/unit/stress/lifecycle build exit 0. Quick stress exit 0 (7,598 menu invocations); lifecycle `--quick` exit 0, all five repository counts returned to baseline, zero residual records. Counts are runner metrics, not independently proven successful-update totals.
- Review: date is explicitly parsed because model setters only assign; ID/CCCD/type remain immutable; one save after all inputs; invalid date/cancel/EOF leaves pending copy unpublished. Approved three lifecycle input payload corrections applied without assertion changes.
- Classifier/YAGNI/whitespace pass. Live data hashes unchanged. Aggregate anti-cheat exception retained.

## Final verification and limitations

Follow-up gate repair (2026-11-04) supersedes the historical red-gate results below: startup restored with original assertions; isolated CTest now passes 3/3; rebuilt installed anti-cheat passes with `--no-revert`. See [verification gate repairs](2026-11-04-verification-gate-repairs.md) for root causes, real-Git regressions and staging caveat. Historical failures remain recorded, not erased.

- Fresh isolated unit rerun on 2026-11-04: 506 passed, 0 failed, exit 0; live-data SHA-256 values match both the before-run values and implementation baseline. Expected invalid-BTS-fixture diagnostic is not a failed assertion.
- Isolated CTest: 2/3 passed, exit 8. `unit_tests` and `stress_test_quick` passed. `startup_from_other_directory` failed with `/usr/bin/bash: /home/vgbn/Documents/codeptit/KTLT/tests/repro/startup_working_directory.sh: No such file or directory`. The script's staged deletion predates implementation and remains untouched. The harness retained registered commands but changed their working directories and live-data argument to a copied fixture; this is not an untouched source-cwd CTest execution.
- Documentation referee reported 45 stale-reference candidates across six existing documents. Shorthand paths, historical archive paths and missing historical files require individual screening; do not label all candidates confirmed defects. No broad documentation repair was attempted.
- `graphify update .` exited 0: 1011 nodes, 2203 edges, 43 communities; regenerated graph JSON/HTML, report and one callflow. Saved labels describe 61 communities, so 25 current communities received hub-derived names. No LLM relabeling performed. Tool-created backup retained under `graphify-out/2026-11-04/`: `.graphify_labels.json`, `manifest.json`, `graph.json`, `.graphify_analysis.json`, `GRAPH_REPORT.md`.
- All three minted reproducer scripts were inspected: they depend on the session's scratch driver/build and are not portable after scratch cleanup. Permanent registered assertions are in `tests/modules/test_menu_parity.cpp`. Preserve historical mint evidence rather than rewriting its commands to imply portability.
- Full explicit tracked/new-file diffs: `2026-11-04-menu-parity-diffs.md`. Tracked code diffs use HEAD, not a pristine pre-implementation working snapshot; package source was already dirty, so its entire HEAD diff cannot be attributed to this session.
- Historical-audit preservation probe exited 1 on `data/CacGoiCuoc.txt`. Differential inspection found three historical data differences (`CacGoiCuoc.txt`, `hopdong.txt`, `khachhang.txt`), all already present in the implementation-start hash baseline; current data matches that baseline exactly. The old audit snapshot also differs at the already-dirty `fileio.h`, so it cannot certify whole-session preservation. No attempt was made to restore these user-owned files.
- Fresh final POSIX PTY rerun exercised all five modes together: package, blocked-save rollback, empty lookup, device labels and customer issue metadata. Every process exited 0, real assertions passed, expected rollback error was displayed, deserialized records matched, and live-data hashes were unchanged.
- Final independent read-only review returned no concrete regressions in the three update paths, action constants and parity tests. This is scoped source review, not whole-repository certification. Its observation about package pause behavior was checked: `capNhatGoiCuoc` itself does not pause, but `showMenu` calls `InputHelper::pause()` after dispatch at line 58; the actual menu still pauses, consistent with PTY payloads. Existing customer blank-phone behavior preserves the phone and does not offer clearing it; that is outside the approved metadata scope.
- No all-green acceptance claim is permitted while the startup and integrity gates remain red. Final quick lifecycle rerun exited 0, restored all five repository counts and left live-data hashes unchanged.
- An agent worktree was inadvertently requested for final review; the reviewer was redirected to live absolute paths, read-only. Existing worktrees were inventoried, not removed. No commit/push/merge or archive/history/timestamp manipulation was performed.

## Known aggregate gate exception

`anti-cheat-enforcer --diff --require-tests --no-revert`: exit 1, reports 23 production files/zero tests in its working-tree group. The 23 staged deleted historical scripts predate this work. New registered test source is untracked, and tracked test wiring/input corrections do exist; this tool output is not a valid count of tests executed. The gate was invoked with `--no-revert`; historical assertions were not removed by this implementation. Its appended integrity-log entries still say `File auto-reverted via git checkout -- working_tree`; that wording is not proof a real file was reverted. Fresh native tests and the inspected feature diff remain intact. Do not claim aggregate integrity acceptance.

## Scope and rulings

- Ruling: zero is data, not skip — y/n distinguishes unchanged values — cost if wrong: accidental catalog edits.
- Ruling: correct labels/issue metadata, not identity — preserve relationships — cost if wrong: orphaned records.
- Ruling: retain unrelated dirty files and red gates — feature approval is not cleanup approval — cost if wrong: lost work/evidence.
- User changed comment preference during execution: new comments use lowercase unaccented Vietnamese like surrounding source, rather than the plan's English comments.
- IDs, existing contract snapshots, historical records, archives and Git metadata remain outside scope. No commit/push/merge or archive regeneration authorized.

## Verification commands

Use a disposable copied-data working directory with a source symlink when running native tests; CTest's configured source working directory must not be allowed to write live data. Session scripts/builds are under `$CLAUDE_JOB_DIR/tmp` and are not permanent build prerequisites.

```bash
cmake -S "$PWD" -B "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" --target test_runner quanlythuebao stress_test_runner lifecycle_3phase_runner -j 4
# Run test_runner from the disposable fixture cwd, not live data/.
function-family-classifier check src/lib/menus
yagni-diff-auditor
anti-cheat-enforcer --diff --require-tests --no-revert
```
