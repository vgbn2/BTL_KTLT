# ZIP Comparison and Function Placement Audit — 2026-11-03

## Verdict

The inspected placement concerns are inherited from `Nhóm 04.zip`, not introduced by the current source. The ZIP is not a single clean project snapshot: it contains an older `Nhoms4/` project and a separate root `src/` overlay. Combining those source trees explicitly yielded all 47 audited source files byte-for-byte. At final closeout, current `src/lib/shared/fileio.h` differed only by removal of `(atomic)` from a header comment; executable code remained unchanged. Simply extracting the ZIP does not apply that overlay automatically.

Mode: `review`; Hard Reading Mode. Source audit only, no production fixes, moves, test restores, commits, pushes, or merges.

## Baseline and scope

- Working tree: `/home/vgbn/Documents/codeptit/KTLT`, `main`, HEAD `b5d5493`.
- Archive: `Nhóm 04.zip`, 218,096 bytes.
- SHA256: `47820d9444ac31aeb952ca0445687a3a2da3f642b0a1ba5932571891139d6328`.
- Archive inventory: 100 entries; 90 regular files, 9 directories, 1 symlink. The `Nhoms4/src/data -> ../data` symlink was not extracted; no absolute/traversal paths were found.
- Base `Nhoms4/`: 47 source files, 3 build files, 11 test files, 12 data files, 2 documentation files.
- Base source comparison: 38 identical, 9 different; no BOM/line-ending-only differences. Differences are in menus, including method ordering and subscriber update functionality.
- Root overlay: 15 menu/main source files, all identical to current HEAD. Overlay plus base: 47/47 audited source files identical; final current-tree check found only the comment-only `fileio.h` difference described above.
- The overlay does not update the ZIP's older `tests/modules/test_menus.cpp`.

## Findings, severity ordered

### AUD-01 — Medium: registered startup test is missing in both delivered trees

- Anchor: `CMakeLists.txt:77–80`.
- Failing boundary: CTest invokes `tests/repro/startup_working_directory.sh`, which does not exist.
- Current working tree: file is staged deleted; the script still exists in HEAD.
- ZIP: same CMake registration, but the script is absent from all archive entries. This is not current-only.
- Empirical evidence: disposable Debug build succeeded; CTest exited 8 with 2/3 passing and Bash reporting `No such file or directory` for the script.
- Differential check: the HEAD script, separately run against disposable binary/data, exited 0. This does not turn the original CTest run into a pass.
- Fault domain: `our_source` (test packaging/registration); repair owner: CMake/test packaging; stub involvement: `none`; confidence: high.
- Acceptance criterion: the actual delivered tree includes the registered script and passes all three CTests without substituting a script from another revision.

### AUD-02 — Medium: archive contains two non-merged source layouts

- Archive paths: `Nhoms4/src/lib/menus/*` versus `src/lib/menus/*` and `src/main.cpp`.
- Boundary: extracting the archive creates two source roots; the newer files are not placed inside the project selected by `Nhoms4/CMakeLists.txt`.
- Cause: newer source files were added under a different archive prefix. The base subscriber menu lacks the current update option; the root overlay includes it.
- Evidence: ZIP entry inventory, exact byte comparison, and subsequent Debug build/test of the base archive project. The separate source overlay was not applied.
- Fault domain: `our_source` (submission packaging); repair owner: submission/archive assembly; stub involvement: `none`; confidence: high.
- Acceptance criterion: one unambiguous project root containing the selected source, matching tests, data and registered startup script. Re-extract and verify before submission.

### PLACE-01 — Low, structural debt: CDR persistence uses a UI formatter

- Current: `src/lib/models/ghicuocgoi.cpp:157–160`; ZIP: `Nhoms4/src/lib/models/ghicuocgoi.cpp`, identical bytes.
- Trace: `Repository<CDR>::saveToFile` calls `CDR::toFileString`, which calls `DisplayHelper::formatCurrency` (`DisplayHelper.h:87–91`) for persisted cost.
- Concern: storage numeric representation depends on a presentation helper. Formatting changes could change the file contract. No current parsing failure was reproduced or claimed.
- Smallest cleanup: format the persisted number within the serializer, preserving the existing representation; no new service or class required.
- Boundary: serializer → presentation helper; fault domain: `our_source`; owner: CDR serialization; stub involvement: `none`; confidence: high for dependency, not a claim of runtime failure.
- Acceptance: unchanged serialized fixtures and round-trip tests, independent of presentation suffix/grouping changes.

