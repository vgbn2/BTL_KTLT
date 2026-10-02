# Session Memory & Durable Facts

- **Target Student:** Trần Đức Anh (B24DCVT021), PTIT Khoa Viễn thông 1.
- **Assigned Modules:** `HopDong` (Use Case 1) & `ThietBiIMEI` (Use Case 2).
- **Core Algorithm:** Luhn Mod-10 algorithm validates 15-digit IMEIs before database commit.
- **Telecom Context:** EIR 3-list model (`HoatDong` = Whitelist, `KhoaMang` = Blacklist); BTS cell tower tracking (`tramBTSGanNhat`).
- **Database Engine:** Generic `Repository<T>` template managing `std::vector<T>` and atomic file persistence to `data/hopdong.txt` and `data/imei.txt`.
- **UI Policy:** Clean, professional console menus matching PTIT Section 5 prompt; strictly NO student name annotations in console output.
- **Coding Style Reference:** `coding_practice/code-c` (clean, straightforward C/C++ style, standard library first, zero external dependencies).
- **YAGNI Constraint:** No live data metering, real-time packet tracking, or socket streaming.
- **Bilingual Naming Policy:** Intentional separation between Vietnamese domain filenames (`hopdong.h/cpp`, `imei.h/cpp`, `data/hopdong.txt`, `data/imei.txt`) matching PTIT group assignment specs, standard English for core OOP architecture & algorithms (`getId`, `validateLuhn`, `isExpired`, `toFileString`, `fromFileString`, `saveToFile`), and Vietnamese for console action handlers (`themHopDong`, `xemDanhSach`, `ganSIM`).
- **Validation Rules:** Vietnamese mobile phone numbers must be exactly 10 digits starting with `03`, `05`, `07`, `08`, `09`. Subscriber birth dates must be $\ge 14$ and $\le 120$ years old, rejecting future dates. All text inputs reject the `|` pipe delimiter to prevent database corruption. Contract prices must be $\ge 0$.
- **VS Code Workspace Wiring:** Parent workspace `.vscode/tasks.json` uses Makefile-aware builds that detect `../Makefile` or `./Makefile`, compiling multi-file projects with `make` and copying binaries to `${fileDirname}/${fileBasenameNoExtension}` while preserving single-file competitive programming tasks.


