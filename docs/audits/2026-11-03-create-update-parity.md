# Create/update feature parity audit

Date: 2026-11-03. Mode: review, Hard Reading Mode. Target: live `main` at `b5d5493`, including existing uncommitted changes. Audit only: no production fixes, archive edits, timestamp changes, commits, pushes, or merges.

## Confirmed findings

| ID | Severity | Boundary | Evidence and impact |
| --- | --- | --- | --- |
| CP-01 | Medium | `src/lib/menus/GoiCuocMenu.cpp:187-195` | Update reads minutes and cycle length into local variables but never calls `updated.setSoPhutGoi` or `updated.setChuKyNgay`. Real PTY runs entered 250 minutes or 60 days separately; reloaded records retained 100 minutes and 30 days while the menu reported success. These prompts are in the pre-existing dirty diff. |
| CP-02 | Medium | `src/lib/menus/GoiCuocMenu.cpp:174-194` | Create accepts data allowance at lines 77-85; update offers no data prompt or setter. A 5 GB record cannot be changed to 10 GB through this menu. The model setter and repository do persist 10 GB when used directly. |

Attribution for both: `fault_domain: our_source`; `repair_owner: GoiCuocMenu::capNhatGoiCuoc`; `stub_involvement: none`; confidence high. The first failing boundary is input-to-copied-model mutation, before repository persistence. No environment, credential, external provider, or serializer failure is required to explain the result.

## Bayesian isolation evidence

1. **Fault classification:** source controller omission versus model rejection versus persistence loss.
2. **Predictions:** if the controller is responsible, direct setter/update/reload succeeds while the menu path fails for the same field.
3. **Binary probe:** direct setters for all five editable package attributes persisted correctly. Create/reload also succeeded.
4. **A/B differential:** menu name and price edits persisted; menu minutes/cycle edits did not. Data editing has no input path. Each update case changed one target field only.
5. **Line attribution:** missing setter wiring at lines 187-194; absent data edit between the copied model at line 174 and commit at line 194. Serialization and repository are cleared by the direct-setter controls, not by assumption.

## Package field matrix

| Field | Create | Update | Evidence |
| --- | --- | --- | --- |
| Package ID | Yes | Lookup only | Identity, not a missing ordinary edit |
| Name | Yes | Yes | Menu/reload exit 0 |
| Price | Yes | Yes | Menu/reload exit 0 |
| Data allowance | Yes | No input path | Menu parity expectation exit 1; direct model control exit 0 |
| Voice minutes | Yes | Prompt only, discarded | Menu parity expectation exit 1; direct model control exit 0 |
| Cycle days | Yes | Prompt only, discarded | Menu parity expectation exit 1; direct model control exit 0 |

## Cross-menu coverage and candidate gaps

The six sibling controllers were source-reviewed separately from the package runtime probes. This is a create/update inventory, not proof that every sibling workflow passes. A model setter alone does not establish a requirement to expose it in the UI.

| Controller | Existing edit or dedicated action | Remaining asymmetry / disposition |
| --- | --- | --- |
| `GoiCuocMenu` | Name and price updates persist | Confirmed CP-01/CP-02 above: three other mutable fields are ineffective or unavailable |
| `ThueBaoMenu` | Type, customer owner, activation date; dedicated package registration, lock/unlock and top-up | No additional confirmed parity defect from source review; dedicated actions must not be mistaken for absent update support |
| `KhachHangMenu` | Name, contact phone, address (`src/lib/menus/KhachHangMenu.cpp:216-229`) | Creation also accepts credential issue date/authority (`:98-111`), absent from update. Candidate correction feature; credential replacement/validation policy needs definition before implementation |
| `HopDongMenu` | Package/price, renewal, status (`src/lib/menus/HopDongMenu.cpp:255-315`) | Customer-owner reassignment is absent although a model setter exists. Policy-dependent, not a confirmed bug: check subscriber/contract ownership consistency before allowing transfers |
| `ThietBiIMEIMenu` | SIM reassignment, BTS, blacklist lock/unlock (`src/lib/menus/ThietBiIMEIMenu.cpp:272-332`) | Device name/manufacturer are entered at creation (`:97-100`) but cannot be corrected in update. Source-backed feature candidate; no sibling menu/reload reproduction performed |
| `HoaDonMenu` | Invoice payment; complaint status update | No unrestricted financial-record edit inferred as required; absence of amount/history editing is not reported as a defect without a contract |
| `NapTienMenu` | Top-up and CDR recording/history workflows | No unrestricted transaction/CDR correction inferred as required; reversal/correction semantics need a separate contract |

The highest-value follow-up after the confirmed package omissions is device metadata correction, then customer credential metadata correction. Contract ownership transfer remains a domain-policy question. IDs, calculated balances, historical amounts, and lifecycle changes are not ordinary free-form edits merely because creation or a model API exposes related values. No regulatory or accounting compliance claim is made by this audit.

## Runnable evidence

