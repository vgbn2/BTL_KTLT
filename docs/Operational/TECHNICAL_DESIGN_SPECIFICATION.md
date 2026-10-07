# ĐẶC TẢ THIẾT KẾ KỸ THUẬT
## HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG — PHÂN HỆ TRẦN ĐỨC ANH (B24DCVT021)

---

## 1. Cấu Trúc Phân Tầng Thư Viện (Library Categorization & Architecture Hierarchy)

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
│   └── Repository.h           # Generic In-Memory Collection & Flat-File Persistence Engine
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

## 2. Sơ Đồ Lớp Kế Thừa

```
+---------------------------------------------------------------+
|                       <<abstract>>                            |
|                          Entity                               |
+---------------------------------------------------------------+
| # id: std::string                                             |
+---------------------------------------------------------------+
| + virtual ~Entity() = default                                 |
| + getId() const: std::string                                  |
| + setId(const std::string& id): void                          |
| + virtual void displayHeader() const = 0                      |
| + virtual void displayRow() const = 0                         |
| + virtual void displayDetail() const = 0                      |
| + virtual std::string toFileString() const = 0                |
| + virtual bool fromFileString(const std::string& line) = 0    |
+-------------------------------+-------------------------------+
                                |
        +-----------------------+-----------------------+
        |                                               |
+-------v-----------------------+       +---------------v---------------+
|           HopDong             |       |          ThietBiIMEI          |
+-------------------------------+       +-------------------------------+
| - maKhachHang: std::string    |       | - tenThietBi: std::string     |
| - soDienThoai: std::string    |       | - hangSanXuat: std::string    |
| - maGoiCuoc: std::string      |       | - soDienThoai: std::string    |
| - ngayDangKy: Date            |       | - ngayKichHoat: Date          |
| - ngayHetHan: Date            |       | - trangThai: std::string      |
| - loaiHopDong: std::string    |       | - tramBTSGanNhat: std::string |
| - trangThai: std::string      |       +-------------------------------+
| - giaTriGoi: double           |       | + static bool validateLuhn(s) |
+-------------------------------+       | + bool isBlacklisted() const  |
| + bool isExpired(Date now)    |       | + void setBlacklist(bool b)   |
| + void giaHan(int soThang)    |       | + void ganSIM(string sdt)     |
| + void chamDut()              |       | + void capNhatBTS(string bts) |
+-------------------------------+       +-------------------------------+
```

---

## 3. Chi Tiết Các Lớp Nghiệp Vụ & Thành Phần Cốt Lõi

### 3.1 Lớp Cơ Sở Trừu Tượng `Entity` (`src/lib/shared/Entity.h`)
* **Mục đích:** Đóng vai trò lớp cơ sở (Base Class) định hình giao diện đa hình cho mọi thực thể dữ liệu trong hệ thống.
* **Phương thức thuần ảo (Pure Virtual Methods):**
  - `toFileString()`: Đóng gói toàn bộ thuộc tính đối tượng thành 1 dòng văn bản phân cách bởi dấu `|`.
  - `fromFileString(const std::string& line)`: Phân rã chuỗi dòng tệp tin và điền dữ liệu vào các thuộc tính thành viên.
  - `displayHeader()`, `displayRow()`, `displayDetail()`: Định dạng đầu ra trực quan bằng thư viện `<iomanip>`.

### 3.2 Lớp `HopDong` (`src/lib/models/hopdong.h` & `src/lib/models/hopdong.cpp`)
* **Thuộc tính:**
  - `id` (Mã HĐ): Kế thừa từ `Entity`, ví dụ: `HD0001`, `HD0002`.
  - `maKhachHang`: Mã định danh khách hàng, ví dụ: `KH0001`.
  - `soDienThoai`: Số điện thoại thuê bao 10 số, ví dụ: `0981234567`.
  - `maGoiCuoc`: Gói cước viễn thông gắn kết, ví dụ: `GC001`, `VD149`.
  - `ngayDangKy`: Ngày ký hợp đồng (`Date`).
  - `ngayHetHan`: Ngày hợp đồng hết hiệu lực (`Date`).
  - `loaiHopDong`: Hình thức thuê bao (`TraTruoc` / `TraSau`).
  - `trangThai`: Trạng thái BSS (`HieuLuc`, `TamDung`, `ThanhLy`).
  - `giaTriGoi`: Cước phí dịch vụ theo chu kỳ (VND).
* **Nghiệp vụ cốt lõi:**
  - Tự động kiểm tra thời hạn: Nếu `ngayHetHan < ngayHienTai`, hợp đồng chuyển trạng thái cảnh báo hoặc cần gia hạn.
  - Chấm dứt hợp đồng: Chuyển `trangThai` thành `ThanhLy`.

