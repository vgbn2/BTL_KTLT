# Code Explanation Notes

## Purpose

This note explains the small interactions that protect the text-file boundary and console-input boundary in the KTLT subscriber-management system. It describes the current implementation; it does not introduce behavior changes.

The canonical path is:

```text
console or persisted line
    -> InputHelper / Repository<T>
    -> Normalized::trim or FileIO::split
    -> model fromFileString()
    -> Date::parse and domain validation
    -> in-memory object
    -> Repository<T>::saveToFile()
```

The shared helpers clean boundaries. The model parser owns the schema and domain invariants. `Repository<T>` owns collection loading, rollback-safe mutation, and persistence.

## 1. Console menu ordering

Top-level menus render their numeric choices in ascending order. The numeric constants and switch dispatch are independent of the rendered order, so an out-of-order display is a presentation defect rather than a routing defect. The current menus keep sort at its assigned numeric position and cover the ordering with captured-output assertions in `tests/test_runner.cpp`.

## 2. Console input: one line, then one boundary check

`InputHelper::getString()` reads a complete line with `std::getline`. The small interaction matters:

1. The prompt is printed.
2. `std::getline` consumes through the newline, so a previous numeric extraction cannot leave a newline to be mistaken for an empty answer.
3. `Normalized::trim()` removes input-edge whitespace.
4. Empty input is rejected unless `allowEmpty` is true.
5. The pipe character is rejected because `|` is the flat-file field delimiter.
6. Only then does the value return to the menu.

```text
user text -> getline -> trim -> empty check -> '|' check -> menu
```

This is a trust-boundary check, not cosmetic formatting. A value containing `|` could change the number of fields written by `toFileString()` and make a later `fromFileString()` parse a different record shape.

`InputHelper::getInt()` and `getDouble()` intentionally parse the whole trimmed string. `std::stoi`/`std::stod` return an index; accepting the value only when that index reaches `s.length()` rejects input such as `149000abc` instead of silently accepting the numeric prefix.

`InputHelper::getDate()` delegates the actual grammar and calendar check to `Date::parse()`. The helper owns retrying and user feedback; `Date` owns whether the value is a valid date.

## 2. `trim`: remove only line-boundary noise

`Normalized::trim()` searches the first and last characters not in:

```text
" \\t\\r\\n"
```

That set means ordinary space, horizontal tab, carriage return, and line feed. These characters commonly appear at the edge of terminal input or at the end of a Windows-style file line. Removing them prevents a value such as `KH0001\\r` from failing an equality or identifier comparison merely because of the line ending.

The empty-string branch is deliberate:

```text
all characters are whitespace -> find_first_not_of() == npos -> return ""
```

It avoids calculating a substring range from an absent first character.

`FileIO::split()` applies `trim()` to every token, not only to the complete line. Therefore this persisted row:

```text
KH0001 | Nguyen Van A | CCCD | 079085012345
```

produces tokens without the spaces around the delimiters. A final delimiter is preserved as one final empty token so that a trailing empty field is visible to the model's field-count and validation logic.

## 3. `CollapseSpace`: canonical internal spacing

`Normalized::CollapseSpace()` first calls `trim()`, then scans the remaining characters. `std::isspace()` identifies a run of whitespace; the first character in the run becomes one ordinary space, and the rest are suppressed.

```text
"  Nguyen\\tVan   A  " -> "Nguyen Van A"
```

The helper uses `std::isspace(static_cast<unsigned char>(c))`. The cast matters because the C library classification functions are only defined for `EOF` or values representable as `unsigned char`; passing a negative signed `char` would be undefined for non-ASCII bytes.

The boundary is intentionally slightly different from `trim()`: `trim()` lists the file/input edge characters explicitly, while `CollapseSpace()` treats all `std::isspace()` characters inside a name as separators.

## 4. Symbols, names, and identifier normalization

