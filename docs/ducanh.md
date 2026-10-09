# TÀI LIỆU HƯỚNG DẪN KỸ THUẬT PHÂN HỆ
## SINH VIÊN THỰC HIỆN: TRẦN ĐỨC ANH — MSSV: B24DCVT021
### LỚP: D24CQVT01-B | KHOA VIỄN THÔNG 1 — PTIT (SINH VIÊN NĂM THỨ 3, NĂM HỌC 2026–2027)
### ĐỀ TÀI: QUẢN LÝ THUÊ BAO DI ĐỘNG | PHÂN HỆ: HỢP ĐỒNG (UC01) & THIẾT BỊ IMEI (UC02)

---

## 1. Tổng Quan Nhiệm Vụ Được Phân Công

Trong đồ án nhóm Kỹ thuật Lập trình (5 thành viên, 10 use cases), sinh viên **Trần Đức Anh** phụ trách 2 phân hệ trung tâm liên kết giữa khách hàng, dịch vụ cước và mạng lưới viễn thông:

1. **UC01: Quản lý Hợp đồng đăng ký dịch vụ (`HopDong`):**
   * Quản lý vòng đời hợp đồng cung cấp dịch vụ viễn thông giữa khách hàng và nhà mạng.
   * Xử lý thời hạn, trạng thái (`HieuLuc`, `TamDung`, `ThanhLy`), phân loại thuê bao (`TraTruoc` / `TraSau`), gia hạn và thanh lý.
2. **UC02: Quản lý Thiết bị đầu cuối & IMEI (`ThietBiIMEI`):**
   * Quản lý mã nhận dạng thiết bị di động quốc tế (IMEI - *International Mobile Station Equipment Identity*) gồm 15 chữ số.
   * Cài đặt và kiểm soát thuật toán **Luhn Checksum (Mod-10)** theo tiêu chuẩn quốc tế **3GPP TS 22.016 / 23.003**.
   * Quản trị phân hệ **EIR (Equipment Identity Register)**: Quản lý danh sách đen (`KhoaMang` - Blacklist) đối với máy báo mất/trộm cắp và truy vết trạm thu phát sóng di động gắn kết (`tramBTSGanNhat`).

---

## 2. Sơ Đồ Mối Quan Hệ Dữ Liệu Trong Hệ Thống Viễn Thông

```
                   +------------------------+
                   |  KhachHang / ThueBao   |
                   | (Thắng - B24DCVT331)   |
                   +-----------+------------+
                               | 1
                               |
                               | tham chiếu (soDienThoai)
             +-----------------+-----------------+
             | 1..*                              | 0..1
             v                                   v
+------------------------+             +------------------------+
|        HopDong         |             |      ThietBiIMEI       |
| (Đức Anh - B24DCVT021) |             | (Đức Anh - B24DCVT021) |
+-----------+------------+             +-----------+------------+
            | 1..*                                 | 1..*
            | tham chiếu (maGoiCuoc)               | phát sóng kết nối
            v                                      v
+------------------------+             +------------------------+
|        GoiCuoc         |             |        Tram BTS        |
| (Dương - B24DCVT106)   |             |    (Vị trí trạm sóng)  |
+------------------------+             +------------------------+
```

* **Ý nghĩa:**
  - `HopDong` đại diện cho quan hệ pháp lý - kinh tế: Khách hàng mua gói cước nào, thời hạn bao lâu, cước phí bao nhiêu.
  - `ThietBiIMEI` đại diện cho quan hệ vật lý - kỹ thuật mạng: Thiết bị phần cứng nào đang gắn SIM nào, phát tín hiệu tới trạm BTS nào, có bị nhà mạng khóa máy hay không.

---

## 3. Giải Thích Chi Tiết Thuật Toán Luhn Mod-10

### 3.1 Cấu Trúc Mã IMEI 15 Chữ Số Chuẩn 3GPP
$$\text{IMEI} = \underbrace{d_1 d_2 d_3 d_4 d_5 d_6 d_7 d_8}_{\text{TAC (Type Allocation Code)}} \underbrace{d_9 d_{10} d_{11} d_{12} d_{13} d_{14}}_{\text{SNR (Serial Number)}} \underbrace{d_{15}}_{\text{CD (Check Digit)}}$$

