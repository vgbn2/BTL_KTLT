# TÀI LIỆU KỸ THUẬT PHÂN HỆ MÔ HÌNH DỮ LIỆU (DOMAIN MODELS)
## SINH VIÊN: TRẦN ĐỨC ANH — MSSV: B24DCVT021
### HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG KTLT — PTIT

---

## 1. Kiến Trúc & Thiết Kế Phân Tầng Lớp Mô Hình (Domain Models Architecture)

Trong hệ thống quản lý thuê bao viễn thông, tầng mô hình dữ liệu (`src/lib/models/`) chịu trách nhiệm biểu diễn các thực thể nghiệp vụ cốt lõi, bảo vệ tính toàn vẹn dữ liệu (Data Integrity) và thực thi các quy tắc bất biến nghiệp vụ (Business Invariants).

```
                      +-----------------------------+
                      |       <<Abstract>>          |
                      |          Entity             |
                      |-----------------------------|
                      | # id : std::string          |
                      |-----------------------------|
                      | + getId() : std::string     |
                      | + setId(id) : void          |
                      | + toFileString()* : string  |
                      | + fromFileString(line)*:bool|
                      | + displayHeader()* : void   |
                      | + displayRow()* : void      |
                      | + displayDetail()* : void   |
                      +--------------+--------------+
                                     |
             +-----------------------+-----------------------+
             |                                               |
             v                                               v
+-----------------------------+             +-----------------------------+
|           HopDong           |             |         ThietBiIMEI         |
|-----------------------------|             |-----------------------------|
| - maKhachHang : string      |             | - tenThietBi : string       |
| - soDienThoai : string      |             | - hangSanXuat : string      |
| - maGoiCuoc : string        |             | - soDienThoai : string      |
| - ngayDangKy : Date         |             | - ngayKichHoat : Date       |
| - ngayHetHan : Date         |             | - trangThai : string        |
| - loaiHopDong : string      |             | - tramBTSGanNhat : string   |
| - trangThai : string        |             |-----------------------------|
| - giaTriGoi : double        |             | + validateLuhn(imei)*: bool |
|-----------------------------|             | + isBlacklisted() : bool    |
| + isExpired(Date) : bool    |             | + setBlacklist(bool) : void |
| + giaHan(Date) : void       |             | + ganSIM(sdt) : void        |
| + chamDut() : void          |             | + goSIM() : void            |
| + tamDung() : void          |             | + capNhatBTS(bts) : void    |
+-----------------------------+             +-----------------------------+
             |                                               |
             +-----------------------+-----------------------+
                                     | (Delegates formatting)
                                     v
                      +-----------------------------+
                      |        DisplayHelper        |
                      |-----------------------------|
                      | + printHeader(...) : void   |
                      | + printRow(...) : void      |
                      | + printCard(...) : void     |
                      | + printBorder(...) : void   |
                      +-----------------------------+
```

### 1.1 Lớp Cơ Sở Trừu Tượng `Entity` (`src/lib/shared/Entity.h`)
* **Mục đích:** Định nghĩa giao diện chung cho tất cả các đối tượng có thể lưu trữ trong bộ nhớ và ghi ra file qua cơ chế ánh xạ `Repository<T>`.
* **Đặc tính kỹ thuật:**
  - `virtual ~Entity() = default;`: Đảm bảo giải phóng bộ nhớ an toàn khi hủy đối tượng đa hình qua con trỏ lớp cha `Entity*`.
  - `toFileString() const = 0;` & `fromFileString(const std::string&) = 0;`: Đóng gói (Serialization) và giải mã (Deserialization) bản ghi thành chuỗi phân tách bởi ký tự `|`.
  - `displayHeader() const = 0;`, `displayRow() const = 0;`, `displayDetail() const = 0;`: Đảm bảo tính đa hình hiển thị trên giao diện dòng lệnh.

