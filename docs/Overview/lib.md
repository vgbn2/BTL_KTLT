# Comprehensive Guide to Custom Libraries (`src/lib/`)

This document provides a technical catalog of all custom libraries developed within `src/lib/` for the **Mobile Subscriber Management System** (Topic 1 — KTLT, PTIT).

---

## 1. Library Categorization & Architecture Hierarchy

The libraries are structured into four distinct functional directories to ensure modularity, low coupling, high testability, and straightforward code inspection.

```
src/lib/
├── shared/                    # Tier 1: Core Shared Foundation & Infrastructure
│   ├── Entity.h               # Abstract Base Entity Contract (Polymorphism & Serialization)
│   ├── Exceptions.h           # Domain-Specific Exception Hierarchy (Derived from std::runtime_error)
│   ├── Date.h / Date.cpp      # Calendar Date Engine (Validation, Age Calculation, Parsing)
│   ├── fileio.h / fileio.cpp  # String Utilities & Cross-Platform Path Handling
│   ├── Normalized.h / .cpp    # Sanitization & Validation Engine (Phone & String Cleaning)
│   ├── InputHelper.h / .cpp   # Safe Console Input Extraction (Console Input Family)
│   ├── DisplayHelper.h        # Unified Terminal Table & Detail Card Layout Engine
│   └── DataStore.h            # Generic In-Memory Collection & Flat-File Persistence Engine
│
├── models/                    # Tier 2: Domain Entities & Business Logic
│   ├── hopdong.h / .cpp       # Subscription Contract Management (UC01: Pricing, Validity, Lifecycle)
│   └── imei.h / .cpp          # Terminal Device & IMEI Tracking (UC02: 3GPP Luhn Checksum, EIR Blacklist)
│
├── menus/                     # Tier 3: Console UI Controllers & Interactive Menus
│   ├── HopDongMenu.h / .cpp   # 0–9 Console Controller for Contract Operations
│   └── ThietBiIMEIMenu.h / .cpp # 0–9 Console Controller for Terminal & IMEI Tracking
│
└── stubs/                     # Tier 4: Team Extension Placeholders (Cross-Member Integration)
    ├── goicuoc.h / .cpp       # Tariff Packages & Plan Types (Nguyễn Tùng Dương - B24DCVT106)
    ├── thuebao.h / .cpp       # Customer KYC & SIM Provisioning (Nguyễn Tất Thắng - B24DCVT331)
    ├── naptien.h / .cpp       # Top-Up Transactions & CDR Raw Records (Nguyễn Mạnh Dũng - B24DCVT094)
    └── hoadon.h / .cpp        # Billing, Invoicing & Dispute Tickets (Đặng Việt Hùng - B24DCVT167)
```

---

## 2. Software Architecture & Telecom Engineering Principles

Designed for a 3rd-year Telecommunications Engineering curriculum (PTIT, 2026–2027), the system enforces rigorous software engineering standards, strict adherence to international telecom protocols, low architectural coupling, and maximum clarity for oral defense and codebase maintainability:

### 2.1. Standard Library & Deterministic Execution
- **Zero external third-party dependencies:** Implemented exclusively in standard C++11 (`<iostream>`, `<string>`, `<vector>`, `<sstream>`, `<iomanip>`, `<ctime>`, `<algorithm>`, `<fstream>`, `<stdexcept>`, `<cstdio>`), ensuring native cross-platform build stability across Linux, Windows, and macOS without dependency hell.
- **Direct algorithmic validation:** Telecom domain algorithms (such as the 3GPP TS 22.016 Luhn Mod-10 checksum) and validation routines operate directly on fundamental types with deterministic $O(N)$ complexity and zero opaque regex overhead.