### 3.2 Quy Tắc Tính Checksum Luhn Mod-10
1. Đánh số vị trí từ phải qua trái: $i = 1$ (chữ số $d_{15}$) đến $i = 15$ (chữ số $d_1$).
2. Đối với các chữ số ở vị trí có thứ tự chẵn từ phải sang (tức $i = 2, 4, 6, 8, 10, 12, 14$):
   * Nhân đôi giá trị: $x = 2 \times d$.
   * Nếu $x > 9$, cộng các chữ số lại (hoặc lấy $x - 9$). Ví dụ: $7 \times 2 = 14 \to 1 + 4 = 5$ (hoặc $14 - 9 = 5$).
3. Đối với các chữ số ở vị trí lẻ từ phải sang (tức $i = 1, 3, 5, 7, 9, 11, 13, 15$):
   * Giữ nguyên giá trị chữ số.
4. Tính tổng $S$ của tất cả 15 giá trị sau khi biến đổi.
5. **Điều kiện hợp lệ:** $S \pmod{10} == 0$.

### 3.3 Ví Dụ Tính Toán Cụ Thể (Mã: `860123456789014`)

| Vị trí (trái $\to$ phải) | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 (Check Digit) |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| Chữ số gốc | **8** | **6** | **0** | **1** | **2** | **3** | **4** | **5** | **6** | **7** | **8** | **9** | **0** | **1** | **4** |
| Nhân đôi vị trí chẵn | $8$ | $6 \times 2$ | $0$ | $1 \times 2$ | $2$ | $3 \times 2$ | $4$ | $5 \times 2$ | $6$ | $7 \times 2$ | $8$ | $9 \times 2$ | $0$ | $1 \times 2$ | $4$ |
| Kết quả biến đổi | $8$ | $3$ ($12 \to 3$) | $0$ | $2$ | $2$ | $6$ | $4$ | $1$ ($10 \to 1$) | $6$ | $5$ ($14 \to 5$) | $8$ | $9$ ($18 \to 9$) | $0$ | $2$ | $4$ |

* **Tổng cộng dồn:** $S = 8 + 3 + 0 + 2 + 2 + 6 + 4 + 1 + 6 + 5 + 8 + 9 + 0 + 2 + 4 = 60$.
* $60 \pmod{10} = 0 \implies$ Mã IMEI hoàn toàn hợp lệ.

Trong mã nguồn, hàm `ThietBiIMEI::validateLuhn(const std::string& imei)` cài đặt thuật toán tự động kiểm tra chính xác 100%.

---

## 4. Giải Thích Các Hàm & Thành Phần Trong Codebase

### 4.1 Lớp `Entity`
* `getId()`, `setId()`: Quản lý khóa chính của bản ghi.
* `toFileString()`: Đóng gói các thuộc tính thành định dạng chuỗi `col1|col2|col3...` để ghi file.
* `fromFileString(line)`: Tách chuỗi theo dấu `|` và khôi phục dữ liệu vào các biến thành viên.
* `displayHeader()`, `displayRow()`, `displayDetail()`: Định dạng in bảng đẹp mắt bằng `<iomanip>`.

### 4.2 Lớp `DataStore<T>`
* Quản lý `std::vector<T>` trong bộ nhớ RAM.
* `loadFromFile()`: Đọc dữ liệu từ file đĩa vào RAM khi khởi động.
* `saveToFile()`: Ghi an toàn thông qua tệp `.tmp` rồi đổi tên (Atomic Write) tránh hỏng dữ liệu khi mất điện.
* `add()`, `update()`, `remove()`, `findById()`: Các thao tác CRUD chuẩn mực.
* `filter()`, `sort()`: Sử dụng con trỏ hàm / Lambda function của C++11 để lọc và sắp xếp dữ liệu linh hoạt.

### 4.3 Lớp `Date`
* `isLeapYear(year)`: Kiểm tra năm nhuận theo quy tắc dương lịch: `(year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)`.
* `daysInMonth(month, year)`: Trả về số ngày tối đa của tháng (xử lý tháng 2 năm nhuận có 29 ngày).
* Nạp chồng toán tử so sánh (`<`, `==`, `<=`) để kiểm tra logic: Ngày hết hạn $\ge$ Ngày đăng ký.