### 1.2 Mẫu Thiết Kế Ủy Nhiệm Hiển Thị (Display Delegation Pattern)
* **Vấn đề thiết kế:** Nếu các lớp thực thể (`HopDong`, `ThietBiIMEI`) tự chứa toàn bộ mã nguồn xử lý chuỗi căn lề ASCII (`std::setw`, vòng lặp vẽ đường kẻ ngang `+---+`), mã nguồn thực thể sẽ bị phình to (boilerplate) và vi phạm nguyên lý đơn nhiệm (Single Responsibility Principle - SRP).
* **Giải pháp áp dụng:** Tạo phân hệ `DisplayHelper.h` đóng gói toàn bộ logic in bảng (`printHeader`, `printRow`) và in phiếu chi tiết (`printCard`). Các hàm `display...()` trong thực thể chỉ đóng vai trò ủy nhiệm (delegate) bằng cách chuẩn bị danh sách chuỗi dữ liệu và truyền cho `DisplayHelper`.
* **Lợi ích:**
  1. Giữ nguyên tính đa hình (`virtual`) phục vụ yêu cầu học phần OOP.
  2. Tách biệt hoàn toàn logic định dạng giao diện khỏi logic nghiệp vụ của thực thể.
  3. Khi cần thay đổi kiểu viền bảng hoặc màu sắc giao diện, chỉ cần sửa đổi duy nhất `DisplayHelper.h`.

### 1.3 Tách Rời Mô Hình Khỏi Phân Hệ Nhập Liệu Console (Decoupling from InputHelper)
* **Vấn đề kiến trúc:** Nếu các lớp thực thể nghiệp vụ (`HopDong`, `ThietBiIMEI`) phụ thuộc trực tiếp vào `InputHelper` (ví dụ thông qua kiểm tra số điện thoại), tầng mô hình nghiệp vụ (Domain Layer) sẽ bị liên kết chặt chẽ (tight coupling) với tầng nhập liệu dòng lệnh (Presentation / Console Input Layer). Điều này làm xói mòn tính độc lập của mô hình, gây khó khăn khi nạp dữ liệu từ tệp tin hoặc tái sử dụng trong các môi trường không dùng giao diện console (automated unit testing, batch processing, GUI/Web API).
* **Giải pháp thiết kế:**
  - **Phân định rõ ranh giới hai họ hàm:**
    + **Sanitization & Validation Family (`Normalized`):** Chuyên trách làm sạch dữ liệu và kiểm tra tính hợp lệ về mặt ngữ nghĩa (pure validation functions). Điển hình là `Normalized::isValidPhoneNumber` kiểm tra định dạng 10 chữ số và dải đầu số di động hợp lệ tại Việt Nam (`03`, `05`, `07`, `08`, `09`) mà không gây bất kỳ hiệu ứng phụ (side effect) nào liên quan đến I/O.
    + **Console Input Family (`InputHelper`):** Chuyên trách bắt luồng nhập liệu từ bàn phím qua `std::cin`, xử lý xóa bộ đệm lỗi (`cin.clear()`, `cin.ignore()`), kiểm tra chống chèn ký tự phân tách `|` (delimiter injection) và duy trì vòng lặp hỏi lại người dùng khi phát hiện sai sót.
  - **Tách rời hoàn toàn (Full Decoupling):** Cả `HopDong` và `ThietBiIMEI` đều được tách rời tuyệt đối khỏi `InputHelper`. Khi cần kiểm tra tính hợp lệ của số điện thoại trong hàm khởi tạo hoặc phương thức cập nhật (`setSoDienThoai`, `ganSIM`), mô hình gọi trực tiếp `Normalized::isValidPhoneNumber`, không phụ thuộc và không `#include "InputHelper.h"`.
* **Lợi ích:**
  1. **Độc lập kiến trúc (Architectural Cleanliness):** Tầng mô hình không bị "nhiễm bẩn" bởi mã nguồn I/O bàn phím.
  2. **An toàn kiểm thử:** Các ca kiểm thử tự động (`test_runner.cpp`) thực thi kiểm tra tính toàn vẹn dữ liệu trên RAM mà không phát sinh bất kỳ tương tác dòng lệnh nào.
  3. **Tuân thủ nguyên lý SoC (Separation of Concerns):** `InputHelper` quản lý console, `Normalized` xử lý chuẩn hóa và kiểm tra dữ liệu, `Entity` bảo vệ tính toàn vẹn nghiệp vụ.

---

## 2. Mô Hình Thực Thể Hợp Đồng (`HopDong` - UC01)