### 2.2. Domain Invariants & Separation of Concerns (SoC)
- **Layered 4-tier decoupling:** Presentation Menu Controllers $\rightarrow$ Domain Models $\rightarrow$ Core Foundations $\rightarrow$ Storage / Integration Stubs. Domain entities (`HopDong`, `ThietBiIMEI`) encapsulate business invariants and are completely decoupled from terminal console I/O (`InputHelper`).
- **Clean polymorphic design:** Single-level polymorphic inheritance (`Entity` $\rightarrow$ `HopDong`, `ThietBiIMEI`) enforcing pure virtual serialization contracts (`toFileString`, `fromFileString`) and table presentation layouts without deep, brittle inheritance hierarchies.
- **Fail-Safe Exception Hierarchy:** Standardized domain error propagation derived from `std::runtime_error` (`ValidationException`, `InvalidDateException`, `InvalidPhoneNumberException`, `InvalidLuhnException`, `DuplicateIdException`, `NotFoundException`).

### 2.3. Reliable Atomic Storage & Protocol Adherence
- **Atomic persistence staging:** Generic in-memory store `DataStore<T>` stages disk persistence through temporary `.tmp` files and atomic filesystem rename (`std::rename`), ensuring zero database file corruption upon abnormal termination or power loss.
- **3GPP compliance & EIR lifecycle:** Full compliance with 3GPP TS 22.016 / 23.003 specifications for 15-digit TAC+SNR+CD hardware validation and EIR Blacklist state transitions.

---

## 3. Tier 1: Core Foundation & Infrastructure Libraries (`src/lib/shared/`)

---

### `Date.h` / `Date.cpp` — Calendar Engine & Date Logic
- **Header:** `src/lib/shared/Date.h`
- **Implementation:** `src/lib/shared/Date.cpp`
- **Purpose:** Handles date manipulation, leap year calculation, calendar validity, age computation, string parsing (`DD/MM/YYYY`), and chronological comparison.

#### Key Constants (`CalendarConstants` Namespace)
```cpp
namespace CalendarConstants {
    const int MIN_VALID_YEAR = 1900;
    const int MAX_VALID_YEAR = 2100;
    const int DEFAULT_MIN_SUBSCRIBER_AGE = 14;   // Legal minimum age for telecom subscriber in Vietnam
    const int DEFAULT_MAX_SUBSCRIBER_AGE = 120;  // Maximum plausible lifespan
    const int MONTHS_PER_YEAR = 12;
    const int MONTH_FEBRUARY = 2;
    const int DAYS_FEB_LEAP = 29;
    const int DAYS_FEB_NORMAL = 28;
    const int DAYS_LONG_MONTH = 31;              // Jan, Mar, May, Jul, Aug, Oct, Dec
    const int DAYS_SHORT_MONTH = 30;             // Apr, Jun, Sep, Nov
}
```

#### Class Definition & Methods
```cpp
class Date {
private:
    int day;
    int month;
    int year;

public:
    Date();                                      // Defaults to 01/01/2000
    Date(int d, int m, int y);                  // Validates calendar bounds
    explicit Date(const std::string& dateStr);   // Parses "DD/MM/YYYY"

    int getDay() const;
    int getMonth() const;
    int getYear() const;

    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);

    std::string toString() const;                // Formats to "DD/MM/YYYY"

    static bool isLeapYear(int y);               // (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0)
    static int daysInMonth(int m, int y);        // Returns exact days considering leap years
    static bool isValid(int d, int m, int y);    // Full range & month length verification
    static bool isValidBirthDate(const Date& d, int minAge, int maxAge, const Date& relativeTo);
    static Date parse(const std::string& str);   // Stream-based string parser
    static Date now();                           // Fetches system local date

    int calculateAge(const Date& relativeTo = Date::now()) const;
    bool isFuture(const Date& relativeTo = Date::now()) const;
    bool isPast(const Date& relativeTo = Date::now()) const;

    // Relational operators for chronological ordering
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Date& d);
    friend std::istream& operator>>(std::istream& is, Date& d);
};
```

---

