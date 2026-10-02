# ĐẶC TẢ THIẾT KẾ KỸ THUẬT (TECHNICAL DESIGN SPECIFICATION)
## HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG — PHÂN HỆ TRẦN ĐỨC ANH (B24DCVT021)

---

## 1. Sơ Đồ Lớp Kế Thừa (Class Diagram & Architecture)

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

## 2. Chi Tiết Các Lớp Nghiệp Vụ & Thành Phần Cốt Lõi

### 2.1 Lớp Cơ Sở Trừu Tượng `Entity` (`src/lib/Entity.h`)
* **Mục đích:** Đóng vai trò lớp cơ sở (Base Class) định hình giao diện đa hình cho mọi thực thể dữ liệu trong hệ thống.
* **Phương thức thuần ảo (Pure Virtual Methods):**
  - `toFileString()`: Đóng gói toàn bộ thuộc tính đối tượng thành 1 dòng văn bản phân cách bởi dấu `|`.
  - `fromFileString(const std::string& line)`: Phân rã chuỗi dòng tệp tin và điền dữ liệu vào các thuộc tính thành viên.
  - `displayHeader()`, `displayRow()`, `displayDetail()`: Định dạng đầu ra trực quan bằng thư viện `<iomanip>`.

### 2.2 Lớp `HopDong` (`src/lib/hopdong.h` & `src/lib/hopdong.cpp`)
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

### 2.3 Lớp `ThietBiIMEI` (`src/lib/imei.h` & `src/lib/imei.cpp`)
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

### 2.4 Lớp Đối Tượng Ngày Tháng `Date` (`src/lib/Date.h` & `src/lib/Date.cpp`)
* **Thuộc tính:** `day` (1–31), `month` (1–12), `year` ($\ge 1900$).
* **Xử lý năm nhuận:**
  ```cpp
  bool Date::isLeapYear(int y) {
      return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
  }
  ```
* **Toán tử & So sánh:** Nạp chồng các toán tử so sánh `operator<`, `operator==`, `operator<=` để phục vụ sắp xếp và xác thực logic `ngayHetHan >= ngayDangKy`.

### 2.5 Lớp Generic Database Engine `Repository<T>` (`src/lib/Repository.h`)
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

---

## 3. Quản Lý Ngoại Lệ (Exception Hierarchy) (`src/lib/Exceptions.h`)

Kế thừa từ `std::runtime_error` để bắt và hiển thị thông báo lỗi nghiệp vụ rõ ràng:
- `AppException`: Ngoại lệ cơ sở của ứng dụng.
- `DuplicateIdException`: Báo lỗi khi tạo đối tượng có mã ID đã tồn tại.
- `NotFoundException`: Báo lỗi khi không tìm thấy bản ghi theo ID.
- `InvalidLuhnException`: Báo lỗi khi mã IMEI không vượt qua thuật toán Luhn Mod-10.
- `InvalidDateException`: Báo lỗi khi ngày tháng không hợp lệ hoặc ngày hết hạn < ngày đăng ký.
- `FileIOException`: Báo lỗi khi không thể mở hoặc ghi tệp dữ liệu.

---

## 4. Quy Ước Đặt Tên & Phối Hợp Ngôn Ngữ (Bilingual Naming Policy)

Hệ thống áp dụng chiến lược phân tách ngôn ngữ có chủ đích giữa Tên tệp tin nghiệp vụ (Tiếng Việt) và Tên hàm/Phương thức kiến trúc (Tiếng Anh):

| Thành Phần | Ngôn Ngữ Áp Dụng | Ví Dụ Minh Họa | Lý Do Kiến Trúc |
| :--- | :--- | :--- | :--- |
| **Tên tệp tin thực thể & dữ liệu** | **Tiếng Việt không dấu** (`lowercase` / `PascalCase`) | `hopdong.h/cpp`, `imei.h/cpp`<br>`data/hopdong.txt`, `data/imei.txt`<br>`HopDongMenu`, `ThietBiIMEIMenu` | Khớp 1:1 với đề bài BTL KTLT của Học viện PTIT và phân công giữa 5 thành viên trong nhóm. |
| **Tên phương thức hạ tầng & thuật toán** | **Tiếng Anh chuẩn mực** (`camelCase`) | `getId()`, `isExpired()`<br>`validateLuhn()`, `isBlacklisted()`<br>`toFileString()`, `fromFileString()`<br>`loadFromFile()`, `saveToFile()`<br>`displayHeader()`, `displayRow()` | Tuân thủ chuẩn mực lập trình C++ OOP quốc tế, giao diện Repository pattern và các thuật toán tiêu chuẩn (3GPP Luhn). |
| **Hàm điều hướng giao diện & Menu** | **Tiếng Việt ngữ cảnh** (`camelCase`) | `themHopDong()`, `xemDanhSach()`<br>`timKiemHopDong()`, `sapXepDanhSach()`<br>`giaHan()`, `chamDut()`, `ganSIM()` | Giúp giảng viên và thành viên nhóm theo dõi trực quan đúng với quy trình nghiệp vụ viễn thông tại Việt Nam. |