### 2.1 Thuộc Tính & Ý Nghĩa Viễn Thông
| Thuộc tính | Kiểu dữ liệu | Ý nghĩa trong hệ thống viễn thông | Ràng buộc / Bất biến dữ liệu |
| :--- | :--- | :--- | :--- |
| `id` (`maHopDong`) | `std::string` | Mã định danh duy nhất của hợp đồng (PK) | Không được rỗng, chuẩn hóa viết hoa |
| `maKhachHang` | `std::string` | Mã khách hàng đứng tên ký hợp đồng (FK) | Tham chiếu đến danh mục `KhachHang` |
| `soDienThoai` | `std::string` | Số thuê bao di động sử dụng gói cước | 10 chữ số, đầu số hợp lệ (03, 05, 07, 08, 09) |
| `maGoiCuoc` | `std::string` | Mã gói cước dịch vụ viễn thông đăng ký (FK) | Tham chiếu danh mục gói (`CacGoiCuoc.txt`) |
| `ngayDangKy` | `Date` | Ngày bắt đầu có hiệu lực của hợp đồng | Phải là ngày lịch hợp lệ (hỗ trợ năm nhuận) |
| `ngayHetHan` | `Date` | Ngày hết hạn sử dụng gói dịch vụ | Phải lớn hơn hoặc bằng `ngayDangKy` |
| `loaiHopDong` | `std::string` | Phân loại phương thức thanh toán cước | `TraTruoc` (Prepaid) hoặc `TraSau` (Postpaid) |
| `trangThai` | `std::string` | Trạng thái vòng đời hợp đồng | `HieuLuc`, `TamDung`, `ThanhLy` |
| `giaTriGoi` | `double` | Cước phí đăng ký gói (VNĐ) | Phải $\ge 0$ |

### 2.2 Máy Trạng Thái Hợp Đồng (Contract Lifecycle State Machine)

```
       +-----------------+
       |   Dang Ky Moi   |
       +--------+--------+
                |
                v
       +-----------------+        tamDung()        +-----------------+
       |     HieuLuc     +------------------------>+     TamDung     |
       |    (Active)     |<------------------------+   (Suspended)   |
       +--------+--------+      kichHoatLai()      +--------+--------+
                |                                           |
                | chamDut()                                 | chamDut()
                v                                           v
       +-------------------------------------------------------------+
       |                          ThanhLy                            |
       |                        (Terminated)                         |
       +-------------------------------------------------------------+
```

### 2.3 Các Phương Thức Xử Lý Nghiệp Vụ Cốt Lõi
* `bool isExpired(const Date& currentDate) const`:
  - So sánh `ngayHetHan < currentDate` và trạng thái hiện tại là `HieuLuc`.
  - Trả về `true` nếu hợp đồng đã quá thời hạn cam kết để hệ thống cảnh báo gia hạn hoặc khóa cước tự động.
* `void giaHan(const Date& ngayHetHanMoi)`:
  - Kiểm tra `ngayHetHanMoi > ngayHetHan`. Nếu không hợp lệ ném ngoại lệ `ValidationException`.
  - Cập nhật thời hạn mới và tự động khôi phục trạng thái về `HieuLuc`.
* `void chamDut()`: Chuyển trạng thái sang `ThanhLy`, kết thúc quan hệ cung cấp dịch vụ.
* `void tamDung()`: Chuyển trạng thái sang `TamDung` khi khách hàng yêu cầu tạm khóa hoặc chậm đóng cước sau kỳ thông báo.
* `void kichHoatLai()`: Khôi phục từ `TamDung` về `HieuLuc`.

---

## 3. Mô Hình Thực Thể Thiết Bị IMEI (`ThietBiIMEI` - UC02)

### 3.1 Cấu Trúc Mã IMEI 15 Chữ Số Chuẩn Quốc Tế 3GPP TS 22.016
$$\text{IMEI} = \underbrace{d_1 d_2 d_3 d_4 d_5 d_6 d_7 d_8}_{\text{TAC (Type Allocation Code)}} \underbrace{d_9 d_{10} d_{11} d_{12} d_{13} d_{14}}_{\text{SNR (Serial Number)}} \underbrace{d_{15}}_{\text{CD (Check Digit)}}$$

* **TAC (Type Allocation Code - 8 chữ số):** Xác định nhà sản xuất thiết bị và model máy (do hiệp hội GSMA cấp phát).
* **SNR (Serial Number - 6 chữ số):** Số serial riêng biệt của từng chiếc điện thoại do nhà máy gán.
* **CD (Check Digit - 1 chữ số):** Chữ số kiểm tra tính toán theo giải thuật **Luhn Checksum Mod-10**.

