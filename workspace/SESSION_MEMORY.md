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

