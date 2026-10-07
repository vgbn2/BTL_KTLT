# Đặc Tả Tương Tác Chi Tiết Giữa Các Hàm
## Dự án: Quản Lý Thuê Bao Di Động - PTIT KTLT
### Phụ trách: Trần Đức Anh - B24DCVT021 | Phân hệ: Hợp Đồng & Thiết Bị IMEI

---

> [Thông tin] Công cụ xem sơ đồ tương tác đa màn hình: [INTERACTIVE_DIAGRAMS.html](../Display/INTERACTIVE_DIAGRAMS.html).

---

## 1. Tổng Quan Kiến Trúc Gọi Hàm

Tài liệu này mô tả chi tiết luồng gọi hàm từ cấp độ điều khiển chương trình (`main`), qua các menu tương tác (`HopDongMenu`, `ThietBiIMEIMenu`), đến các hàm tiện ích nhập liệu dòng lệnh (`InputHelper`), bộ tiện ích chuẩn hóa & xác thực (`Normalized`), động cơ hiển thị ASCII (`DisplayHelper`), động cơ lịch (`Date`), các lớp thực thể nghiệp vụ (`HopDong`, `ThietBiIMEI`), và tầng lưu trữ tệp tin nguyên tử (`Repository<T>`, `FileIO`).

```mermaid
flowchart TD
    Main["main [src/main.cpp]"]

    subgraph MenuLayer ["1. TANG MENU & DIEU HUONG NGHIEP VU"]
        direction TB
        subgraph HD_MenuBox ["Phan He Hop Dong"]
            direction TB
            HD_Show["HopDongMenu::showMenu"]
            HD_Actions["themHopDong | xemDanhSach<br/>timKiemHopDong | sapXepDanhSach<br/>capNhatHopDong | xoaHopDong"]
            HD_Show --> HD_Actions
        end
        subgraph IMEI_MenuBox ["Phan He Thiet Bi IMEI"]
            direction TB
            IMEI_Show["ThietBiIMEIMenu::showMenu"]
            IMEI_Actions["themThietBi | xemDanhSach<br/>timKiemThietBi | danhSachKhoaMang<br/>capNhatThietBi | xoaThietBi"]
            IMEI_Show --> IMEI_Actions
        end
        HD_MenuBox --> IMEI_MenuBox
    end

    subgraph HelperLayer ["2. TANG TIEN ICH, CHUAN HOA & GIAO DIEN"]
        direction TB
        InputHelpGroup["InputHelper (Console Input Family)<br/><small>getString | getInt | getDouble | getDate | pause</small>"]
        NormGroup["Normalized (Sanitization & Validation Family)<br/><small>isValidPhoneNumber | trim | CollapseSpace | toUpper</small>"]
        DisplayGroup["DisplayHelper (Presentation & ASCII Layout)<br/><small>printBorder | printHeader | printRow | printCard</small>"]
        DateEngineGroup["Date (Calendar Date Engine)<br/><small>parse | isValid | isLeapYear | calculateAge</small>"]
        FileIOGroup["FileIO (String & Disk IO)<br/><small>trim | split | ensureDirectoryExists</small>"]
        InputHelpGroup --> NormGroup
        NormGroup --> DateEngineGroup
        DisplayGroup --> FileIOGroup
    end

    subgraph DomainLayer ["3. TANG MO HINH NGHIEP VU (DECOUPLED TU INPUTHELPER)"]
        direction TB
        HD_ModelBox["HopDong Model<br/><small>HopDong | giaHan | isExpired | chamDut</small>"]
        IMEI_ModelBox["ThietBiIMEI Model<br/><small>ThietBiIMEI | validateLuhn | ganSIM | setBlacklist</small>"]
        HD_ModelBox --> IMEI_ModelBox
    end

    subgraph StorageLayer ["4. TANG LUU TRU FILE NGUYEN TU"]
        direction TB
        RepoOps["Repository&lt;T&gt;::add | update | remove<br/>Repository&lt;T&gt;::findById | filter | sort"]
        RepoSync["Repository&lt;T&gt;::saveToFile & loadFromFile<br/><small>Ghi file .tmp -> doi ten nguyen tu</small>"]
        DataFilesGroup[("data/hopdong.txt<br/>data/imei.txt")]
        RepoOps --> RepoSync --> DataFilesGroup
    end

    Main --> MenuLayer
    MenuLayer -->|Console Input| InputHelpGroup
    InputHelpGroup -->|Xac thuc logic| NormGroup
    DomainLayer -->|Validation Invariants| NormGroup
    DomainLayer -->|Uy nhiem format| DisplayGroup
    MenuLayer --> DomainLayer
    DomainLayer --> StorageLayer
    Main -.->|Exit & Sync| StorageLayer
```