### PLACE-02 — Low, intentional architectural coupling: domain models render console tables/cards

- Current: `src/lib/shared/Entity.h:31–35`, implemented in all nine model types; ZIP: identical base files.
- Trace: `DisplayHelper::printTable` calls model `displayHeader`/`displayRow`; model methods supply console labels, widths and alignment.
- Concern: models own presentation as well as domain state. Under strict layering, renderers belong in presentation code.
- Counterevidence: this is a consistent, explicit base-class contract in an academic console application, not a recent accidental placement or a demonstrated runtime bug.
- Boundary: model → console rendering; fault domain: `our_source`; owner: Entity/model presentation contract; stub involvement: `none`; confidence: high for coupling, conditional for whether to refactor.
- Recommendation: retain unless a headless/second UI requirement justifies moving rendering. Do not add a presenter framework merely to satisfy a classifier.

## Luhn helper specifically requested by the user

`src/lib/shared/Normalized.cpp:12–15` defines `static int doubleLuhnDigit(int digit)`. Its sole source caller is `Normalized::isValidLuhn` at line 233. `static` gives file-local linkage; the helper doubles a digit and subtracts nine when necessary. The caller validates the input as 15 decimal digits before reaching it.

Call chain: menu/constructor/setter → `ThietBiIMEI::validateLuhn` (`imei.cpp:65–67`) → `Normalized::isValidLuhn` (`Normalized.cpp:211`) → `doubleLuhnDigit` (`Normalized.cpp:12`).

The ZIP's entire `Normalized.cpp` is byte-identical to current. This is not duplicated validation and not a current-only addition. `Normalized` already deliberately owns phone, customer-ID, CCCD and Luhn validation under the project convention. Its name is broader in practice than string normalization alone.

Strict domain-layer alternative: move the IMEI-specific validator and its private arithmetic helper together to the IMEI implementation, adjusting callers. Moving only `doubleLuhnDigit` is not a coherent boundary fix. No move was performed or required by this audit.

## Dismissed and conditional candidates

- `ThietBiIMEI::validateLuhn`: delegates to the one canonical algorithm; not duplicate Luhn logic. A domain-facing wrapper is defensible, not a confirmed misplaced function.
- `ThietBiIMEI::generateSimulatedImei` (`imei.cpp:71–89`): explicitly simulated, used by a real menu option and tests, identical in ZIP. Domain generator placement is a design choice; no demonstrated requirement makes it wrong.
- `BTSRegistry` (`BTSRegister.h:5–6`): explicitly experimental and absent from production menu callers. Potential directory/build hygiene debt, but not evidence that its read-only external-dataset parser must use `Repository<T>`. It already reuses `Normalized::trim` and `FileIO::split`.
- Repository calls in menus: intentional controller orchestration and referential-integrity checks, not raw file I/O embedded in presentation.
- `Date::toString`, shared console input utilities, file I/O staging, generic Repository storage, and same-named getters across different classes: classifier heuristics, not independently proved placement defects.
- `showMenu()` at top versus bottom: method order, not responsibility placement.
- Dirty data: package rows reordered; one subscriber changed from `TraSau` to `TraTruoc`. No automatic corruption finding. ZIP data matches HEAD for these files, not the two local edits.

## Verification evidence and limitations

Initial native verification used a disposable copy of current source/tests/data, keeping missing files missing. Subsequently, the user explicitly requested archive-native verification: the ZIP base `Nhoms4/` project was safely extracted and built/tested separately, without applying the root source overlay. Both builds used Debug mode with assertions retained.

| Check | Result |
| --- | --- |
| CMake configure/build: app, unit runner, stress runner | Exit 0 |
| Unit runner | Exit 0; 431 passed, 0 failed |
| Quick stress, 5 seconds | Exit 0; observed 17,293 transitions, 7,778 menu invocations, 1,361 CRUD operations |
| CTest, timeout 60 seconds | Exit 8; 2/3 passed; missing startup script |
| HEAD startup script, separately on disposable binary/data | Exit 0; separate evidence only |
| Function classifier | 326 functions scanned; 22 warnings and 7 duplicate-signature candidates screened; not accepted wholesale |
| Complexity diagnostic | 47 source files, reported score 99/100; no blocking hotspots; not a functional qualification |
| Documentation drift sentinel | Exit 1; 41 candidate references in 18 files; not individually verified defects |
| Anti-cheat `--diff --require-tests --no-revert` | Exit 1; tool labels 23 existing deleted repro scripts as production changes with no tests; not an audit-induced source change |
| `git diff --check` | Exit 0 |
| Original-file preservation hashes | Initial recheck: 75 unchanged. Final recheck: 74 unchanged; `fileio.h` has only a comment change, not authored by this audit |