### 4.4 Thư Viện Tiện Ích `DisplayHelper`
* Đóng gói toàn bộ cơ chế vẽ bảng ASCII (`+---+---+`) và thẻ chi tiết vào namespace `DisplayHelper`.
* Loại bỏ mã lặp `<iomanip>` trong `HopDong` và `ThietBiIMEI`, đảm bảo tất cả các bảng dữ liệu trong hệ thống có cùng chuẩn căn chỉnh và lề.

### 4.5 Phân Định Họ Hàm: `Normalized` (Validation) & `InputHelper` (Console Input)
* **Sanitization & Validation Family (`Normalized`):**
  - Đóng gói các hàm thuần túy (pure functions) xử lý chuỗi và thẩm định dữ liệu không phát sinh hiệu ứng phụ (side effects).
  - Hàm `Normalized::isValidPhoneNumber(phone, allowUnassigned)` kiểm tra nghiêm ngặt quy chuẩn thuê bao di động Việt Nam: đúng 10 chữ số, bắt đầu bằng `0`, và thuộc các dải đầu số hợp lệ (`03`, `05`, `07`, `08`, `09`).
  - Namespace `Normalized` còn cung cấp các tiện ích làm sạch: `trim`, `CollapseSpace`, `removeSymbols` (ngăn chặn chèn ký tự phân tách `|`), `NormalizedName`, `toUpper`, `toLower`.
* **Console Input Family (`InputHelper`):**
  - Đóng vai trò là tầng tiện ích nhập liệu dòng lệnh (Terminal Console UI).
  - Quản lý luồng nhập `std::cin`, xử lý lỗi bộ đệm (`cin.clear()`, `cin.ignore()`), kiểm tra giới hạn min/max, và duy trì vòng lặp hỏi lại cho đến khi người dùng nhập đúng cú pháp.
* **Tách rời mô hình khỏi phân hệ nhập liệu (Decoupling Architecture):**
  - Các lớp thực thể cốt lõi (`HopDong`, `ThietBiIMEI`) thuộc tầng Domain Layer hoàn toàn không phụ thuộc vào `InputHelper`.
  - Mọi thao tác kiểm tra tính toàn vẹn của số thuê bao khi tạo hợp đồng (`HopDong::HopDong`), cập nhật số thuê bao (`HopDong::setSoDienThoai`), hoặc gán SIM vào thiết bị (`ThietBiIMEI::ganSIM`) đều gọi trực tiếp `Normalized::isValidPhoneNumber`.
  - Nhờ đó, các thực thể dữ liệu có thể vận hành độc lập, dễ dàng kiểm thử tự động (`test_runner.cpp`), nạp từ tệp tin qua `DataStore<T>`, và không bị gắn chặt vào bất kỳ môi trường console cụ thể nào.

---

## 5. Quy Ước Đặt Tên & Ngôn Ngữ Trong Mã Nguồn

Dự án áp dụng quy ước ngôn ngữ phân tầng rõ ràng:

1. **Tên tệp tin thực thể & lưu trữ (Tiếng Việt):**
   * `src/lib/models/hopdong.h/cpp`, `src/lib/models/imei.h/cpp`, `data/hopdong.txt`, `data/imei.txt`.
   * Khớp 1:1 với tên module phân công trong đề tài BTL KTLT của Học viện PTIT giữa 5 thành viên.
2. **Tên phương thức hạ tầng & thuật toán (Tiếng Anh):**
   * `getId()`, `setId()`, `isExpired()`, `validateLuhn()`, `isBlacklisted()`, `setBlacklist()`.
   * `toFileString()`, `fromFileString()`, `loadFromFile()`, `saveToFile()`.
   * Tuân thủ quy chuẩn thiết kế C++ OOP, DataStore pattern và thuật toán quốc tế 3GPP.
3. **Tên hàm điều hướng giao diện (Tiếng Việt):**
   * `themHopDong()`, `xemDanhSach()`, `timKiemHopDong()`, `sapXepDanhSach()`, `capNhatHopDong()`, `xoaHopDong()`, `ganSIM()`, `goSIM()`, `capNhatBTS()`.
   * Phản ánh trực quan quy trình nghiệp vụ viễn thông tại các điểm giao dịch trong nước.

---

## 6. Bảng Phân Loại Họ Hàm (Function Family Taxonomy)