### 1.1 Nguyên Tắc Tách Rời Phân Hệ (Decoupling Architecture)
* **Phân định họ hàm nghiêm ngặt:**
  - **`InputHelper` (Console Input Family):** Chuyên trách tương tác bàn phím, điều phối luồng `std::cin`, xóa bộ đệm lỗi (`cin.clear()`, `cin.ignore()`), kiểm tra giới hạn min/max, và xử lý vòng lặp nhập lại trên Console.
  - **`Normalized::isValidPhoneNumber` (Sanitization & Validation Family):** Là hàm thuần túy (pure function) kiểm tra định dạng và đầu số viễn thông Việt Nam (`03`, `05`, `07`, `08`, `09`), hoàn toàn độc lập với console hoặc bất kỳ môi trường I/O nào.
* **Tách rời Domain Models (`HopDong`, `ThietBiIMEI`) khỏi `InputHelper`:**
  - Các lớp mô hình nghiệp vụ (`HopDong`, `ThietBiIMEI`) thuộc tầng Domain Layer **tuyệt đối không phụ thuộc vào `InputHelper`**.
  - Khi cần thẩm định tính hợp lệ của số điện thoại, mô hình gọi trực tiếp `Normalized::isValidPhoneNumber`.
  - Khi cần hiển thị dữ liệu bảng hoặc thẻ chi tiết, mô hình ủy nhiệm cho `DisplayHelper`.
  - Sự tách rời này đảm bảo các lớp mô hình có thể nạp từ file qua `Repository<T>`, chạy hàng loạt test tự động trong `test_runner.cpp`, hoặc mở rộng giao diện đồ họa/API sau này mà không bị kéo theo mã nguồn console.

---

## 2. Chu Trình Sống & Nạp Dữ Liệu Khởi Động

### 2.1 Sơ Đồ Tuần Tự Gọi Hàm Khi Khởi Động Ứng Dụng
```mermaid
sequenceDiagram
    autonumber
    participant Main as main
    participant RepoHD as Repository<HopDong>
    participant RepoIMEI as Repository<ThietBiIMEI>
    participant FileIO as FileIO
    participant HD as HopDong
    participant DateMod as Date

    Main->>RepoHD: loadFromFile()
    activate RepoHD
    RepoHD->>FileIO: ensureDirectoryExists("data/hopdong.txt")
    loop Đọc từng dòng
        RepoHD->>FileIO: trim(line)
        FileIO-->>RepoHD: trimmedLine
        opt Bỏ qua comment hoặc dòng trống
            RepoHD->>RepoHD: continue
        end
        RepoHD->>HD: fromFileString(trimmedLine)
        activate HD
        HD->>FileIO: split(trimmedLine, '|')
        FileIO-->>HD: tokens
        HD->>DateMod: parse(tokens[4])
        DateMod-->>HD: Date
        HD->>DateMod: parse(tokens[5])
        DateMod-->>HD: Date
        HD-->>RepoHD: return true
        deactivate HD
        RepoHD->>RepoHD: items.push_back(hd)
    end
    RepoHD-->>Main: return true
    deactivate RepoHD

    Main->>RepoIMEI: loadFromFile()
    activate RepoIMEI
    RepoIMEI->>FileIO: ensureDirectoryExists("data/imei.txt")
    RepoIMEI-->>Main: return true
    deactivate RepoIMEI
```

---

## 3. Luồng Gọi Hàm: UC01 - Thêm Mới Hợp Đồng Đăng Ký

