# TÀI LIỆU HƯỚNG DẪN KỸ THUẬT PHÂN HỆ
## SINH VIÊN THỰC HIỆN: TRẦN ĐỨC ANH — MSSV: B24DCVT021
### LỚP: D24CQVT01-B | KHOA VIỄN THÔNG 1 — PTIT
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

### 4.2 Lớp `Repository<T>`
* Quản lý `std::vector<T>` trong bộ nhớ RAM.
* `loadFromFile()`: Đọc dữ liệu từ file đĩa vào RAM khi khởi động.
* `saveToFile()`: Ghi an toàn thông qua tệp `.tmp` rồi đổi tên (Atomic Write) tránh hỏng dữ liệu khi mất điện.
* `add()`, `update()`, `remove()`, `findById()`: Các thao tác CRUD chuẩn mực.
* `filter()`, `sort()`: Sử dụng con trỏ hàm / Lambda function của C++11 để lọc và sắp xếp dữ liệu linh hoạt.

### 4.3 Lớp `Date`
* `isLeapYear(year)`: Kiểm tra năm nhuận theo quy tắc dương lịch: `(year % 400 == 0) || (year % 4 == 0 && year % 100 != 0)`.
* `daysInMonth(month, year)`: Trả về số ngày tối đa của tháng (xử lý tháng 2 năm nhuận có 29 ngày).
* Nạp chồng toán tử so sánh (`<`, `==`, `<=`) để kiểm tra logic: Ngày hết hạn $\ge$ Ngày đăng ký.

---

## 5. Quy Ước Đặt Tên & Ngôn Ngữ Trong Mã Nguồn

Dự án áp dụng quy ước ngôn ngữ phân tầng rõ ràng:

1. **Tên tệp tin thực thể & lưu trữ (Tiếng Việt):**
   * `src/lib/hopdong.h/cpp`, `src/lib/imei.h/cpp`, `data/hopdong.txt`, `data/imei.txt`.
   * Khớp 1:1 với tên module phân công trong đề tài BTL KTLT của Học viện PTIT giữa 5 thành viên.
2. **Tên phương thức hạ tầng & thuật toán (Tiếng Anh):**
   * `getId()`, `setId()`, `isExpired()`, `validateLuhn()`, `isBlacklisted()`, `setBlacklist()`.
   * `toFileString()`, `fromFileString()`, `loadFromFile()`, `saveToFile()`.
   * Tuân thủ quy chuẩn thiết kế C++ OOP, Repository pattern và thuật toán quốc tế 3GPP.
3. **Tên hàm điều hướng giao diện (Tiếng Việt):**
   * `themHopDong()`, `xemDanhSach()`, `timKiemHopDong()`, `sapXepDanhSach()`, `capNhatHopDong()`, `xoaHopDong()`, `ganSIM()`, `goSIM()`, `capNhatBTS()`.
   * Phản ánh trực quan quy trình nghiệp vụ viễn thông tại các điểm giao dịch trong nước.