### 3.3 Lớp `ThietBiIMEI` (`src/lib/models/imei.h` & `src/lib/models/imei.cpp`)
* **Thuộc tính:**
  - `id` (Mã IMEI 15 chữ số): Kế thừa từ `Entity`, ví dụ: `860123456789012`.
  - `tenThietBi`: Model điện thoại (iPhone 15 Pro, Galaxy S24...).
  - `hangSanXuat`: Nhà sản xuất phần cứng (Apple, Samsung, Xiaomi...).
  - `soDienThoai`: Số SIM đang hoạt động trên máy (`0981234567` hoặc `ChuaGan`).
  - `ngayKichHoat`: Ngày thiết bị gắn SIM truy cập mạng lần đầu (`Date`).
  - `trangThai`: Trạng thái trên mạng viễn thông (`HoatDong`, `TamKhoa`, `KhoaMang`).
  - `tramBTSGanNhat`: Mã định danh trạm phát sóng di động gắn kết (`BTS-HN-001`).
* **Thuật toán Xác thực 3GPP Luhn Checksum (Mod-10):**
  - IMEI gồm 14 chữ số dữ liệu ($d_1, d_2, \dots, d_{14}$) và 1 chữ số kiểm tra ($d_{15}$).
  - Duyệt từ chữ số cuối (phải qua trái): các chữ số ở vị trí chẵn từ phải sang (tức vị trí có khoảng cách 2 tính từ cuối) được nhân đôi ($2 \times d_i$).
  - Nếu $2 \times d_i > 9$, tổng các chữ số của tích là $(2 \times d_i - 9)$.
  - Các chữ số ở vị trí lẻ giữ nguyên giá trị.
  - Tính tổng toàn bộ: $S = \sum_{i=1}^{15} f(d_i)$.
  - Nếu $S \pmod{10} == 0$, mã IMEI là **hợp lệ**. Ngược lại là **không hợp lệ**.

### 3.4 Lớp Đối Tượng Ngày Tháng `Date` (`src/lib/shared/Date.h` & `src/lib/shared/Date.cpp`)
* **Thuộc tính:** `day` (1–31), `month` (1–12), `year` ($\ge 1900$).
* **Xử lý năm nhuận:**
  ```cpp
  bool Date::isLeapYear(int y) {
      return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
  }
  ```
* **Toán tử & So sánh:** Nạp chồng các toán tử so sánh `operator<`, `operator==`, `operator<=` để phục vụ sắp xếp và xác thực logic `ngayHetHan >= ngayDangKy`.

### 3.5 Lớp Generic Database Engine `Repository<T>` (`src/lib/shared/Repository.h`)
* **Thiết kế:** Template class quản lý danh sách bộ nhớ `std::vector<T>` liên kết với tệp văn bản đĩa cứng `data/*.txt`.
* **Phương thức chính:**
  - `bool loadFromFile()`: Đọc từng dòng tệp tin, phân tách bằng `|`, tạo đối tượng và nạp vào vector.
  - `bool saveToFile()`: Ghi dữ liệu an toàn (ghi vào `.tmp`, kiểm tra ghi thành công, sau đó đổi tên ghi đè tệp chính).
  - `bool add(const T& item)`: Kiểm tra trùng ID trước khi thêm.
  - `bool update(const std::string& id, const T& item)`: Cập nhật bản ghi theo khóa chính.
  - `bool remove(const std::string& id)`: Xóa bản ghi theo khóa chính.
  - `T* findById(const std::string& id)`: Tìm kiếm con trỏ bản ghi theo ID.
  - `std::vector<T>& getAll()`: Trả về tham chiếu toàn bộ danh sách.
  - `std::vector<T> filter(std::function<bool(const T&)> predicate)`: Lọc dữ liệu linh hoạt.
  - `void sort(std::function<bool(const T&, const T&)> comparator)`: Sắp xếp theo tiêu chí tùy biến.

### 2.6 Thư Viện Tiện Ích Giao Diện Console `DisplayHelper` (`src/lib/shared/DisplayHelper.h`)
* **Thiết kế:** Namespace chứa các hàm tiện ích định dạng bảng ASCII và thẻ chi tiết (card layout), loại bỏ mã lặp `<iomanip>` trong các lớp thực thể `HopDong` và `ThietBiIMEI`.
* **Phương thức chính:**
  - `void printBorder(const std::vector<int>& widths, std::ostream& os)`: Vẽ đường viền bảng `+---+---+` theo danh sách độ rộng cột.
  - `void printHeader(const std::vector<std::string>& headers, const std::vector<int>& widths, const std::vector<bool>& rightAlign, std::ostream& os)`: In tiêu đề bảng kèm đường viền trên/dưới.
  - `void printRow(const std::vector<std::string>& cells, const std::vector<int>& widths, const std::vector<bool>& rightAlign, std::ostream& os)`: In một hàng dữ liệu với căn lề trái/phải tùy biến.
  - `void printCard(const std::vector<std::pair<std::string, std::string>>& fields, int labelWidth, std::ostream& os)`: In thông tin chi tiết một bản ghi dưới dạng thẻ khóa-giá trị (`Key : Value`).