### 3.1 Sơ Đồ Tuần Tự Gọi Hàm Chi Tiết
```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as HopDongMenu::themHopDong
    participant Input as InputHelper
    participant Norm as Normalized
    participant DateMod as Date
    participant Repo as Repository<HopDong>
    participant Model as HopDong::HopDong
    participant Disk as File System

    User->>Menu: Chọn 1 Thêm hợp đồng mới
    activate Menu

    loop Kiểm tra trùng lặp mã hợp đồng
        Menu->>Input: getString("Nhap Ma hop dong", false)
        Input-->>Menu: maHD = "HD0011"
        Menu->>Repo: findById("HD0011")
        Repo-->>Menu: nullptr
    end

    Menu->>Input: getString("Nhap Ma khach hang", false)
    Input-->>Menu: maKH = "KH0001"

    Menu->>Input: getPhoneNumber("Nhap So dien thoai", false)
    activate Input
    Input->>Norm: isValidPhoneNumber(sdt, false)
    Norm-->>Input: true
    Input-->>Menu: sdt = "0981234567"
    deactivate Input

    Menu->>Input: getString("Nhap Ma goi cuoc", false)
    Input-->>Menu: maGC = "VD149"

    Menu->>Input: getDate("Nhap Ngay dang ky")
    Input->>DateMod: parse(str)
    DateMod->>DateMod: isValid(d, m, y)
    DateMod-->>Input: Date
    Input-->>Menu: ngayDK

    loop Kiểm tra ngày hết hạn
        Menu->>Input: getDate("Nhap Ngay het han")
        Input-->>Menu: ngayHH
    end

    Menu->>Input: getInt("Lua chon Loai hop dong", 1, 2)
    Input-->>Menu: loaiChoice = 1

    Menu->>Input: getDouble("Nhap Gia tri goi cuoc", 0.0)
    Input-->>Menu: gia = 149000.0

    Note over Model,Norm: HopDong duoc tach roi khoi InputHelper, goi truc tiep Normalized de kiem tra bat bien
    Menu->>Model: HopDong("HD0011", "KH001", "0981234567", "VD149", ngayDK, ngayHH, "TraTruoc", "HieuLuc", 149000.0)
    activate Model
    Model->>Norm: isValidPhoneNumber(sdt, false)
    Norm-->>Model: true
    Model-->>Menu: hd
    deactivate Model

    Menu->>Repo: add(hd)
    activate Repo
    Repo->>Repo: findById(hd.getId())
    Repo->>Repo: items.push_back(hd)
    Repo->>Repo: saveToFile()
    activate Repo
    Repo->>Disk: std::rename(tempPath, filePath)
    Disk-->>Repo: 0
    Repo-->>Repo: return true
    deactivate Repo
    Repo-->>Menu: return true
    deactivate Repo

    Menu-->>User: "[THANH CONG] Da them hop dong HD0011 vao he thong!"
    Menu->>Input: pause()
    deactivate Menu
```

---

## 4. Luồng Gọi Hàm: UC02 - Ghi Nhận Thiết Bị & Kiểm Tra Luhn 3GPP

### 4.1 Sơ Đồ Tuần Tự Gọi Hàm Chi Tiết
```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as ThietBiIMEIMenu::themThietBi
    participant Input as InputHelper
    participant Norm as Normalized
    participant IMEIMod as ThietBiIMEI
    participant Repo as Repository<ThietBiIMEI>

    User->>Menu: Chọn 1 Thêm thiết bị mới
    activate Menu

    loop Kiểm tra mã IMEI hợp lệ chuẩn 3GPP
        Menu->>Input: getString("Nhap Ma IMEI", false)
        Input-->>Menu: imei = "860123456789014"
        Menu->>IMEIMod: validateLuhn("860123456789014")
        activate IMEIMod
        IMEIMod-->>Menu: return true
        deactivate IMEIMod
        Menu->>Repo: findById(imei)
        Repo-->>Menu: nullptr
    end

    Menu->>Input: getString("Nhap Ten thiet bi", false)
    Input-->>Menu: tenTB = "iPhone 15 Pro"
    Menu->>Input: getString("Nhap Hang san xuat", false)
    Input-->>Menu: hangSX = "Apple"

    Menu->>Input: getPhoneNumber("Nhap So dien thoai gan kem", true)
    activate Input
    Input->>Norm: isValidPhoneNumber(sdt, true)
    Norm-->>Input: true
    Input-->>Menu: sdt = "0981234567"
    deactivate Input

    Menu->>Input: getDate("Nhap Ngay kich hoat")
    Input-->>Menu: ngayKH

    Menu->>Input: getString("Nhap Tram BTS gan nhat", false)
    Input-->>Menu: bts = "BTS-HN-001"

    Note over IMEIMod,Norm: ThietBiIMEI duoc tach roi khoi InputHelper, goi Normalized truc tiep
    Menu->>IMEIMod: ThietBiIMEI(imei, tenTB, hangSX, sdt, ngayKH, "HoatDong", bts)
    activate IMEIMod
    IMEIMod->>IMEIMod: validateLuhn(imei)
    IMEIMod->>Norm: isValidPhoneNumber(sdt, true)
    Norm-->>IMEIMod: true
    IMEIMod-->>Menu: tb
    deactivate IMEIMod

    Menu->>Repo: add(tb)
    activate Repo
    Repo->>Repo: saveToFile()
    Repo-->>Menu: return true
    deactivate Repo

    Menu-->>User: "[THANH CONG] Da them thiet bi IMEI 860123456789014 vao he thong!"
    Menu->>Input: pause()
    deactivate Menu
```