`tests/audit_menu_parity.cpp` is an audit probe, not registered as a passing CTest test. It uses the real public menu dispatch, real model, real repository files, and real deserialization. It returns 0 for a matching record, 1 for a parity violation, and 2 for setup/usage errors. A failing result is intentional evidence of an unfixed gap; it is not a repaired regression.

Build the project normally, then link this probe with the library object files from `test_runner` (not its test/main objects):

```bash
ROOT="$PWD"
BUILD="$CLAUDE_JOB_DIR/tmp/parity-build"
cmake -S "$ROOT" -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$BUILD" --target test_runner quanlythuebao stress_test_runner -j 4
mapfile -t OBJECTS < <(fd -a -e o . "$BUILD/CMakeFiles/test_runner.dir/src/lib")
g++ -std=c++11 -g -Isrc/lib/menus -Isrc/lib/models -Isrc/lib/shared \
    tests/audit_menu_parity.cpp "${OBJECTS[@]}" -o "$BUILD/audit_menu_parity"
WORK=$(mktemp -d "$CLAUDE_JOB_DIR/tmp/parity-check.XXXXXX")
(cd "$WORK" && printf '5\nAUDIT\n\nn\n250\n30\n\n0\n' | \
    "$BUILD/audit_menu_parity" "$WORK/minutes.txt" menu minutes)
# Expected exit 1: persisted AUDIT|Original|50000|5|100|30.
(cd "$WORK" && "$BUILD/audit_menu_parity" "$WORK/model.txt" model minutes)
# Expected exit 0: persisted AUDIT|Original|50000|5|250|30.
```

The executed PTY driver and fresh binaries are session scratch under `$CLAUDE_JOB_DIR/tmp`; durable probe source and evidence are in this repository. Executed controls: create once; five direct model field updates; five menu field attempts. Eight expected passes and three expected failures. The data case represents an unavailable requested edit, not rejected valid data input: no data prompt exists. All repository data files were hashed before/after the PTY/native suite pass and were unchanged.

## Verification and limitations

- Fresh Debug build of unit, application, and stress targets: exit 0.
- PTY driver uses stdlib POSIX PTY; `pyte` is unavailable. `verify-headless-agent-env`: 6/6 reported passes, exit 0; no graphical UI qualification claimed.
- Unit runner in isolated cwd with copied data and read-only source symlink: **431 passed, 0 failed**, exit 0.
- Initial isolated unit run without source fixtures: 419 passed, 12 failed. Restoring source visibility alone cleared all 12; these were probe setup failures, not reported product defects.
- Native quick stress (`--duration 5`): exit 0; latest rerun printed 7,598 interactive menu invocations and 1,348 direct CRUD operations. No soak or exhaustive transition coverage claimed.
- Registered startup CTest: fails because the existing deleted startup script is absent. No aggregate CTest pass claimed.
- `git diff --check`: exit 0.
- Menu family classifier: 58 functions, 0 violations, 0 duplicates, exit 0. Heuristic output is not feature coverage.
- Complexity command exits 0 but reports only six lines for the 249-line package file; its score is not used as a cleanliness grade.
- `anti-cheat-enforcer --diff --require-tests --no-revert`: exit 1 on existing dirty/staged groups (one production change group and 23 deleted scripts; tool sees zero tracked test changes). No user files reverted; new untracked audit probe does not make this aggregate gate clean.
- `doc-drift-sentinel --dir docs --strict`: exit 1, latest closeout 45 candidates across 20 files (previously 44/19). The additional warning is this report explicitly documenting the unavailable hygiene script. Shorthand paths and archive-relative references are not automatically defects; remaining candidates not individually audited here.
- Repository lacks `scripts/dev/check_hygiene.js`, `package.json`, and the requested restore-drill path; those generic Node/host checks are unavailable, not passes.
- Open PR #26 only adds the subscriber update declaration/constant, not implementation; PR #25 changes subscriber implementation. Both reported conflicts. No merges performed.

## Implementation handoff

| Finding | Acceptance criteria | Safety boundary | Verification gate |
| --- | --- | --- | --- |
| CP-01 | Entered minutes/cycle values reach the copied model and survive reload; unchanged fields stay unchanged | Reuse existing model setters and transactional `store.update`; preserve EOF cancellation and IDs | Real menu/reload cases for each field, then unit/stress suite |
| CP-02 | Data allowance can be edited to a valid nonnegative value or left unchanged | Reuse `setDungLuongData`; no ad hoc text-file editing | Real menu/reload case, including zero and unchanged value |

Recommended smallest implementation surface: `GoiCuocMenu::capNhatGoiCuoc`, with optional numeric updates consistent with the existing price confirmation pattern. No need for a generic form framework or a repository rewrite. Do not silently rewrite existing contract prices/expiry dates when editing a catalog package; any propagation policy is a separate domain decision.

Before implementation, choose whether to complete all three missing package attributes or deliberately retain a narrower edit contract. Complete parity means five mutable package fields; the current reliable menu coverage is two of five. This is a field count, not a quality grade.
