# Mobile Subscriber Management System

C++11 command-line application for managing mobile subscribers, plans, contracts, devices, top-ups, CDR records, invoices, and complaints.

This course project simulates telecom workflows with local text files. It does not connect to a carrier network, EIR, payment provider, SMS gateway, or real radio infrastructure.

## Features

- Customer records with CCCD and phone validation
- Prepaid and postpaid subscriber management
- Mobile plan management
- Contract creation, search, update, renewal, suspension, and termination
- IMEI validation with the 3GPP Luhn checksum
- Simulated IMEI generation for demo data
- Device/SIM assignment and offline BTS registry lookup
- Top-up and CDR records for voice, SMS, and data
- Invoice and complaint management
- Repository-based CRUD with text-file persistence
- Staged file writes with rollback on persistence failure
- Vietnamese console menus

## Requirements

### Linux

Required tools:

- GCC/G++ or Clang with C++11 support
- GNU Make
- CMake 3.10 or newer
- Git, if cloning the repository
- GDB (GNU Debugger) for runtime inspection and stream tracing

On Debian/Ubuntu:

```bash
sudo apt update
sudo apt install -y build-essential cmake git gdb
```

On Fedora:

```bash
sudo dnf install -y gcc-c++ make cmake git gdb
```

On Arch Linux:

```bash
sudo pacman -S base-devel cmake git gdb
```

Verify the installation:

```bash
g++ --version
make --version
cmake --version
git --version
gdb --version
```

### Windows

Choose one supported setup:

1. **MinGW-w64 or MSYS2 MinGW-w64** — provides `g++`, `make`, and optionally CMake. Add the compiler's `bin` directory to `PATH` if using a regular Command Prompt or PowerShell.
2. **Visual Studio 2019/2022** — install the **Desktop development with C++** workload and the CMake tools component.
3. **Git for Windows** — recommended for cloning and for the Git Bash commands below.

The included `run_windows.bat` is the simplest MinGW path. It automatically sets the UTF-8 code page (`chcp 65001`), requires `g++` on `PATH`, compiles with C++11 and warnings enabled, then starts `quanlythuebao.exe`.

Check a MinGW/MSYS2 installation in Command Prompt or Git Bash:

```bat
g++ --version
cmake --version
git --version
gdb --version
```

To install GDB on Windows via MSYS2:

```bash
pacman -S mingw-w64-x86_64-gdb
```

> **Note on UTF-8 in Windows Terminal:**
> The application uses Vietnamese console menus. If running directly from Command Prompt or PowerShell without `run_windows.bat`, set the console code page to UTF-8 before running:
> - Command Prompt: `chcp 65001`
> - PowerShell: `[Console]::OutputEncoding = [System.Text.Encoding]::UTF8`

## Get the source

### GitHub Codespaces: individual terminal or shared session

The checked-in Dev Container config installs the C++/CMake toolchain and builds the project at creation. For an individual terminal-only Codespace, install and authenticate GitHub CLI locally, then run:

```bash
gh auth login
gh codespace create --repo OWNER/REPOSITORY --branch main
gh codespace list
gh codespace ssh --codespace CODESPACE_NAME
```

Replace `OWNER/REPOSITORY` with this repository's GitHub slug and `CODESPACE_NAME` with the name returned by `gh codespace list`. The last command attaches the current terminal to the Codespace. Do not run `gh codespace code` if you want to stay in a raw terminal; it opens a VS Code-compatible editor.

To share one Codespace with collaborators, the host must open that Codespace in the browser or desktop VS Code, start a **Live Share** session, and send the invitation link to trusted collaborators. The Dev Container recommends the Live Share extension. Guests join through a VS Code-compatible client; this is collaboration in the host's Codespace, not separate SSH logins to a shared shell. The host must keep the Codespace and Live Share session running. Stop sharing from the Live Share panel when finished.

#### Joining from Windows