`removeSymbols()` replaces the configured forbidden characters—including `|`, `#`, and punctuation that can interfere with stored text—with spaces, then calls `CollapseSpace()`. Replacing with a space rather than deleting prevents two adjacent words from being joined accidentally.

`NormalizedName()` demonstrates the micro-interaction between normalization and `std::istringstream`:

1. `removeSymbols()` produces a safe, consistently spaced string.
2. `std::istringstream iss(...)` gives the string stream extraction semantics without touching `std::cin`.
3. `while (iss >> word)` skips repeated whitespace and extracts one word at a time.
4. Each word is lowercased, then its first byte is uppercased.
5. Words are joined with one ordinary space.

```text
raw name -> remove symbols -> string stream -> word extraction -> case normalization -> one-space name
```

The stream is local to the function. It is not a second console input channel and does not modify the process-wide input stream.

`NormalizedId()` uppercases and trims an identifier, then keeps its digits when digits are present. `NormalizedCustomerId()` is intentionally narrower: it trims and uppercases the complete customer ID so the `KH` prefix remains attached to its numeric suffix. Menu and foreign-key paths use the latter where customer IDs are compared.

## 5. Date parsing: grammar before calendar validity

`Date::parse()` uses a local `std::istringstream` to extract:

```text
integer day, separator, integer month, separator, integer year
```

The function accepts either `/` or `-`, but requires both separators to be the same. It also tries one more extraction into `trailing`; if any extra token exists, the parse fails.

This creates two layers of rejection:

```text
text grammar: 31/02/2024x -> rejected because trailing text remains
calendar rule: 31/02/2024 -> rejected by Date(d, m, y) / isValid()
```

`Date::isValid()` checks the year range, month range, and the month-specific maximum day. February delegates to `isLeapYear()`, so `29/02/2024` and `29/02/2023` take different paths.

`InputHelper::getDate()` catches `InvalidDateException`, prints the reason, and retries. `Date::parse()` itself has no console side effect, which lets model deserialization and tests use the same grammar.

## 6. Persisted lines: delimiter split before model validation

The flat-file schema uses `|` as the delimiter. The load interaction is:

```text
getline(file, line)
    -> Normalized::trim(line)
    -> skip empty/comment line
    -> T::fromFileString(trimmedLine)
    -> model calls FileIO::split(line, '|')
    -> field-count check
    -> numeric/date/domain checks
    -> assign parsed object only on success
```

`Repository<T>::loadFromFile()` keeps a temporary `loadedItems` vector. A valid parsed object is appended there. Only after the complete file has been read successfully does the repository swap that vector into `items`. A bad stream leaves the existing in-memory collection unchanged.

Malformed records are currently skipped when `fromFileString()` returns `false`; the repository returns `false` for an unopened file or a bad stream, but does not expose a per-line diagnostic. This is documented behavior and a future observability/audit candidate, not a change made by this note.

## 7. Model deserialization: strict shape, then domain constructor

Most model parsers follow the safer two-phase shape:

```cpp
std::vector<std::string> tokens = FileIO::split(line, '|');
if (tokens.size() != EXPECTED_FIELD_COUNT) return false;

try {
    Model parsed(/* tokens converted and validated */);
    *this = parsed;
    return true;
} catch (...) {
    return false;
}
```

The temporary `parsed` object is important. Conversion and constructor validation happen before `*this` is replaced, so a malformed line does not leave a partially assigned object behind.

Examples currently using this pattern include `KhachHang`, `HopDong`, `ThietBiIMEI`, `ThueBao`, `NapTien`, and `HoaDon`. Their constructors then enforce domain rules such as valid phone numbers, valid dates, non-negative amounts, valid states, or a valid IMEI Luhn checksum.

The numeric parsers use the same whole-token rule as console input:

```cpp
std::stod(tokens[i], &index);
if (index != tokens[i].length()) return false;
```

That prevents a field such as `1000xyz` from being accepted as `1000`.

`KhieuNai::fromFileString()` is a different current path: it accepts `tokens.size() < 6` as invalid, assigns fields directly, and checks only that the ID and phone are non-empty. This difference is retained here as an explicit audit point; it is not silently normalized by documentation.