### 2.7 Bộ Tiện Ích Chuẩn Hóa & Xác Thực Dữ Liệu `Normalized` (`src/lib/shared/Normalized.h`)
* **Thiết kế:** Namespace thuần túy (Sanitization & Validation Family) chịu trách nhiệm làm sạch chuỗi và xác thực dữ liệu nghiệp vụ không phụ thuộc vào I/O console.
* **Phương thức chính:**
  - `bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false)`: Xác thực số điện thoại di động Việt Nam (đúng 10 chữ số, bắt đầu bằng `0`, thuộc các đầu số `03`, `05`, `07`, `08`, `09`).
  - `std::string trim(const std::string& str)` / `CollapseSpace(const std::string& str)`: Cắt khoảng trắng đầu/cuối và thu gọn khoảng trắng liên tiếp.
  - `std::string removeSymbols(const std::string& str)`: Loại bỏ các ký tự phân tách nguy hiểm (`|`, `{`, `}`, `<`, `>`, ...) ngăn chặn lỗi format tệp tin.
  - `std::string NormalizedName(const std::string& str)`: Chuẩn hóa họ tên người dùng/khách hàng.

### 2.8 Phân Hệ Nhập Liệu Console An Toàn `InputHelper` (`src/lib/shared/InputHelper.h`)
* **Thiết kế:** Lớp tiện ích tĩnh (Console Input Family) chịu trách nhiệm duy nhất là trích xuất và kiểm soát nhập liệu an toàn từ bàn phím Console qua `std::cin`.
* **Đặc tính kỹ thuật & Tách rời (Decoupling):**
  - Quản lý `cin.clear()`, `cin.ignore()`, ngăn ngừa trôi dòng bộ đệm và tấn công chèn ký tự phân tách.
  - Các lớp thực thể Domain (`HopDong`, `ThietBiIMEI`) hoàn toàn tách rời khỏi `InputHelper`, chỉ gọi trực tiếp `Normalized::isValidPhoneNumber` để kiểm tra bất biến dữ liệu.

---

## 3. Quản Lý Ngoại Lệ (`src/lib/shared/Exceptions.h`)

Kế thừa từ `std::runtime_error` để bắt và hiển thị thông báo lỗi nghiệp vụ rõ ràng:
- `AppException`: Ngoại lệ cơ sở của ứng dụng.
- `DuplicateIdException`: Báo lỗi khi tạo đối tượng có mã ID đã tồn tại.
- `NotFoundException`: Báo lỗi khi không tìm thấy bản ghi theo ID.
- `InvalidLuhnException`: Báo lỗi khi mã IMEI không vượt qua thuật toán Luhn Mod-10.
- `InvalidDateException`: Báo lỗi khi ngày tháng không hợp lệ hoặc ngày hết hạn < ngày đăng ký.
- `FileIOException`: Báo lỗi khi không thể mở hoặc ghi tệp dữ liệu.

---

## 4. Quy Ước Đặt Tên & Phối Hợp Ngôn Ngữ

Hệ thống áp dụng chiến lược phân tách ngôn ngữ có chủ đích giữa Tên tệp tin nghiệp vụ (Tiếng Việt) và Tên hàm/Phương thức kiến trúc (Tiếng Anh):

| Thành Phần | Ngôn Ngữ Áp Dụng | Ví Dụ Minh Họa | Lý Do Kiến Trúc |
| :--- | :--- | :--- | :--- |
| **Tên tệp tin thực thể & dữ liệu** | **Tiếng Việt không dấu** (`lowercase` / `PascalCase`) | `hopdong.h/cpp`, `imei.h/cpp`<br>`data/hopdong.txt`, `data/imei.txt`<br>`HopDongMenu`, `ThietBiIMEIMenu` | Khớp 1:1 với đề bài BTL KTLT của Học viện PTIT và phân công giữa 5 thành viên trong nhóm. |
| **Tên phương thức hạ tầng & thuật toán** | **Tiếng Anh chuẩn mực** (`camelCase`) | `getId()`, `isExpired()`<br>`validateLuhn()`, `isBlacklisted()`<br>`toFileString()`, `fromFileString()`<br>`loadFromFile()`, `saveToFile()`<br>`displayHeader()`, `displayRow()` | Tuân thủ chuẩn mực lập trình C++ OOP quốc tế, giao diện Repository pattern và các thuật toán tiêu chuẩn (3GPP Luhn). |
| **Hàm điều hướng giao diện & Menu** | **Tiếng Việt ngữ cảnh** (`camelCase`) | `themHopDong()`, `xemDanhSach()`<br>`timKiemHopDong()`, `sapXepDanhSach()`<br>`giaHan()`, `chamDut()`, `ganSIM()` | Giúp giảng viên và thành viên nhóm theo dõi trực quan đúng với quy trình nghiệp vụ viễn thông tại Việt Nam. |