### `fileio.h` / `fileio.cpp` — File & String Utility Engine
- **Header:** `src/lib/shared/fileio.h`
- **Implementation:** `src/lib/shared/fileio.cpp`
- **Purpose:** Provides reusable string manipulation (trimming, token splitting) and cross-platform directory creation.

#### Key Constants
```cpp
namespace FilePaths {
    const std::string HOPDONG_DATA = "data/hopdong.txt";
    const std::string IMEI_DATA    = "data/imei.txt";
}

namespace FileConstants {
    const char DEFAULT_DELIMITER = '|';
    const char COMMENT_PREFIX    = '#';
}
```

#### Static Utility Methods
- `std::vector<std::string> FileIO::split(const std::string& s, char delimiter)`:
  Splits a string by delimiter, trimming surrounding whitespaces from each extracted token while preserving trailing empty tokens.
- `std::string FileIO::trim(const std::string& s)`:
  Strips leading and trailing whitespace characters (`' '`, `'\t'`, `'\r'`, `'\n'`).
- `bool FileIO::ensureDirectoryExists(const std::string& filepath)`:
  Extracts parent directory path and calls `mkdir(dir, 0755)` (POSIX) or `_mkdir(dir)` (Windows) to prevent file write failures.

---

### `Normalized.h` / `Normalized.cpp` — Sanitization & Validation Engine
- **Header:** `src/lib/shared/Normalized.h`
- **Implementation:** `src/lib/shared/Normalized.cpp`
- **Purpose:** Pure functions for string sanitization, delimiter injection prevention, and strict Vietnamese 10-digit mobile phone validation. Decoupled from interactive I/O.

#### Key Functions (`Normalized` Namespace)
- `bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false)`: Validates 10-digit mobile phone number starting with `03`, `05`, `07`, `08`, `09`.
- `std::string trim(const std::string& str)`: Strips leading/trailing whitespace.
- `std::string CollapseSpace(const std::string& str)`: Collapses multiple consecutive spaces into a single space.
- `std::string removeSymbols(const std::string& str)`: Strips forbidden delimiter and control symbols.
- `std::string NormalizedName(const std::string& str)`: Standardizes Vietnamese person/customer names.
- `std::string toUpper(const std::string& str)` / `toLower(const std::string& str)`: Case transformations.

---

### `InputHelper.h` / `InputHelper.cpp` — Safe Console Input Handler (Console Input Family)
- **Header:** `src/lib/shared/InputHelper.h`
- **Implementation:** `src/lib/shared/InputHelper.cpp`
- **Purpose:** Prevents `std::cin` buffer lockups, handles invalid input types gracefully, enforces numeric ranges, sanitizes delimiter injection, and delegates phone validation to `Normalized::isValidPhoneNumber`.

#### Key Constants (`InputLimits` Namespace)
```cpp
namespace InputLimits {
    const int DEFAULT_MIN_INT = -2147483647;
    const int DEFAULT_MAX_INT = 2147483647;
    const double DEFAULT_MIN_DOUBLE = 0.0;
    const double DEFAULT_MAX_DOUBLE = 1e12;
    const size_t REQUIRED_PHONE_LENGTH = 10;
    const char PHONE_PREFIX = '0';
    const std::string UNASSIGNED_PHONE_TAG = "ChuaGan";
}
```

#### Static Methods
- `void clearBuffer()`: Clears `std::cin.fail()` state and flushes remaining newline characters.
- `std::string getString(prompt, allowEmpty)`: Reads input via `std::getline`, trims whitespace, prevents delimiter character `|` injection.
- `int getInt(prompt, minVal, maxVal)`: Extracts and validates integer within `[minVal, maxVal]`.
- `double getDouble(prompt, minVal, maxVal)`: Extracts and validates floating-point numbers.
- `Date getDate(prompt)`: Prompts until a syntactically and logically valid `Date` is entered.
- `Date getBirthDate(prompt, minAge, maxAge)`: Ensures entered date is not in the future and corresponds to age within `[minAge, maxAge]`.
- `bool isValidPhoneNumber(phone, allowUnassigned)`: Validates 10-digit mobile phone number starting with `03`, `05`, `07`, `08`, `09`.
- `std::string getPhoneNumber(prompt, allowUnassigned)`: Robust loop for phone number input.
- `bool getConfirm(prompt)`: Prompts user for `(y/n)` confirmation.
- `void pause(message)`: Halts execution until user presses Enter.