### 3.2 Thuật Toán Kiểm Tra Toàn Vẹn Luhn Checksum Mod-10
* **Nguyên lý:**
  1. Đánh số các vị trí $i$ từ $1$ đến $15$ tính từ phải sang trái ($i=1$ ứng với chữ số cuối cùng $d_{15}$).
  2. Đối với các chữ số ở vị trí chẵn từ phải sang ($i \in \{2, 4, 6, 8, 10, 12, 14\}$):
     - Nhân đôi giá trị: $v = 2 \times d$.
     - Nếu $v \ge 10$, lấy $v - 9$ (tương đương cộng 2 chữ số của số có 2 chữ số).
  3. Đối với các chữ số ở vị trí lẻ từ phải sang ($i \in \{1, 3, 5, 7, 9, 11, 13, 15\}$): giữ nguyên giá trị $d$.
  4. Tính tổng tất cả 15 giá trị: $S = \sum_{i=1}^{15} v_i$.
  5. Mã IMEI hợp lệ khi và chỉ khi: $S \pmod{10} == 0$.

* **Cài đặt trong `ThietBiIMEI::validateLuhn` (`src/lib/models/imei.cpp`):**
```cpp
bool ThietBiIMEI::validateLuhn(const std::string& imeiStr) {
    // 1. Kiem tra do dai bat buoc 15 chu so chuan 3GPP
    if (imeiStr.length() != REQUIRED_IMEI_LENGTH) {
        return false;
    }

    // 2. Kiem tra tat ca ky tu phai la chu so [0-9] va khong duoc toan so 0
    bool allZeros = true;
    for (char c : imeiStr) {
        if (c < '0' || c > '9') {
            return false;
        }
        if (c != '0') {
            allZeros = false;
        }
    }
    if (allZeros) {
        return false;
    }

    // 3. Thuat toan Luhn Mod-10: Nhan doi cac chu so o vi tri thu tu chan tu phai sang
    int sum = 0;
    for (int i = static_cast<int>(REQUIRED_IMEI_LENGTH) - 1; i >= 0; --i) {
        int d = imeiStr[i] - '0';
        int posFromRight = static_cast<int>(REQUIRED_IMEI_LENGTH) - i;
        if (posFromRight % 2 == 0) {
            d *= LUHN_MULTIPLIER;
            if (d > 9) {
                d -= LUHN_DIGIT_OVERFLOW_SUBTRACT;
            }
        }
        sum += d;
    }
    return (sum % MODULO_BASE == 0);
}
```

### 3.3 Quản Trị Trạm Gốc & Danh Bạ Thiết Bị Mạng (EIR Blacklist)
* `isBlacklisted()` / `setBlacklist(bool lock)`:
  - Quản trị trạng thái `KhoaMang` (Blacklist). Khi thiết bị bị báo mất hoặc gian lận cước, hệ thống EIR gắn cờ khóa máy, ngăn chặn mọi SIM kết nối vào hạ tầng mạng vô tuyến.
* `ganSIM(const std::string& sdt)` / `goSIM()`:
  - Cập nhật số thuê bao đang gắn vào khe SIM của thiết bị. Khi chưa lắp SIM, giá trị mặc định là `ChuaGan`.
* `capNhatBTS(const std::string& bts)`:
  - Ghi nhận vị trí trạm thu phát sóng di động (Cell ID / Trạm BTS) gần nhất mà thiết bị đang phát tín hiệu định kỳ, phục vụ định vị và tối ưu hóa phủ sóng mạng viễn thông.

---

## 4. Bảng Phân Loại Họ Hàm (Function Family Taxonomy)