1. Install [Visual Studio Code](https://code.visualstudio.com/) on Windows, then install the **Live Share** extension (`MS-vsliveshare.vsliveshare`). Alternatively, open the invitation in a supported browser-based VS Code session.
2. Sign in if prompted, then open the Live Share invitation link sent by the host and choose **Join**.
3. To use the shared app terminal, ask the host to share a terminal and choose **read-only** or **read/write**. Terminals are not shared automatically. Read/write lets guests run commands in the host's Codespace, so only use it with trusted collaborators.
4. In the shared terminal, run `make test` or `./build/quanlythuebao`. The terminal and filesystem belong to the host's Codespace; do not edit or commit shared `data/` files unless the host has prepared a disposable test copy.

If the invitation does not open the Live Share session, install/enable the extension and ask the host to send a fresh invite while the Codespace is running.

Inside the remote shell, run:

```bash
make test
ctest --test-dir build --output-on-failure
./build/quanlythuebao
```

The application uses the checked-out repository's `data/` files. Do not use real personal data or commit mutated test data. A normal Codespace is per-user; a Live Share guest joins the host's active session and can access the shared workspace/session features according to the host's sharing permissions. Only invite trusted people. Each participant needs the access and GitHub account eligibility required by the repository/organization. Pull requests and pushes also run the CMake build and registered CTest checks through GitHub Actions. CI is not an interactive VM; Codespaces are billed/controlled by GitHub account policy.

Using Git:

```bash
git clone <repository-url>
cd KTLT
```

If the project was supplied as a ZIP archive, extract it and open a terminal in the extracted `KTLT` directory. Keep the `data/` directory beside the source tree; the application needs its text fixtures at startup.

## Build and run on Linux

### Option A: GNU Make

Run these commands from the repository root:

```bash
make clean
make all
./quanlythuebao
```

For the normal build-and-run shortcut:

```bash
make run
```

### Option B: CMake

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/quanlythuebao
```

The program can also be launched from another working directory because it resolves `data/` relative to the executable, provided the complete data directory is present.

## Build and run on Windows

### Option A: One-click Batch script (MinGW-w64)

1. Open Command Prompt or Git Bash.
2. Navigate to the repository root.
3. Ensure `g++` is available on your `PATH`.
4. Run the script:

```bat
run_windows.bat
```

The script sets `chcp 65001` for UTF-8 Vietnamese display, compiles all source files into `quanlythuebao.exe` at the project root, and starts it with the required `data/` fixtures.

To compile manually with MinGW in Command Prompt or Git Bash:

```bash
g++ -std=c++11 -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus \
  src/main.cpp src/lib/shared/*.cpp src/lib/models/*.cpp src/lib/menus/*.cpp \
  -o quanlythuebao.exe
chcp 65001
.\quanlythuebao.exe
```

### Option B: MinGW-w64 / MSYS2 with CMake

From Command Prompt, PowerShell, or Git Bash with MinGW tools on `PATH`:

```bat
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
chcp 65001
.\build\quanlythuebao.exe
```

### Option C: Visual Studio (MSVC) or Developer PowerShell

From a **Developer PowerShell for VS** or a terminal where MSVC/CMake is available:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
.\build\Debug\quanlythuebao.exe
```

Alternatively, open the repository folder in Visual Studio, select the CMake target `quanlythuebao`, choose the `Debug` configuration, and run it. Keep the repository's `data/` directory available beside the source tree.

## Build with CMake on either platform

The CMake targets are:

- `quanlythuebao` — interactive application
- `test_runner` — direct assertion suite
- `stress_test_runner` — sanitizer stress runner

Linux:

```bash
cmake -S . -B build
cmake --build build --parallel
```

Windows with a multi-configuration generator:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
```

Useful targets:

```bash
make run       # build and run the application
make test      # build and run the direct assertion test runner
make stress    # build and run the 300-second ASan/UBSan stress test
make clean     # remove generated binaries and coverage files
```

## Run tests

### CTest (Automated Test Suite)

On Linux / macOS or Windows with single-config generators (MinGW Makefiles / Unix Makefiles):

```bash
cmake -S . -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

On Windows with Visual Studio / multi-config generators:

```powershell
cmake -S . -B build
cmake --build build --config Debug --parallel
ctest --test-dir build -C Debug --output-on-failure
```

The CTest suite contains:

1. `unit_tests` — Direct unit/model/menu tests (507 assertions)
2. `stress_test_quick` — A five-second combinatorial state transition stress test
3. `startup_from_other_directory` — Startup from a different working directory

### Direct assertion runner

On Linux / macOS:

```bash
# Via GNU Make
make test

# Or via CMake build artifact
./build/test_runner
```

On Windows:

```bat
# With MinGW / CMake
.\build\test_runner.exe

# With Visual Studio (MSVC)
.\build\Debug\test_runner.exe
```

To compile and run unit tests directly on Windows using MinGW `g++`:

```bat
g++ -std=c++11 -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus -Itests/modules tests/test_runner.cpp tests/modules/*.cpp src/lib/shared/*.cpp src/lib/models/*.cpp src/lib/menus/*.cpp -o test_runner.exe
.\test_runner.exe
```

## Package submission archive

To package a clean, deterministic submission archive `Nhóm 04.zip` matching the PTIT assignment structure (single `Nhoms4/` root without temporary files, swap artifacts, or build directories):

On Linux or Git Bash on Windows:

```bash
chmod +x scripts/package_submission.sh
./scripts/package_submission.sh
```

The script cleans transient artifacts and archives all source code, build files (`CMakeLists.txt`, `Makefile`, `run_windows.bat`), documentation, and data fixtures into `Nhóm 04.zip`.

### Debugging and Tracing with GDB

GDB (GNU Debugger) allows inspecting program state, setting breakpoints, stepping through execution, analyzing crash stack traces, and auditing the atomic persistence pipeline.

#### 1. Compile with Debug Symbols

Ensure the binaries are built with debug flags (`-g`), enabled by default in `Debug` builds:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
```

#### 2. Interactive GDB Session

Launch the interactive debugger with the main executable or test runner:

```bash
gdb ./build/quanlythuebao
```

Run the bounded stress workload and sample its process memory once per second (30 seconds by default):

```bash
make monitor
```

Set `DURATION=60` to change the stress run length. `make monitor` is an alias for `make monitor-stress`. For a live list of processes associated with this repository (working directory or executable path), use `make monitor-project`; to inspect a specific process use `make monitor-project PID=<pid>`. Host-wide RAM/swap totals are available with `make monitor-system`. Stop watchers with `Ctrl-C`. These commands inspect Linux memory statistics, not physical hardware registers. Use the interactive GDB instructions below for stopped-point register and address inspection.

Common GDB commands:

| Command | Shortcut | Description | Example |
|---|---|---|---|
| `break <location>` | `b` | Set a breakpoint at function or file:line | `b main`, `b FileIO::commitFromTmp`, `b KhachHangMenu.cpp:45` |
| `run [args]` | `r` | Start program execution | `r` |
| `next` | `n` | Step over next line of code | `n` |
| `step` | `s` | Step into function call | `s` |
| `continue` | `c` | Resume execution until next breakpoint | `c` |
| `finish` | `fin` | Run until current function returns | `fin` |
| `print <expr>` | `p` | Print variable value or evaluate expression | `p targetPath`, `p *tb`, `p hoaDonStore.size()` |
| `display <expr>` | `disp` | Automatically print expression on each step | `disp sdt` |
| `backtrace` | `bt` | Print call stack backtrace | `bt`, `bt full` |
| `frame <n>` | `f` | Switch to stack frame `<n>` | `f 2` |
| `info locals` | `i lo` | Print all local variables in current frame | `info locals` |
| `info breakpoints` | `i b` | List all active breakpoints | `info breakpoints` |
| `delete <n>` | `d` | Delete breakpoint `<n>` | `d 1` |
| `quit` | `q` | Exit GDB | `q` |

Example interactive debugging workflow:

```text
$ gdb ./build/quanlythuebao
(gdb) b KhachHangMenu::themKhachHang
Breakpoint 1 at 0x4a1230: file src/lib/menus/KhachHangMenu.cpp, line 38.
(gdb) run
[... select menu option 1 ...]
Breakpoint 1, KhachHangMenu::themKhachHang (this=0x7fffffffd980) at src/lib/menus/KhachHangMenu.cpp:38
(gdb) n
(gdb) print maKH
$1 = "KH0011"
(gdb) c
Continuing...
(gdb) quit
```

#### 3. Automated Stream Tracing (RAM tmpfs → SSD)

Run a scripted non-interactive GDB trace to audit the two-tier atomic persistence pipeline (staging in RAM `/dev/shm/ktlt_tmps` via `tmps/`, intermediate `.ssd_swap` write, and atomic `rename` onto SSD):

```bash
gdb -batch \
  -ex "file ./build/test_runner" \
  -ex "break FileIO::getTempStagingPath" \
  -ex "break FileIO::commitFromTmp" \
  -ex "run" \
  -ex "print targetFilePath" \
  -ex "continue" \
  -ex "print tmpPath" \
  -ex "print targetPath" \
  -ex "quit"
```

### Strict C++11 build

```bash
cmake -S . -B build-strict \
  -DCMAKE_CXX_FLAGS='-std=c++11 -pedantic-errors -Wall -Wextra'
cmake --build build-strict --parallel
```

## Data files and startup behavior

The application uses local pipe-delimited text files in `data/`:

```text
data/CacGoiCuoc.txt
data/khachhang.txt
data/thuebao.txt
data/hopdong.txt
data/imei.txt
data/naptien.txt
data/ghicuocgoi.txt
data/hoadon.txt
data/khieunai.txt
```

`data/bts_vietnam.txt` and `data/bts_vietnam.csv` provide the offline BTS registry dataset.

The application requires the nine primary data files at startup. If the data directory is incomplete, it prints an error and exits with status `1`; it does not fabricate a successful empty startup.

Do not put real personal data into the repository. The checked-in examples are course/demo data.

## Project layout

```text
.
├── CMakeLists.txt, Makefile     # Build targets and developer commands
├── src/
│   ├── main.cpp                 # Startup, data-store wiring, top-level menu
│   └── lib/
│       ├── menus/               # Interactive workflows; coordinate model/repository calls
│       ├── models/              # Telecom entities and domain rules
│       └── shared/              # Entity, Repository<T>, FileIO, validation, input/display helpers
├── data/                        # Text persistence files and offline BTS datasets
├── tests/
│   ├── test_runner.cpp          # Assertion-suite entry point
│   ├── modules/                 # Shared, infrastructure, model, menu, and fixture test modules
│   ├── *runner.cpp              # Stress, lifecycle, malformed-input, and deep-CRUD runners
│   └── repro/                   # Focused regression reproducers and startup scripts
├── docs/                        # Architecture, operations, user guides, reports, and audits
├── scripts/                     # Packaging and documentation/build utilities
├── workspace/                   # Session state, handoffs, and project work notes
└── mkdocs.yml                   # Documentation site configuration
```

### Runtime and ownership flow

```text
main.cpp
  → constructs repositories and menu controllers
  → menu controller reads/validates user input and coordinates an operation
  → model enforces entity/domain invariants and owns serialization
  → Repository<T> owns each in-memory collection and persistence file
  → FileIO stages and commits file changes; data/*.txt is the durable store
```

There are no separate service or per-entity repository layers: menus are the workflow coordinators, and the shared `Repository<T>` template is the storage owner. The app is a synchronous, single-process console program; it has no background worker or network service.

### Build and verification structure

- `quanlythuebao`: application executable (`src/main.cpp` plus shared production sources).
- `test_runner`: modular assertion suite (`tests/test_runner.cpp` and `tests/modules/`).
- `stress_test_runner`, `malformed_fuzzer_runner`, `crud_deep_tester`, and `lifecycle_3phase_runner`: focused stress, malformed-input, CRUD, and lifecycle executables.
- CTest registers the unit suite, a bounded stress test, and startup from a different working directory. `make monitor` runs the stress-memory demo; `make monitor-project` and `make monitor-system` are separate monitoring modes.

For the documentation map, start at [`docs/index.md`](docs/index.md). For deeper structure, see [`docs/Overview/lib.md`](docs/Overview/lib.md), [`docs/Operational/FUNCTION_CALLFLOW_SPEC.md`](docs/Operational/FUNCTION_CALLFLOW_SPEC.md), and [`docs/engineering/function_process_map.md`](docs/engineering/function_process_map.md). Those documents go deeper in their respective areas; this README is the repository-level map.

## Persistence behavior

`Repository<T>` provides:

- `loadFromFile()` and `saveToFile()`
- add, update, remove, find, filter, sort, and clear operations
- duplicate-ID rejection
- malformed-record skipping during deserialization
- rollback of in-memory mutation if saving fails

Writes are staged through the temporary area and committed by `FileIO`. On Linux, the project uses `/dev/shm` when available for temporary staging.

## Scope boundaries

The current project does not provide:

- Carrier-network or EIR integration
- Real payment processing or SMS delivery
- Enterprise authentication or role-based access control
- Multi-user transactional locking
- A complete CDR-to-invoice billing engine
- A complete global orphan-record scanner
- A complete guard preventing deletion of every referenced plan
- A dedicated statistics dashboard

These are possible future extensions, not runtime dependencies.

## Verification

Run these commands on the current checkout:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/test_runner
```

For repository-integrity checks, inspect the working tree before committing:

```bash
git status --short
git diff --check
```

## Further documentation

- `docs/UserGuide/USER_MANUAL.md`
- `docs/Overview/ARCHITECTURE_AND_REQUIREMENTS.md`
- `docs/Overview/lib.md`
- `docs/Operational/TECHNICAL_DESIGN_SPECIFICATION.md`
- `docs/Operational/FUNCTION_CALLFLOW_SPEC.md`
- `docs/Operational/USE_CASE_SPEC_DUCANH.md`
- `docs/Reports/IMEI_AND_SHARED_LIBRARIES_REPORT.md`