---

### `Exceptions.h` — Domain Exception System
- **Header:** `src/lib/shared/Exceptions.h`
- **Purpose:** Clean, type-safe runtime error propagation derived from `std::runtime_error`.

```cpp
class AppException : public std::runtime_error;               // Base domain exception
class DuplicateIdException : public AppException;             // Primary key violation
class NotFoundException : public AppException;                // Entity lookup miss
class InvalidDateException : public AppException;             // Date boundary or format error
class InvalidLuhnException : public AppException;             // 3GPP IMEI checksum mismatch
class InvalidPhoneNumberException : public AppException;      // Non-compliant phone prefix/length
class FileIOException : public AppException;                   // Disk read/write failure
```

---

### `Entity.h` — Abstract Base Entity
- **Header:** `src/lib/shared/Entity.h`
- **Purpose:** Defines the polymorphic interface for all domain entities.

```cpp
class Entity {
protected:
    std::string id;

public:
    Entity();
    explicit Entity(const std::string& entityId);
    virtual ~Entity() = default;

    std::string getId() const;
    void setId(const std::string& entityId);

    virtual void displayHeader() const = 0;
    virtual void displayRow() const = 0;
    virtual void displayDetail() const = 0;

    virtual std::string toFileString() const = 0;
    virtual bool fromFileString(const std::string& line) = 0;
};
```

---

### `DisplayHelper.h` — Unified Table & Detail Card Layout Engine
- **Header:** `src/lib/shared/DisplayHelper.h`
- **Purpose:** Eliminates redundant ASCII table border and padding boilerplate across models and menus. Provides reusable functions for fixed-width header rows, data rows with left/right alignment, and multi-line detail cards.

#### Key Functions (`DisplayHelper` Namespace)
- `void printBorder(const std::vector<int>& widths, std::ostream& os = std::cout)`: Generates dynamic `+---+---+` border rows matching column widths.
- `void printHeader(const std::vector<std::string>& headers, const std::vector<int>& widths, const std::vector<bool>& rightAlign = {}, std::ostream& os = std::cout)`: Renders table header with automatic uppercase/spacing and surrounding borders.
- `void printRow(const std::vector<std::string>& cells, const std::vector<int>& widths, const std::vector<bool>& rightAlign = {}, std::ostream& os = std::cout)`: Formats single entity row with column padding and optional right-alignment (e.g. currency).
- `void printCard(const std::vector<std::pair<std::string, std::string>>& fields, int labelWidth = 16, std::ostream& os = std::cout)`: Prints multi-line key-value detailed view for single-record inspection.

---

### `DataStore.h` — Generic In-Memory & File Persistence Store
- **Header:** `src/lib/shared/DataStore.h`
- **Purpose:** Provides generic CRUD operations, linear searching, predicate filtering, lambda sorting, and atomic file saving (write temp file + rename) for any entity derived from `Entity`.

#### Key Template Methods (`DataStore<T>`)
- `bool loadFromFile()`: Reads all non-comment lines from disk and deserializes into `std::vector<T>`.
- `bool saveToFile() const`: Atomically writes records to `filePath.tmp` and renames to `filePath`.
- `bool add(const T& item)`: Inserts new record; throws `DuplicateIdException` if ID already exists.
- `bool update(const std::string& id, const T& updatedItem)`: Updates existing record by ID; throws `NotFoundException` if absent.
- `bool remove(const std::string& id)`: Deletes record by ID; throws `NotFoundException` if absent.
- `T* findById(const std::string& id)`: Linear scan returning pointer to matching record or `nullptr`.
- `std::vector<T> filter(std::function<bool(const T&)> predicate) const`: Returns subset of matching records.
- `void sort(std::function<bool(const T&, const T&)> comparator)`: Sorts in-place via `std::sort`.

