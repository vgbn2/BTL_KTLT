# C++11 Engineering Patterns & Architecture Rationale (`docs/Overview/syntax.md`)

This document outlines the core C++11 idioms, architectural patterns, type safety mechanisms, and OOP designs applied across `src/lib/`, serving as an engineering reference and oral defense guide for the 3rd-year Mobile Subscriber Management System (PTIT, 2026–2027).

---

## Table of Contents
1. [Member Initializer Lists (`: var(val)`)](#1-member-initializer-lists)
2. [The `explicit` Keyword](#2-the-explicit-keyword)
3. [Trailing `const` on Methods](#3-trailing-const-on-methods)
4. [Range-Based `for` Loops (`const auto&`)](#4-range-based-for-loops)
5. [Pass by `const` Reference (`const Type&`)](#5-pass-by-const-reference)
6. [Abstract Interfaces: `virtual`, `= 0`, and `= default`](#6-abstract-interfaces-virtual--0-and--default)
7. [The `override` Specifier](#7-the-override-specifier)
8. [Templates (`template <typename T>`)](#8-templates-generic-programming)
9. [C++ Casts (`static_cast<T>`)](#9-explicit-casting-static_cast)
10. [Operator Overloading (`operator<`, `operator<<`)](#10-operator-overloading)
11. [Namespaces and Scope Resolution (`::`)](#11-namespaces-and-scope-resolution)
12. [Stream Manipulators (`<iomanip>`)](#12-stream-manipulators-iomanip)
13. [Lambdas and `std::function`](#13-lambdas-and-stdfunction)

---

### 1. Member Initializer Lists
**Where used:** `Date.cpp`, `hopdong.cpp`, `imei.cpp`, `Entity.h`

```cpp
// Date.cpp
Date::Date() : day(1), month(1), year(2000) {}

// hopdong.cpp (calling base constructor and initializing fields)
HopDong::HopDong(...)
    : Entity(maHD), maKhachHang(maKH), soDienThoai(sdt), giaTriGoi(gia) {
    // constructor body
}
```

* **What it means:** Initializes member variables **before** entering the constructor's `{}` body.
* **Why C++ uses it:** In C++, class members are constructed before the `{}` body runs. Using an initializer list constructs them directly with their final values instead of default-constructing them first and reassigning them later.
* **Plain C equivalent:**
  ```c
  Date d;
  d.day = 1;
  d.month = 1;
  d.year = 2024;
  ```

---

### 2. The `explicit` Keyword
**Where used:** `Date.h`, `Entity.h`, `DataStore.h`, `HopDongMenu.h`

```cpp
// Date.h
explicit Date(const std::string& dateStr);

// DataStore.h
explicit DataStore(const std::string& path);
```

* **What it means:** Prevents single-argument constructors from performing **implicit type conversions** behind your back.
* **Example of what it stops:**
  ```cpp
  void printDate(Date d);

  // Without explicit: C++ silently converts the raw string into a Date!
  printDate("01/01/2024"); // Might hide logic bugs

  // With explicit: Compiler forces explicit construction
  printDate(Date("01/01/2024")); // Clear and intentional
  ```

---

### 3. Trailing `const` on Methods
**Where used:** Getters, checkers (`isExpired`, `isBlacklisted`, `getId`, `toString`)

```cpp
// Date.h
int getDay() const;
bool isFuture(const Date& relativeTo) const;
```

* **What it means:** Promises that calling this method will **never modify the object's internal fields** (`this` pointer is treated as `const Date*`).
* **Why it matters:** If you pass a `const Date& d` into a function, the compiler only allows calling methods marked with `const`. If you try to write `day = 10;` inside a `const` method, the compiler generates a build error.

---

### 4. Range-Based `for` Loops
**Where used:** `DataStore.h`, `InputHelper.cpp`, `imei.cpp`

```cpp
// DataStore.h
for (const auto& item : items) {
    outFile << item.toFileString() << "\n";
}

// imei.cpp
for (char c : imeiStr) {
    if (c < '0' || c > '9') return false;
}
```

* **Breakdown:**
  - `auto`: "Compiler, deduce the type of elements inside the container automatically."
  - `&`: "Borrow a reference to the element instead of copying the whole struct/object in memory."
  - `const`: "Read-only access — do not allow accidental modification."
* **Plain C equivalent:**
  ```c
  for (size_t i = 0; i < items_count; ++i) {
      const Item* item = &items[i];
      // read item->...
  }
  ```

---

### 5. Pass by `const` Reference
**Where used:** Throughout all function parameters taking strings or objects (`const std::string&`, `const Date&`)

```cpp
bool isExpired(const Date& currentDate) const;
std::string trim(const std::string& s);
```

* **Comparison:**
  | Syntax | Memory Behavior | Modification Allowed? |
  |---|---|---|
  | `void f(std::string s)` | Copies the whole string buffer (slow) | Yes (on the local copy) |
  | `void f(std::string* s)` | Passes pointer address (4/8 bytes) | Yes (modifies original) |
  | `void f(const std::string& s)` | Passes pointer address under the hood, clean dot `.` syntax | **No** (fast + safe) |

---

### 6. Abstract Interfaces: `virtual`, `= 0`, and `= default`
**Where used:** `Entity.h`

```cpp
class Entity {
public:
    virtual ~Entity() = default;                    // 1. Virtual destructor with default cleanup
    virtual std::string toFileString() const = 0;  // 2. Pure virtual function
};
```

1. **`virtual ~Entity() = default;`**:
   - `virtual`: Guarantees that when you delete an `Entity*` pointing to a `HopDong`, the `HopDong` destructor runs properly.
   - `= default`: Tells the compiler "generate the standard destructor body for me."
2. **`= 0` (Pure Virtual Function):**
   - Declares a method that has no implementation in the base class.
   - Forces any derived class (`HopDong`, `ThietBiIMEI`) to implement `toFileString()`, otherwise the program will not compile.

---

### 7. The `override` Specifier
**Where used:** `hopdong.h`, `imei.h`

```cpp
// hopdong.h
std::string toFileString() const override;
bool fromFileString(const std::string& line) override;
```

* **What it means:** Explicitly states: *"This function is meant to override a `virtual` function from the parent class `Entity`."*
* **Safety Benefit:** If you misspell the name (e.g. `toFilestring()`), the compiler will immediately alert you instead of silently treating it as a new, unrelated function.

---

### 8. Templates (Generic Programming)
**Where used:** `DataStore.h`

```cpp
template <typename T>
class DataStore {
private:
    std::string filePath;
    std::vector<T> items;
public:
    bool add(const T& item);
    T* findById(const std::string& id);
};
```

* **What it means:** `T` is a **placeholder type**.
* **How it works:**
  - `DataStore<HopDong>` $\rightarrow$ Compiler generates a class where `T` is replaced by `HopDong`.
  - `DataStore<ThietBiIMEI>` $\rightarrow$ Compiler generates a class where `T` is replaced by `ThietBiIMEI`.
* **Plain C equivalent:** In C, you would have to use unsafe `void*` casts or duplicate code into `HopDongStore` and `IMEIStore`.

---

### 9. Explicit Casting (`static_cast`)
**Where used:** `imei.cpp`, `Date.cpp`

```cpp
int posFromRight = static_cast<int>(REQUIRED_IMEI_LENGTH) - i;
```

* **What it means:** Converts types with compiler safety checks.
* **Why not `(int)`?** C-style cast `(int)x` can silently perform dangerous memory reinterpretation. `static_cast<int>(x)` only permits valid, well-defined numeric/type conversions.

---

### 10. Operator Overloading
**Where used:** `Date.h`, `Date.cpp`

```cpp
// Relational comparison
bool Date::operator<(const Date& other) const;

// Stream insertion (printing)
std::ostream& operator<<(std::ostream& os, const Date& d);
```

* **What it allows:** Enables natural mathematical and I/O syntax for custom objects:
  ```cpp
  if (ngayHetHan < ngayDangKy) { ... }  // Calls ngayHetHan.operator<(ngayDangKy)
  std::cout << "Ngay: " << d << "\n";   // Calls operator<<(std::cout, d)
  ```

---

### 11. Namespaces and Scope Resolution (`::`)
**Where used:** `CalendarConstants::`, `HopDongConstants::`, `FilePaths::`, `Date::parse`

```cpp
namespace FilePaths {
    const std::string HOPDONG_DATA = "data/hopdong.txt";
    const std::string IMEI_DATA    = "data/imei.txt";
}

// Accessing:
std::string path = FilePaths::HOPDONG_DATA;
```

* **What it means:** Groups related constants and functions under a named scope to prevent naming collisions in large projects.
* **The `::` symbol (Scope Resolution Operator):**
  - `Date::parse(...)`: Calls the static function `parse` defined inside class `Date`.
  - `FilePaths::HOPDONG_DATA`: Accesses the constant `HOPDONG_DATA` inside namespace `FilePaths`.
  - `std::vector`: Accesses `vector` from the Standard C++ library (`std`).

---

### 12. Stream Manipulators (`<iomanip>`)
**Where used:** `HopDong.cpp`, `imei.cpp`, `Date.cpp`

```cpp
// Formatting table headers:
std::cout << "|" << std::left << std::setw(10) << " Ma HD" << "|";

// Padding numbers with leading zeros (e.g. 05/09/2024):
oss << std::setfill('0') << std::setw(2) << day;

// Formatting floating-point prices with 0 decimals:
std::cout << std::fixed << std::setprecision(0) << giaTriGoi << " VND";
```

* **Translation to `printf` format specifiers:**
  - `std::setw(10)` $\approx$ `%10s` (field width 10)
  - `std::left` $\approx$ `%-10s` (left-aligned)
  - `std::setfill('0') << std::setw(2) << day` $\approx$ `%02d`
  - `std::fixed << std::setprecision(0)` $\approx$ `%.0f`

---

### 13. Lambdas and `std::function`
**Where used:** `DataStore.h` (`filter`, `sort`), `HopDongMenu.cpp`, `ThietBiIMEIMenu.cpp`

```cpp
// Searching for contracts with price > 100,000 using a Lambda:
auto results = store.filter([](const HopDong& hd) {
    return hd.getGiaTriGoi() > 100000;
});

// Sorting contracts by price descending:
store.sort([](const HopDong& a, const HopDong& b) {
    return a.getGiaTriGoi() > b.getGiaTriGoi();
});
```

* **What a Lambda `[](const HopDong& hd) { return ...; }` is:** An **inline, anonymous function** written directly at the call site without needing to declare a separate helper function.
  - `[]`: Capture list (empty means no local variables from outer scope are captured).
  - `(params)`: Argument list.
  - `{ body }`: Function logic returning a `bool`.
* **Plain C equivalent:** A C callback function pointer (`bool (*predicate)(const HopDong*)`).