---

## 5. Luồng Gọi Hàm: Tra Cứu, Lọc & Sắp Xếp Đa Năng

### 5.1 Tra Cứu & Lọc Đa Điều Kiện Với Lambda Expression
```mermaid
sequenceDiagram
    autonumber
    participant Menu as HopDongMenu::timKiemHopDong()
    participant Input as InputHelper
    participant Repo as Repository<HopDong>
    participant HD as HopDong

    Menu->>Input: getInt("Lua chon tra cuu", 0, 3)
    Input-->>Menu: choice = 2

    Menu->>Input: getString("Nhap So dien thoai can tim", false)
    Input-->>Menu: sdt = "0981234567"

    Menu->>Repo: filter([&sdt](const HopDong& hd) { return hd.getSoDienThoai() == sdt; })
    activate Repo
    loop Duyệt từng item trong vector
        Repo->>Repo: predicate(item)
        opt Nếu true
            Repo->>Repo: result.push_back(item)
        end
    end
    Repo-->>Menu: results
    deactivate Repo

    alt Kết quả rỗng
        Menu-->>Menu: In thông báo rỗng
    else Có kết quả
        Menu->>HD: results[0].displayHeader()
        loop Duyệt từng kết quả
            Menu->>HD: hd.displayRow()
        end
    end
```

### 5.2 Sắp Xếp Danh Sách Bằng Thuật Toán Tiêu Chuẩn `std::sort`
```mermaid
sequenceDiagram
    autonumber
    participant Menu as HopDongMenu::sapXepDanhSach()
    participant Input as InputHelper
    participant Repo as Repository<HopDong>
    participant StdSort as std::sort

    Menu->>Input: getInt("Lua chon sap xep", 0, 2)
    Input-->>Menu: choice = 2

    Menu->>Repo: sort([](const HopDong& a, const HopDong& b) { return a.getGiaTriGoi() > b.getGiaTriGoi(); })
    activate Repo
    Repo->>StdSort: std::sort(items.begin(), items.end(), comparator)
    StdSort-->>Repo: Hoàn tất sắp xếp trong RAM
    deactivate Repo

    Menu->>Menu: xemDanhSach()
```

---

## 6. Ma Trận Lan Truyền Ngoại Lệ Giữa Các Hàm

| Lớp Ngoại Lệ | Hàm Phát Sinh | Điều Kiện Kích Hoạt | Hàm Đón Bắt | Hành Vi Xử Lý Ngoại Lệ |
|---|---|---|---|---|
| **`InvalidLuhnException`** | `ThietBiIMEI::ThietBiIMEI`<br/>`ThietBiIMEI::setMaIMEI` | Chuỗi IMEI không thỏa mãn thuật toán Luhn Mod-10 hoặc không đủ 15 chữ số. | `ThietBiIMEIMenu::themThietBi` | In thông báo lỗi, ngăn không cho khởi tạo thực thể lỗi và yêu cầu nhập lại. |
| **`InvalidDateException`** | `Date::parse`<br/>`Date::setDay/Month/Year`<br/>`HopDong::HopDong`<br/>`HopDong::giaHan` | Ngày không hợp lệ hoặc ngày hết hạn nhỏ hơn ngày đăng ký. | `InputHelper::getDate`<br/>`HopDongMenu::capNhatHopDong` | In thông báo chi tiết của lỗi và duy trì vòng lặp nhập liệu an toàn. |
| **`InvalidPhoneNumberException`** | `HopDong::HopDong`<br/>`HopDong::setSoDienThoai`<br/>`ThietBiIMEI::ThietBiIMEI` | Số điện thoại không đủ 10 chữ số hoặc sai đầu số nhà mạng Việt Nam. | `HopDongMenu::capNhatHopDong`<br/>`ThietBiIMEIMenu::capNhatThietBi` | Bắt tại tầng Menu, in cảnh báo và hủy bỏ thao tác gán SIM sai. |
| **`DuplicateIdException`** | `Repository<T>::add` | Mã định danh đã tồn tại trong danh sách RAM của Repository. | `HopDongMenu::themHopDong`<br/>`ThietBiIMEIMenu::themThietBi` | Tầng menu kiểm tra trước qua `findById`; nếu lọt ngoại lệ sẽ in thông báo mã trùng. |
| **`NotFoundException`** | `Repository<T>::update`<br/>`Repository<T>::remove` | Không tìm thấy phần tử có mã cần sửa hoặc xóa. | `HopDongMenu::capNhatHopDong`<br/>`HopDongMenu::xoaHopDong` | In thông báo không tìm thấy mã. |
| **`FileIOException`** | `Repository<T>::saveToFile` | Không thể mở file tạm để ghi hoặc thao tác đổi tên thất bại. | `main` | Bắt ở mức ứng dụng cao nhất, bảo vệ dữ liệu gốc không bị ghi đè khi lỗi ổ đĩa. |