| Phân tầng kiến trúc | Tệp nguồn / Header | Lớp / Namespace | Họ hàm (Function Family) | Danh sách hàm chính | Mục đích kỹ thuật |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Foundation / Shared** | `Date.h`, `Date.cpp` | `Date` | Date Arithmetic & Validation | `isLeapYear`, `daysInMonth`, `isValid`, operators `<`, `==`, `>` | Xử lý ngày tháng chuẩn lịch Gregory, hỗ trợ năm nhuận |
| **Foundation / Shared** | `DisplayHelper.h` | `DisplayHelper` | Presentation & ASCII Layout | `printBorder`, `printHeader`, `printRow`, `printCard` | Xuất bảng và thẻ chi tiết chuẩn hóa trên Console |
| **Foundation / Shared** | `Normalized.h`, `Normalized.cpp` | `Normalized` | Sanitization & Validation Family | `trim`, `CollapseSpace`, `removeSymbols`, `NormalizedName`, `isValidPhoneNumber`, `toUpper` | Chuẩn hóa chuỗi họ tên, cắt khoảng trắng thừa, thẩm định định dạng số điện thoại viễn thông |
| **Foundation / Shared** | `Exceptions.h` | Global | Domain Error Handling | `ValidationException`, `NotFoundException`, `DuplicateException` | Xử lý lỗi theo mô hình ngoại lệ tường minh (C++ Exception) |
| **Presentation / Input** | `InputHelper.h`, `InputHelper.cpp` | `InputHelper` | Console Input Family | `clearBuffer`, `getString`, `getInt`, `getDouble`, `getDate`, `getBirthDate`, `getPhoneNumber`, `getConfirm`, `pause` | Trích xuất và kiểm soát nhập liệu an toàn từ bàn phím Console, chống trôi dòng `cin` |
| **Data Access Layer** | `Repository.h` | `Repository<T>` | Generic In-Memory CRUD | `add`, `update`, `remove`, `findById`, `filter`, `sort`, `saveToFile`, `loadFromFile` | Quản lý tập thực thể, tìm kiếm qua Lambda, ghi file an toàn (Atomic Write) |
| **Data Access Layer** | `BTSRegister.h`, `BTSRegister.cpp` | `BTSRegister` | Hardware Registry | `loadBTSData`, `isValidBTS`, `getBTSInfo`, `suggestNearestBTS` | Quản lý danh mục trạm phát sóng di động Việt Nam |
| **Domain Models** | `Entity.h` | `Entity` | Abstract Base Contract | `getId`, `setId`, `toFileString`, `fromFileString`, `display...` | Giao diện đa hình cho mọi thực thể nghiệp vụ |
| **Domain Models** | `hopdong.h`, `hopdong.cpp` | `HopDong` | Invariants & Lifecycle Mutators | `isExpired`, `giaHan`, `chamDut`, `tamDung`, `kichHoatLai` | Quản lý vòng đời hợp đồng cung cấp dịch vụ viễn thông |
| **Domain Models** | `imei.h`, `imei.cpp` | `ThietBiIMEI` | Telecom Invariants & EIR | `validateLuhn`, `isBlacklisted`, `setBlacklist`, `ganSIM`, `capNhatBTS` | Quản lý định danh thiết bị vô tuyến và danh sách đen EIR |
| **Controller / Menu** | `HopDongMenu.h`, `HopDongMenu.cpp` | `HopDongMenu` | Interactive Console Workflow | `displayMenu`, `handleSelection`, `themHopDong`, `giaHanHopDong`, `traCuuHopDong` | Điều hướng menu quản lý hợp đồng cho giao dịch viên |
| **Controller / Menu** | `ThietBiIMEIMenu.h`, `ThietBiIMEIMenu.cpp` | `ThietBiIMEIMenu` | Interactive Console Workflow | `displayMenu`, `handleSelection`, `themThietBi`, `khoaMoKhoaIMEI`, `traCuuTheoBTS` | Điều hướng menu quản lý thiết bị và trạm phát sóng |

---

## 5. Hướng Dẫn Vấn Đáp & Phản Biện Đồ Án (Defense Q&A Guide)

### Câu 1: Tại sao lớp `Entity` lại chứa các phương thức thuần ảo `displayHeader()`, `displayRow()`, `displayDetail()`?
* **Trả lời:**
  - `Entity` đóng vai trò là lớp cơ sở trừu tượng (Abstract Base Class). Việc khai báo các phương thức thuần ảo (`= 0`) định nghĩa một bản giao kèo (Interface Contract) bắt buộc mọi thực thể con phải cung cấp cơ chế biểu diễn dữ liệu của chính nó trên màn hình.
  - Điều này cho phép thực hiện **Tính đa hình (Polymorphism)**: Khi duyệt một danh sách các con trỏ `Entity*`, chương trình có thể gọi `entity->displayRow()` mà không cần quan tâm thực thể cụ thể là `HopDong` hay `ThietBiIMEI`.
  - Để tránh vi phạm nguyên lý đơn nhiệm (SRP), các lớp con không tự viết logic căn lề mà ủy nhiệm cho `DisplayHelper` xử lý định dạng.

