# KIẾN TRÚC HỆ THỐNG VÀ ĐẶC TẢ YÊU CẦU
## HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG (KTLT - PTIT)

---

## 1. Thông Tin Chung Đề Tài

* **Học viện:** Học viện Công nghệ Bưu chính Viễn thông (PTIT) — Cơ sở Hà Nội.
* **Khoa:** Khoa Viễn thông 1.
* **Học phần:** Kỹ thuật Lập trình (KTLT) — Sinh viên năm thứ 3, Năm học 2026–2027.
* **Đề tài:** Đề tài 1 — Hệ thống Quản lý Thuê bao Di động (*Mobile Subscriber Management System*).
* **Sinh viên thực hiện phân hệ:** Trần Đức Anh — MSSV: B24DCVT021 — Lớp: D24CQVT01-B.
* **Nhánh phát triển:** `DucAnh-B24DCVT021`.

---

## 2. Bối Cảnh Viễn Thông & Phân Công Nhiệm Vụ Nhóm

Hệ thống mô phỏng hệ sinh thái phần mềm quản trị nghiệp vụ và hỗ trợ vận hành viễn thông (BSS/OSS - *Business Support Systems / Operations Support Systems*) thu nhỏ của các nhà mạng viễn thông tại Việt Nam (như VNPT/VinaPhone, Viettel, MobiFone).

### 2.1 Bảng Phân Công 5 Thành Viên (10 Use Cases)

| STT | Họ và Tên | Mã Sinh Viên | Module Quản Lý | Use Cases Đảm Nhận |
| :---: | :--- | :---: | :--- | :--- |
| 1 | Nguyễn Tùng Dương | B24DCVT106 | `goicuoc.h/cpp` | Quản lý danh mục gói cước; Chuyển đổi Trả trước / Trả sau |
| 2 | Nguyễn Tất Thắng | B24DCVT331 | `thuebao.h/cpp` | Quản lý thông tin thuê bao; Đăng ký thông tin chính chủ & SIM |
| **3** | **Trần Đức Anh** | **B24DCVT021** | **`hopdong.h/cpp` & `imei.h/cpp`** | **UC01: Quản lý Hợp đồng đăng ký dịch vụ<br>UC02: Quản lý Thiết bị đầu cuối & IMEI (3GPP Luhn Checksum)** |
| 4 | Nguyễn Mạnh Dũng | B24DCVT094 | `naptien.h/cpp` | Quản lý nạp tiền, thẻ cào, lịch sử biến động số dư tài khoản |
| 5 | Đặng Việt Hùng | B24DCVT167 | `hoadon.h/cpp` | Lập hóa đơn cước hàng tháng; Báo cáo thống kê doanh thu |

---

## 3. Kiến Trúc Tổng Thể 3 Tầng

Hệ thống được thiết kế theo mô hình 3 tầng phân tách trách nhiệm (Separation of Concerns), tuân thủ nguyên lý Lập trình hướng đối tượng (OOP) thuần C++11 chuẩn mực:

```
+---------------------------------------------------------------------------------+
|                       TẦNG 1: TRÌNH DIỄN (PRESENTATION TIER)                    |
|  - main.cpp: Điều hướng Menu chính (0-9 Console Menu)                           |
|  - HopDongMenu.h/cpp: Điều hướng quản lý Hợp đồng đăng ký (CRUD, tìm kiếm, lọc) |
|  - ThietBiIMEIMenu.h/cpp: Điều hướng thiết bị đầu cuối IMEI, Blacklist, BTS     |
+---------------------------------------+-----------------------------------------+
                                        |
                                        v
+---------------------------------------------------------------------------------+
|                       TẦNG 2: NGHIỆP VỤ (BUSINESS DOMAIN TIER)                  |
|  - Entity.h: Lớp cơ sở trừu tượng (Abstract Base Class)                         |
|  - HopDong.h/cpp: Kế thừa Entity, tính hạn hợp đồng, chuyển trạng thái BSS     |
|  - ThietBiIMEI.h/cpp: Kế thừa Entity, xác thực thuật toán Luhn Mod-10 3GPP,      |
|    quản lý danh sách EIR Blacklist / Whitelist, gắn SIM và vị trí BTS           |
|  - Date.h/cpp: Xử lý lịch vạn niên, kiểm tra năm nhuận, so sánh mốc thời gian   |
|  - InputHelper.h/cpp: Chống trôi dòng cin, lọc buffer an toàn                   |
+---------------------------------------+-----------------------------------------+
                                        |
                                        v
+---------------------------------------------------------------------------------+
|                       TẦNG 3: LƯU TRỮ (PERSISTENCE DATA TIER)                   |
|  - DataStore<T>.h: Template thao tác file phẳng dạng Generic (Atomic Write)     |
|  - fileio.h/cpp: Quản lý đường dẫn, định dạng phân cách cột (|)                 |
|  - data/hopdong.txt: Lưu trữ dữ liệu hợp đồng viễn thông                        |
|  - data/imei.txt: Lưu trữ dữ liệu định danh phần cứng thiết bị IMEI             |
+---------------------------------------------------------------------------------+
```

---

## 4. Đặc Tả Yêu Cầu Chức Năng