---

## 7. Bảng Đối Chiếu Nguyên Mẫu Hàm Theo Phân Tầng

### 7.1 Lớp `InputHelper` — Console Input Family (`src/lib/shared/InputHelper.h`)
```cpp
static void clearBuffer();
static std::string getString(const std::string& prompt, bool allowEmpty = false);
static int getInt(const std::string& prompt, int minVal = InputLimits::DEFAULT_MIN_INT, int maxVal = InputLimits::DEFAULT_MAX_INT);
static double getDouble(const std::string& prompt, double minVal = InputLimits::DEFAULT_MIN_DOUBLE, double maxVal = InputLimits::DEFAULT_MAX_DOUBLE);
static Date getDate(const std::string& prompt);
static Date getBirthDate(const std::string& prompt, int minAge = 14, int maxAge = 120);
static bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false);
static std::string getPhoneNumber(const std::string& prompt, bool allowUnassigned = false);
static bool getConfirm(const std::string& prompt);
static void pause(const std::string& message = "Nhan Enter de tiep tuc...");
```

### 7.2 Phân Hệ `Normalized` — Sanitization & Validation Family (`src/lib/shared/Normalized.h`)
```cpp
namespace Normalized {
    std::string trim(const std::string& str);
    std::string CollapseSpace(const std::string& str);
    std::string removeSymbols(const std::string& str);
    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    std::string NormalizedName(const std::string& str);
    bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false);
}
```

### 7.3 Thư Viện Tiện Ích `DisplayHelper` — Presentation & ASCII Layout (`src/lib/shared/DisplayHelper.h`)
```cpp
namespace DisplayHelper {
    void printBorder(const std::vector<int>& widths, std::ostream& os = std::cout);
    void printHeader(const std::vector<std::string>& headers, const std::vector<int>& widths, const std::vector<bool>& rightAlign = {}, std::ostream& os = std::cout);
    void printRow(const std::vector<std::string>& cells, const std::vector<int>& widths, const std::vector<bool>& rightAlign = {}, std::ostream& os = std::cout);
    void printCard(const std::vector<std::pair<std::string, std::string>>& fields, int labelWidth = 16, std::ostream& os = std::cout);
}
```

### 7.4 Lớp `Repository<T>` — Generic In-Memory CRUD & Flat-File Store (`src/lib/shared/Repository.h`)
```cpp
bool loadFromFile();
bool saveToFile() const;
bool add(const T& item);
bool update(const std::string& id, const T& updatedItem);
bool remove(const std::string& id);
T* findById(const std::string& id);
const T* findById(const std::string& id) const;
std::vector<T> filter(std::function<bool(const T&)> predicate) const;
void sort(std::function<bool(const T&, const T&)> comparator);
```

### 7.5 Lớp `ThietBiIMEI` — Telecom Domain Entity (`src/lib/models/imei.h`)
```cpp
static bool validateLuhn(const std::string& imeiStr);
bool isBlacklisted() const;
void setBlacklist(bool lock);
void ganSIM(const std::string& sdt);
void goSIM();
void capNhatBTS(const std::string& bts);
```

### 7.6 Lớp `HopDong` — Contract Lifecycle Domain Entity (`src/lib/models/hopdong.h`)
```cpp
bool isExpired(const Date& currentDate) const;
void giaHan(const Date& ngayHetHanMoi);
void chamDut();
void tamDung();
void kichHoatLai();
```