### Câu 2: Thuật toán Luhn Mod-10 hoạt động như thế nào và tại sao cần áp dụng trong quản lý IMEI?
* **Trả lời:**
  - Thuật toán Luhn (Mod-10) là chuẩn quốc tế (ISO/IEC 7812 và 3GPP TS 22.016) nhằm kiểm tra lỗi nhập liệu thủ công (chữ số bị sai hoặc đảo vị trí liền kề).
  - Thuật toán nhân đôi các chữ số ở vị trí có thứ tự chẵn từ phải sang trái. Nếu kết quả nhân $\ge 10$, ta cộng hai chữ số của nó lại (bằng cách lấy giá trị trừ 9). Cuối cùng, tổng của tất cả các chữ số sau biến đổi phải chia hết cho 10.
  - Cài đặt hàm `static bool ThietBiIMEI::validateLuhn(const std::string& imeiStr)` chặn đứng các dữ liệu rác, đảm bảo tính hợp lệ trước khi lưu trữ vào hệ thống hoặc ghi xuống đĩa.

### Câu 3: Làm thế nào để đảm bảo tính an toàn dữ liệu (Atomic Persistence) khi ghi dữ liệu ra file?
* **Trả lời:**
  - Trong lớp `Repository<T>`, phương thức `saveToFile(filename)` không ghi đè trực tiếp lên tệp dữ liệu chính mà ghi ra một tệp tạm thời `filename + ".tmp"`.
  - Chỉ khi toàn bộ dữ liệu được ghi thành công và không phát sinh lỗi I/O, hệ thống mới tiến hành xóa tệp cũ và đổi tên tệp `.tmp` thành tệp chính thức.
  - Cơ chế này (Atomic Write) bảo vệ cơ sở dữ liệu không bị hỏng (corrupted) khi chương trình bị tắt đột ngột hoặc xảy ra sự cố mất điện giữa chừng.

### Câu 4: Phân hệ xử lý kiểm tra năm nhuận và ngày hợp lệ như thế nào?
* **Trả lời:**
  - Lớp `Date` cài đặt thuật toán kiểm tra năm nhuận theo lịch Gregory: `(year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)`.
  - Hàm `daysInMonth(month, year)` tự động xác định tháng 2 có 28 hay 29 ngày, các tháng khác có 30 hay 31 ngày.
  - Bất kỳ thao tác khởi tạo hoặc gán ngày nào không hợp lệ đều ném ngoại lệ `ValidationException` để thông báo lỗi rõ ràng cho người dùng.

### Câu 5: Tại sao cần tách rời `HopDong` và `ThietBiIMEI` khỏi `InputHelper`, và ranh giới giữa `Normalized::isValidPhoneNumber` với `InputHelper` là gì?
* **Trả lời:**
  - **Ranh giới phân tầng:** `InputHelper` thuộc Console Input Family (tầng Presentation / Input), có nhiệm vụ tương tác trực tiếp với người dùng qua bàn phím (`std::cin`), xóa cờ lỗi `failbit`, đọc dòng an toàn và lặp lại thông báo lỗi nếu nhập sai. Trong khi đó, `Normalized::isValidPhoneNumber` thuộc Sanitization & Validation Family, là hàm thuần túy (pure validation logic) kiểm tra định dạng và đầu số di động Việt Nam không mang hiệu ứng phụ (side effects).
  - **Tách rời kiến trúc (Decoupling):** Lớp mô hình (`HopDong`, `ThietBiIMEI`) là thực thể nghiệp vụ cốt lõi (Domain Entities). Việc tách rời hoàn toàn khỏi `InputHelper` giúp mô hình không bị phụ thuộc vào môi trường giao diện console. Khi khởi tạo từ file (`Repository::loadFromFile`) hay trong các bài kiểm thử tự động (`test_runner.cpp`), các thực thể này vẫn bảo vệ được các bất biến nghiệp vụ nhờ gọi trực tiếp `Normalized::isValidPhoneNumber` mà không kéo theo bất kỳ thư viện nhập liệu dòng lệnh nào.