## 8. IMEI records: date and Luhn validation are nested boundaries

`ThietBiIMEI::fromFileString()` requires seven fields. It passes the date field through `Date::parse()` and passes the IMEI ID through the constructor, which calls `validateLuhn()` and then `Normalized::isValidLuhn()`.

```text
seven fields
    -> Date::parse(tokens[4])
    -> constructor
    -> Luhn validation of 15 ASCII digits
    -> phone validation unless ChuaGan
    -> status/BTS validation
    -> object assignment
```

`ChuaGan` is a domain sentinel for an unassigned phone, not a valid phone number. The phone validator therefore receives `allowUnassigned = true` at the device boundary.

The IMEI schema remains seven fields:

```text
imei|tenThietBi|hangSanXuat|soDienThoai|ngayKichHoat|trangThai|tramBTSGanNhat
```

A generated simulated IMEI can satisfy the same syntactic and Luhn boundary while remaining demo data; checksum validity does not prove that the value belongs to a real physical device.

## 9. Atomic persistence: temporary file before replacement

`Repository<T>::saveToFile()` writes every `toFileString()` result to the temporary path returned by `FileIO::getTempStagingPath()`. It checks opening, flushing, and closing before asking `FileIO::commitFromTmp()` to replace the target.

The normal Linux path is:

```text
tmps/<name>.tmp -> rename() -> data/<name>
```

When a direct rename cannot cross from RAM-backed `/dev/shm` to the target filesystem, `commitFromTmp()` copies through a target-side `.ssd_swap` file and renames that intermediate file into place. Failed copies remove the intermediate file and report failure.

Repository mutations keep an in-memory rollback copy:

- `add()` removes the appended item when save fails.
- `update()` restores the previous object.
- `remove()` reinserts the erased object at its original index.
- `sortAndSave()` restores the previous ordering.
- `clear()` restores the previous collection.

These rollback paths protect the process state; the file-commit path protects the persisted state. They are separate layers and should be tested separately.

## 10. Current audit boundaries and deferred work

The following are intentionally recorded rather than claimed as fixed:

- `Repository<T>::loadFromFile()` rejects invalid rows and keeps the current collection unchanged; it still does not report a row number or field-level diagnostic.
- `KhieuNai::fromFileString()` does not currently use the constructor-validation pattern used by several other models.
- `ThueBao::setMaKhachHang()` and related setters should remain part of the cross-module foreign-key audit because the constructor and setter validation boundaries are not identical.
- `ThueBaoMenu::xoaThueBao()` rejects deletion when an IMEI remains assigned to the subscriber phone; automatic unassignment remains intentionally out of scope.
- The protected dirty files `src/lib/models/imei.h` and `src/lib/shared/BTSRegister.h` must be inspected before any source-comment pass and must not be normalized or overwritten accidentally.

## Source map

| Concern | Canonical source | Current responsibility |
|---|---|---|
| Input line and retry loop | `src/lib/shared/InputHelper.cpp` | Console extraction, retry, user-facing validation |
| Whitespace and string cleanup | `src/lib/shared/Normalized.cpp` | Pure trimming, spacing, symbol/case normalization |
| Date grammar and calendar rules | `src/lib/shared/Date.cpp` | Parse, validate, compare, format |
| Delimited fields and paths | `src/lib/shared/fileio.cpp` | Split, trim tokens, staging paths, commit |
| Collection load/save and rollback | `src/lib/shared/Repository.h` | In-memory ownership and persistence orchestration |
| Record schema and domain checks | `src/lib/models/*.cpp` | `toFileString()`, `fromFileString()`, constructors |
| Interactive orchestration | `src/lib/menus/*.cpp` | Prompts, lookups, foreign-key choices, mutations |
| Regression evidence | `tests/test_runner.cpp`, `tests/repro/*` | Real parser, lookup, persistence, and menu checks |