| Phân tầng kiến trúc | Tệp nguồn / Header | Lớp / Namespace | Họ hàm (Function Family) | Danh sách hàm chính | Mục đích kỹ thuật |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **Foundation / Shared** | `Date.h`, `Date.cpp` | `Date` | Date Arithmetic & Validation | `isLeapYear`, `daysInMonth`, `isValid`, operators `<`, `==`, `>` | Xử lý ngày tháng chuẩn lịch Gregory, hỗ trợ năm nhuận |
| **Foundation / Shared** | `DisplayHelper.h` | `DisplayHelper` | Presentation & ASCII Layout | `printBorder`, `printHeader`, `printRow`, `printCard` | Xuất bảng và thẻ chi tiết chuẩn hóa trên Console |
| **Foundation / Shared** | `Normalized.h`, `Normalized.cpp` | `Normalized` | Sanitization & Validation Family | `trim`, `CollapseSpace`, `removeSymbols`, `NormalizedName`, `isValidPhoneNumber`, `toUpper` | Chuẩn hóa chuỗi họ tên, cắt khoảng trắng thừa, thẩm định định dạng số điện thoại viễn thông |
| **Foundation / Shared** | `Exceptions.h` | Global | Domain Error Handling | `ValidationException`, `NotFoundException`, `DuplicateException` | Xử lý lỗi theo mô hình ngoại lệ tường minh (C++ Exception) |
| **Presentation / Input** | `InputHelper.h`, `InputHelper.cpp` | `InputHelper` | Console Input Family | `clearBuffer`, `getString`, `getInt`, `getDouble`, `getDate`, `getBirthDate`, `getPhoneNumber`, `getConfirm`, `pause` | Trích xuất và kiểm soát nhập liệu an toàn từ bàn phím Console, chống trôi dòng `cin` |
| **Data Access Layer** | `DataStore.h` | `DataStore<T>` | Generic In-Memory CRUD | `add`, `update`, `remove`, `findById`, `filter`, `sort`, `saveToFile`, `loadFromFile` | Quản lý tập thực thể, tìm kiếm qua Lambda, ghi file an toàn (Atomic Write) |
| **Data Access Layer** | `BTSRegister.h`, `BTSRegister.cpp` | `BTSRegister` | Hardware Registry | `loadBTSData`, `isValidBTS`, `getBTSInfo`, `suggestNearestBTS` | Quản lý danh mục trạm phát sóng di động Việt Nam |
| **Domain Models** | `Entity.h` | `Entity` | Abstract Base Contract | `getId`, `setId`, `toFileString`, `fromFileString`, `display...` | Giao diện đa hình cho mọi thực thể nghiệp vụ |
| **Domain Models** | `hopdong.h`, `hopdong.cpp` | `HopDong` | Invariants & Lifecycle Mutators | `isExpired`, `giaHan`, `chamDut`, `tamDung`, `kichHoatLai` | Quản lý vòng đời hợp đồng cung cấp dịch vụ viễn thông |
| **Domain Models** | `imei.h`, `imei.cpp` | `ThietBiIMEI` | Telecom Invariants & EIR | `validateLuhn`, `isBlacklisted`, `setBlacklist`, `ganSIM`, `capNhatBTS` | Quản lý định danh thiết bị vô tuyến và danh sách đen EIR |
| **Controller / Menu** | `HopDongMenu.h`, `HopDongMenu.cpp` | `HopDongMenu` | Interactive Console Workflow | `displayMenu`, `handleSelection`, `themHopDong`, `giaHanHopDong`, `traCuuHopDong` | Điều hướng menu quản lý hợp đồng cho giao dịch viên |
| **Controller / Menu** | `ThietBiIMEIMenu.h`, `ThietBiIMEIMenu.cpp` | `ThietBiIMEIMenu` | Interactive Console Workflow | `displayMenu`, `handleSelection`, `themThietBi`, `khoaMoKhoaIMEI`, `traCuuTheoBTS` | Điều hướng menu quản lý thiết bị và trạm phát sóng |

---

## 7. Hướng Dẫn Vấn Đáp & Phản Biện Đồ Án (Defense Q&A Guide)