---

## 4. Tier 2: Domain Entities & Business Logic (`src/lib/models/`)

---

### `hopdong.h` / `hopdong.cpp` — Subscription Contract (UC01)
- **Header:** `src/lib/models/hopdong.h`
- **Implementation:** `src/lib/models/hopdong.cpp`
- **Responsible Student:** Trần Đức Anh (B24DCVT021)
- **Purpose:** Encapsulates subscriber service contracts, package types (`TraTruoc`, `TraSau`), contract lifecycle states (`HieuLuc`, `TamDung`, `ThanhLy`), contract extension (`giaHan`), and pricing validation.

#### Key Constants (`HopDongConstants` Namespace)
```cpp
namespace HopDongConstants {
    const std::string STATUS_ACTIVE     = "HieuLuc";
    const std::string STATUS_SUSPENDED  = "TamDung";
    const std::string STATUS_TERMINATED = "ThanhLy";

    const std::string TYPE_PREPAID      = "TraTruoc";
    const std::string TYPE_POSTPAID     = "TraSau";
}
```

#### Data Members
| Member Variable | Type | Description |
|---|---|---|
| `id` (inherited) | `std::string` | Primary Key — Contract Code (`HDxxxx`) |
| `maKhachHang` | `std::string` | Customer Identifier (`KHxxxx`) |
| `soDienThoai` | `std::string` | Bound Mobile Number (10 digits) |
| `maGoiCuoc` | `std::string` | Tariff Package Code (`GCxxxx`) |
| `ngayDangKy` | `Date` | Registration / Effective Date |
| `ngayHetHan` | `Date` | Expiration Date ($\ge$ `ngayDangKy`) |
| `loaiHopDong` | `std::string` | `TraTruoc` or `TraSau` |
| `trangThai` | `std::string` | `HieuLuc`, `TamDung`, or `ThanhLy` |
| `giaTriGoi` | `double` | Package Price in VND ($\ge 0.0$) |

#### Core Business Methods
- `bool isExpired(const Date& currentDate) const`: Evaluates if `currentDate > ngayHetHan`.
- `void giaHan(const Date& ngayHetHanMoi)`: Extends expiration date and resets state to `HieuLuc`.
- `void chamDut()`: Transitions state to `ThanhLy` (Terminated).
- `void tamDung()`: Transitions state to `TamDung` (Suspended).
- `void kichHoatLai()`: Reactivates contract to `HieuLuc`.

---

### `imei.h` / `imei.cpp` — Terminal Device & IMEI Tracking (UC02)
- **Header:** `src/lib/models/imei.h`
- **Implementation:** `src/lib/models/imei.cpp`
- **Responsible Student:** Trần Đức Anh (B24DCVT021)
- **Purpose:** Tracks terminal hardware (IMEI), validates 15-digit 3GPP **Luhn Mod-10 Checksum**, manages EIR (Equipment Identity Register) Blacklist status (`KhoaMang`), pairs/unpairs SIM cards, and logs BTS cell tower locations.

#### Key Constants (`IMEIConstants` Namespace)
```cpp
namespace IMEIConstants {
    const size_t REQUIRED_IMEI_LENGTH        = 15;
    const int MODULO_BASE                    = 10;
    const int LUHN_MULTIPLIER                = 2;
    const int LUHN_DIGIT_OVERFLOW_SUBTRACT   = 9;
    const int TAC_LENGTH                     = 8;  // Type Allocation Code length

    const std::string STATUS_ACTIVE          = "HoatDong";
    const std::string STATUS_LOCKED          = "KhoaMang";   // Blacklisted device
    const std::string STATUS_SUSPENDED       = "TamKhoa";
    const std::string UNASSIGNED_PHONE       = "ChuaGan";
}
```