### 4.1 Phân hệ Hợp đồng đăng ký (UC01 - `HopDong`)
* **FR-HD-01 (Thêm mới):** Tạo hợp đồng với mã hợp đồng duy nhất (`HDxxxx`), gắn kết `maKhachHang`, `soDienThoai`, `maGoiCuoc`, ngày đăng ký, ngày hết hạn, loại hợp đồng (`TraTruoc` / `TraSau`), và giá cước.
* **FR-HD-02 (Ràng buộc ngày):** `ngayHetHan >= ngayDangKy`. Ngày phải hợp lệ theo dương lịch (năm nhuận tháng 2 có 29 ngày, tháng 4/6/9/11 có 30 ngày).
* **FR-HD-03 (Duyệt & Hiển thị):** Hiển thị danh sách hợp đồng dạng bảng định dạng căn lề chuẩn bằng `<iomanip>`, hiển thị rõ trạng thái (`HieuLuc`, `TamDung`, `ThanhLy`).
* **FR-HD-04 (Tìm kiếm đa tiêu chí):** Tra cứu theo Mã hợp đồng (chính xác), Số điện thoại thuê bao (10 số), hoặc Mã khách hàng (liệt kê toàn bộ hợp đồng của một chủ thể).
* **FR-HD-05 (Sắp xếp):** Sắp xếp danh sách theo ngày đăng ký (mới nhất đến cũ nhất) hoặc theo giá trị cước (tăng/giảm dần).
* **FR-HD-06 (Cập nhật & Chấm dứt):** Cho phép gia hạn hợp đồng, đổi gói cước, hoặc thanh lý/chấm dứt hợp đồng.
* **FR-HD-07 (Xóa an toàn):** Yêu cầu xác nhận (y/n) trước khi xóa khỏi bộ nhớ và đồng bộ xuống tệp tin.

### 4.2 Phân hệ Thiết bị đầu cuối & IMEI (UC02 - `ThietBiIMEI`)
* **FR-IM-01 (Xác thực IMEI 3GPP):** Kiểm tra độ dài chuẩn 15 chữ số. Bắt buộc áp dụng thuật toán **Luhn Mod-10**; từ chối lưu trữ và cảnh báo nếu mã sai checksum.
* **FR-IM-02 (Quản lý thiết bị):** Lưu tên thiết bị (VD: iPhone 15 Pro, Galaxy S24), hãng sản xuất (Apple, Samsung, Xiaomi), số thuê bao đang gắn, ngày kích hoạt, trạng thái thiết bị, và trạm thu phát sóng di động gắn kết (`tramBTSGanNhat`).
* **FR-IM-03 (Quản trị EIR Blacklist / Khóa mạng):** Hỗ trợ tra cứu nhanh danh sách thiết bị bị báo mất hoặc gian lận (`KhoaMang`). Cho phép nhà mạng khóa mạng khẩn cấp hoặc mở khóa thiết bị khi xác minh thành công.
* **FR-IM-04 (Truy vết vị trí trạm BTS):** Cập nhật mã trạm BTS (`BTS-HN-001`, `BTS-DN-005`...) khi thuê bao di chuyển vùng phủ sóng.
* **FR-IM-05 (Đổi SIM phần cứng):** Cho phép gỡ số thuê bao khỏi thiết bị hoặc gán số điện thoại mới khi khách hàng đổi máy.

---

## 5. Đặc Tả Yêu Cầu Phi Chức Năng

1. **Chuẩn mã nguồn & Thư viện:**
   - 100% C++11 tiêu chuẩn (ISO/IEC 14882:2011).
   - Chỉ sử dụng Thư viện chuẩn (`<vector>`, `<string>`, `<fstream>`, `<sstream>`, `<iomanip>`, `<algorithm>`, `<iostream>`).
   - Tuyệt đối không phụ thuộc vào thư viện ngoài, không sử dụng framework nặng.
2. **An toàn luồng dữ liệu (Input Stream Safety):**
   - Loại trừ hoàn toàn hiện tượng lặp vô tận (infinite loop) khi người dùng nhập sai kiểu dữ liệu (nhập ký tự chữ vào biến số).
   - Bắt và xử lý sự kiện ngắt kết thúc luồng `EOF` (`Ctrl+D` / `Ctrl+Z`).
3. **Toàn vẹn tệp tin (Atomic File Persistence):**
   - Không làm mất mát hay phân mảnh dữ liệu khi gặp sự cố ngắt nguồn giữa chừng: áp dụng quy trình ghi ra tệp tạm `.tmp` sau đó đổi tên ghi đè tệp chính thức.
   - Định dạng tệp tin phân tách cột bằng dấu sổ đứng `|` (`pipe-delimited`), tự động bỏ qua dòng trống hoặc dòng chú thích bắt đầu bằng `#`.
4. **Hiệu năng & Tài nguyên:**
   - Tốc độ tải dữ liệu từ đĩa vào RAM và tìm kiếm tuyến tính/nhị phân tức thời (< 50ms cho tập mẫu hàng ngàn bản ghi).
   - Tối ưu bộ nhớ: Giải phóng tài nguyên đối tượng đúng cách, không rò rỉ bộ nhớ (zero memory leak).