### Câu 1: Tại sao lớp `Entity` lại chứa các phương thức thuần ảo `displayHeader()`, `displayRow()`, `displayDetail()`?
* **Trả lời:**
  - `Entity` là lớp cơ sở trừu tượng (Abstract Base Class). Việc khai báo các phương thức thuần ảo (`= 0`) tạo hợp đồng giao diện bắt buộc các thực thể con phải triển khai cách hiển thị dữ liệu của chính mình.
  - Hỗ trợ **Tính đa hình (Polymorphism)**: Có thể duyệt qua danh sách con trỏ `Entity*` để hiển thị dữ liệu bảng mà không cần ép kiểu thủ công.
  - Để đảm bảo nguyên lý đơn nhiệm (SRP), các thực thể con không tự xử lý căn lề hay vẽ đường kẻ viền mà ủy nhiệm hoàn toàn cho `DisplayHelper`.

### Câu 2: Thuật toán Luhn Mod-10 kiểm tra tính hợp lệ của IMEI ra sao?
* **Trả lời:**
  - Tiêu chuẩn quốc tế 3GPP TS 22.016 quy định mã IMEI gồm 15 chữ số, chữ số thứ 15 là Check Digit (CD).
  - Thuật toán nhân đôi các chữ số ở vị trí chẵn từ phải sang trái (vị trí 2, 4, 6, 8, 10, 12, 14). Nếu kết quả nhân $\ge 10$, trừ 9 để thu được tổng 2 chữ số. Giữ nguyên các chữ số ở vị trí lẻ.
  - Tổng của tất cả 15 chữ số sau biến đổi phải chia hết cho 10 ($S \pmod{10} == 0$).
  - Hàm `ThietBiIMEI::validateLuhn` tự động phát hiện các lỗi nhập sai chữ số đơn lẻ hoặc đảo vị trí 2 chữ số liền kề.

### Câu 3: Làm thế nào để đảm bảo tính an toàn dữ liệu (Atomic Persistence) khi ghi dữ liệu ra file?
* **Trả lời:**
  - Trong lớp `DataStore<T>`, phương thức `saveToFile(filename)` không ghi đè trực tiếp lên tệp dữ liệu chính mà ghi ra một tệp tạm thời `filename + ".tmp"`.
  - Chỉ khi toàn bộ dữ liệu được ghi thành công và không phát sinh lỗi I/O, hệ thống mới tiến hành xóa tệp cũ và đổi tên tệp `.tmp` thành tệp chính thức (`std::rename`).
  - Cơ chế này (Atomic Write) bảo vệ cơ sở dữ liệu không bị hỏng (corrupted) khi chương trình bị tắt đột ngột hoặc xảy ra sự cố mất điện giữa chừng.

### Câu 4: Phân hệ xử lý kiểm tra năm nhuận và ngày hợp lệ như thế nào?
* **Trả lời:**
  - Lớp `Date` cài đặt thuật toán kiểm tra năm nhuận theo lịch Gregory: `(year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)`.
  - Hàm `daysInMonth(month, year)` tự động xác định tháng 2 có 28 hay 29 ngày, các tháng khác có 30 hay 31 ngày.
  - Bất kỳ thao tác khởi tạo hoặc gán ngày nào không hợp lệ đều ném ngoại lệ `ValidationException` để thông báo lỗi rõ ràng cho người dùng.

### Câu 5: Tại sao cần tách rời `HopDong` và `ThietBiIMEI` khỏi `InputHelper`, và phân định vai trò giữa `Normalized::isValidPhoneNumber` với `InputHelper` thế nào?
* **Trả lời:**
  - **Phân định ranh giới:** `InputHelper` thuộc Console Input Family (tầng giao diện người dùng), phụ trách việc đọc dòng từ `std::cin`, xử lý lỗi bộ đệm và lặp lại nhắc lệnh. Ngược lại, `Normalized::isValidPhoneNumber` thuộc Sanitization & Validation Family, là hàm thuần túy (pure function) xác thực định dạng và đầu số viễn thông Việt Nam độc lập với môi trường dòng lệnh.
  - **Tách rời kiến trúc (Decoupling):** Các lớp mô hình nghiệp vụ (`HopDong`, `ThietBiIMEI`) là thực thể dữ liệu cốt lõi (Domain Layer). Việc loại bỏ hoàn toàn sự phụ thuộc vào `InputHelper` giúp mô hình dữ liệu giữ nguyên tính đóng gói, dễ dàng kiểm thử tự động (`test_runner.cpp`), nạp từ tệp tin (`DataStore::loadFromFile`), và không bị ràng buộc vào giao diện dòng lệnh.