#### 3GPP Luhn Checksum Algorithm (`validateLuhn`)
The 15-digit IMEI verification follows standard 3GPP TS 23.003 specifications:
1. Verify that string length is exactly 15 and contains only ASCII digits `'0'`–`'9'` (rejecting all zeros).
2. Iterate from the rightmost digit (index 14 down to 0).
3. For every **even position from the right** (1-indexed from right: 2nd, 4th, 6th, etc.), double the digit ($d \times 2$).
4. If $d \times 2 > 9$, subtract 9 (sum of digits of the product).
5. Accumulate all processed digits into `sum`.
6. Return `true` if and only if `(sum % 10 == 0)`.

```cpp
bool ThietBiIMEI::validateLuhn(const std::string& imeiStr) {
    if (imeiStr.length() != REQUIRED_IMEI_LENGTH) return false;
    bool allZeros = true;
    for (char c : imeiStr) {
        if (c < '0' || c > '9') return false;
        if (c != '0') allZeros = false;
    }
    if (allZeros) return false;

    int sum = 0;
    for (int i = static_cast<int>(REQUIRED_IMEI_LENGTH) - 1; i >= 0; --i) {
        int d = imeiStr[i] - '0';
        int posFromRight = static_cast<int>(REQUIRED_IMEI_LENGTH) - i;
        if (posFromRight % 2 == 0) {
            d *= LUHN_MULTIPLIER;
            if (d > 9) d -= LUHN_DIGIT_OVERFLOW_SUBTRACT;
        }
        sum += d;
    }
    return (sum % MODULO_BASE == 0);
}
```

#### Core Business Methods
- `bool isBlacklisted() const`: Returns `true` if `trangThai == "KhoaMang"`.
- `void setBlacklist(bool lock)`: Toggles status between `KhoaMang` and `HoatDong`.
- `void ganSIM(const std::string& sdt)`: Binds a valid 10-digit mobile phone number to the hardware IMEI.
- `void goSIM()`: Unbinds SIM and resets phone number to `"ChuaGan"`.
- `void capNhatBTS(const std::string& bts)`: Updates latest BTS base station tracking ID.

---

## 5. Tier 3: Console UI Controllers & Interactive Menus (`src/lib/menus/`)

---

### `HopDongMenu.h` / `HopDongMenu.cpp` — Contract Controller
- **Header:** `src/lib/menus/HopDongMenu.h`
- **Implementation:** `src/lib/menus/HopDongMenu.cpp`
- **Role:** Interactive 0–9 console interface managing UC01 operations:
  - `[1]` Thêm mới hợp đồng đăng ký (Add Contract)
  - `[2]` Xem danh sách hợp đồng (List Contracts)
  - `[3]` Tìm kiếm hợp đồng (Search by ID, Phone, Customer Code)
  - `[4]` Sắp xếp danh sách (Sort by Registration Date, Price Descending)
  - `[5]` Cập nhật hợp đồng (Update Package, Extend Expiry, Change Status)
  - `[6]` Xóa hợp đồng (Delete Contract with confirmation)
  - `[0]` Quay lại menu chính (Return)

---

### `ThietBiIMEIMenu.h` / `ThietBiIMEIMenu.cpp` — Terminal Controller
- **Header:** `src/lib/menus/ThietBiIMEIMenu.h`
- **Implementation:** `src/lib/menus/ThietBiIMEIMenu.cpp`
- **Role:** Interactive 0–9 console interface managing UC02 operations:
  - `[1]` Ghi nhận thiết bị IMEI mới (Register Hardware with Luhn check)
  - `[2]` Xem danh sách thiết bị đầu cuối (List Devices)
  - `[3]` Tìm kiếm thiết bị (Search by IMEI, Phone, Brand)
  - `[4]` Danh sách thiết bị bị khóa mạng / Blacklist (View EIR Blacklist)
  - `[5]` Cập nhật thiết bị (Pair SIM, Update BTS Location, Lock/Unlock Network)
  - `[6]` Xóa thiết bị khỏi hệ thống (Delete Device with confirmation)
  - `[0]` Quay lại menu chính (Return)