Final closeout reran the isolated unit runner (exit 0, 431 passed/0 failed), CTest (exit 8, 2/3 passed, same missing script), anti-cheat (exit 1, same pre-existing deletions), documentation drift (exit 1, unscreened candidates), and `git diff --check` (exit 0). The isolated snapshot differs from final current executable source only in the non-executable header comment noted above.

Do not repeat older claims of 416 assertions or 3/3 passing CTests: fresh current evidence is 431 assertions and 2/3 CTests. No sanitizer, long soak, or full interactive PTY qualification is claimed.

### Follow-up: exact ZIP base build and tests

User requested direct archive verification. Source: `/home/vgbn/.claude/jobs/d64cc50d/tmp/zip_extract_02oqkjd7/Nhoms4`; build: `/home/vgbn/.claude/jobs/d64cc50d/tmp/build_nhoms4`. Safe extraction omitted the `src/data` symlink; root `src/` overlay was deliberately not applied. Original repository fixtures were not used for writes.

- Debug CMake configure/build succeeded. All six targets built: application, unit runner, stress runner, malformed fuzzer, deep CRUD tester and lifecycle runner. Building these targets does not mean all runners were executed.
- Supervisor reran `cmake --build ... -j2`: exit 0; cache confirms `CMAKE_BUILD_TYPE=Debug`.
- Supervisor reran direct `test_runner` from extracted base: exit 0, **426 passed, 0 failed**. The malformed-coordinate diagnostic is expected fixture output, not a failed assertion.
- Supervisor reran CTest: exit 8, **2/3 passed**. Unit and five-second quick stress passed; startup test could not execute because its script is absent from the ZIP.
- Delegated independent five-second stress: exit 0, 16,911 transitions reported. This is a different run from the supervisor's CTest stress.
- Delegated headless piped-input smoke test: copied executable with adjacent isolated data, launched from unrelated `/tmp`, supplied option `0`; exit 0 and empty stderr. Without discoverable data, exit 1 with a clear diagnostic. This is startup/exit evidence, not full interactive qualification.
- Follow-up integrity check: `git diff --check` exit 0; anti-cheat exit 1 for the same pre-existing deleted reproducer scripts, with `--no-revert`.

Conclusion: the ZIP base application builds and passes its included unit/quick-stress tests, but its full registered CTest suite is not passing. Its older menus and separate unapplied overlay remain a packaging/version-selection concern. Current-tree count is 431 assertions; ZIP-base count is 426.

Supervisor rerun logs: `zip-build-verified.txt`, `zip-ctest-verified.txt`, and `zip-unit-verified.txt` under the job temporary directory.

Full temporary logs are under `/home/vgbn/.claude/jobs/d64cc50d/tmp/`; the durable outcome is this report. Optional npm hygiene/structure scripts and workstation restore drill were absent and were not claimed as passing. Graph lookup was used; no graph refresh is required because production code did not change.

Open PRs #25/#26 touch subscriber menu implementation/header; both were reported conflicting by GitHub. No merges or remote mutations were performed. Cached `origin/main...HEAD` was `0 0`, not a fresh remote synchronization claim.

## Implementation handoff and safety boundary

1. Prioritize AUD-01/AUD-02: restore an approved startup test in the delivery and produce one coherent archive root with matching tests. Gate: fresh extraction, Debug build, 431 current unit assertions and all registered CTests passing. Functional route: bounded `codex` work after approval.
2. PLACE-01 is a small behavior-preserving serialization cleanup if desired. Preserve numeric encoding and exercise real serialization/parser round trips. Route: `refactor-readability` with tests.
3. PLACE-02 is optional, broader contract work; do not start without a concrete need and approved scope.

All existing data edits, staged deletions, archive changes, swap files and session output were preserved. No evidence authorizes deleting experimental code, removing wrapper APIs, modifying fixtures, or merging conflicting PRs.