---

## 6. Tier 4: Team Extension Placeholders (`src/lib/stubs/`)

These header and stub files define integration contracts for the remaining group members:

| Library Header | Module & Use Case | Responsible Member | Key Features |
|---|---|---|---|
| `stubs/goicuoc.h` / `.cpp` | Gói cước & Loại hình thuê bao | Nguyễn Tùng Dương (B24DCVT106) | Data/Voice/SMS packages, prepaid/postpaid conversion |
| `stubs/thuebao.h` / `.cpp` | Khách hàng & SIM Số | Nguyễn Tất Thắng (B24DCVT331) | CCCD/eKYC identification, MSISDN inventory, lock/unlock line |
| `stubs/naptien.h` / `.cpp` | Nạp tiền & Bản ghi cước (CDR) | Nguyễn Mạnh Dũng (B24DCVT094) | Scratch card top-up, e-wallet, raw CDR call/data parsing |
| `stubs/hoadon.h` / `.cpp` | Hóa đơn & Khiếu nại cước | Đặng Việt Hùng (B24DCVT167) | Monthly invoice aggregation, discount calculation, dispute tickets |

---

## 7. Function Catalog & Complexity Reference Table

| Function / Method | Location | Input Parameters | Return Type | Complexity | Reverse-Eng. Difficulty |
|---|---|---|---|---|---|
| `Date::isLeapYear` | `shared/Date.cpp` | `int y` | `bool` | $O(1)$ | Very Easy (Standard arithmetic) |
| `Date::daysInMonth` | `shared/Date.cpp` | `int m, int y` | `int` | $O(1)$ | Very Easy (Switch table) |
| `Date::isValid` | `shared/Date.cpp` | `int d, int m, int y` | `bool` | $O(1)$ | Very Easy (Range checking) |
| `Date::calculateAge` | `shared/Date.cpp` | `const Date& relativeTo` | `int` | $O(1)$ | Very Easy (Year & Month delta) |
| `Date::parse` | `shared/Date.cpp` | `const std::string& str` | `Date` | $O(1)$ | Very Easy (Stream extraction) |
| `FileIO::trim` | `shared/fileio.cpp` | `const std::string& s` | `std::string` | $O(N)$ | Very Easy (`find_first_not_of`) |
| `FileIO::split` | `shared/fileio.cpp` | `const std::string& s, char del`| `vector<string>` | $O(N)$ | Easy (`getline` token stream) |
| `Normalized::isValidPhoneNumber` | `shared/Normalized.cpp` | `const std::string& phone` | `bool` | $O(1)$ | Very Easy (Prefix & length check) |
| `InputHelper::isValidPhoneNumber` | `shared/InputHelper.cpp` | `const std::string& phone` | `bool` | $O(1)$ | Very Easy (Delegates to Normalized) |
| `ThietBiIMEI::validateLuhn` | `models/imei.cpp` | `const std::string& imeiStr` | `bool` | $O(1)$ | Easy (15-step Mod-10 loop) |
| `DataStore::loadFromFile` | `shared/DataStore.h` | None | `bool` | $O(N)$ | Easy (File stream line reader) |
| `DataStore::saveToFile` | `shared/DataStore.h` | None | `bool` | $O(N)$ | Easy (Temp file + rename) |
| `DataStore::findById` | `shared/DataStore.h` | `const std::string& id` | `T*` | $O(N)$ | Very Easy (Linear loop) |
| `DataStore::filter` | `shared/DataStore.h` | `std::function<bool(const T&)>`| `vector<T>` | $O(N)$ | Easy (Predicate iteration) |
| `DataStore::sort` | `shared/DataStore.h` | `Comparator` | `void` | $O(N \log N)$ | Standard (`std::sort`) |
