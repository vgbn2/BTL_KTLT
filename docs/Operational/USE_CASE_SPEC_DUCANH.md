# ĐẶC TẢ CHI TIẾT USE CASE
## SINH VIÊN: TRẦN ĐỨC ANH — MSSV: B24DCVT021
### PHÂN HỆ: QUẢN LÝ HỢP ĐỒNG (UC01) & THIẾT BỊ ĐẦU CUỐI IMEI (UC02)

---

## 1. Phân Hệ UC01: Quản Lý Hợp Đồng Đăng Ký (`HopDong`)

### 1.1 UC01.1: Thêm Hợp Đồng Mới
* **Mục tiêu:** Tạo và lưu trữ một hợp đồng cung cấp dịch vụ viễn thông mới vào hệ thống.
* **Tác nhân:** Giao dịch viên viễn thông / Quản trị viên hệ thống.
* **Tiền điều kiện:** Hệ thống đã tải dữ liệu hợp đồng hiện có vào bộ nhớ.
* **Luồng sự kiện chính:**
  1. Người dùng chọn chức năng `[1] Them hop dong moi`.
  2. Hệ thống yêu cầu nhập `Ma hop dong` (Định dạng: `HDxxxx`).
     * *Ngoại lệ 1a:* Nếu mã hợp đồng đã tồn tại trong hệ thống, thông báo lỗi và yêu cầu nhập lại hoặc hủy.
  3. Hệ thống yêu cầu nhập `Ma khach hang` (Định dạng: `KHxxxx`).
  4. Hệ thống yêu cầu nhập `So dien thoai` (Chuỗi 10 chữ số).
  5. Hệ thống yêu cầu nhập `Ma goi cuoc` (Định dạng: `GCxxxx` hoặc tên gói: `VD149`, `D159V`).
  6. Hệ thống yêu cầu nhập `Ngay dang ky` (Định dạng `DD/MM/YYYY`). Kiểm tra ngày dương lịch hợp lệ.
  7. Hệ thống yêu cầu nhập `Ngay het han` (Định dạng `DD/MM/YYYY`). Kiểm tra ngày hợp lệ và $\ge$ ngày đăng ký.
  8. Hệ thống yêu cầu chọn `Loai hop dong`:
     - Nhập `1`: Trả trước (`TraTruoc`)
     - Nhập `2`: Trả sau (`TraSau`)
  9. Hệ thống yêu cầu nhập `Gia tri goi cuoc` (Số thực $\ge 0$).
  10. Hệ thống đóng gói dữ liệu, thêm vào vector quản lý và đồng bộ ghi xuống tệp `data/hopdong.txt`.
  11. Hệ thống in thông báo thêm thành công và quay lại menu hợp đồng.

### 1.2 UC01.2: Xem Danh Sách Hợp Đồng
* **Mục tiêu:** Hiển thị toàn bộ hợp đồng hiện có trong cơ sở dữ liệu dưới dạng bảng định dạng trực quan.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[2] Xem danh sach hop dong`.
  2. Hệ thống kiểm tra số lượng bản ghi:
     * Nếu danh sách rỗng, hiển thị thông báo "Danh sach hop dong hien dang trong!".
     * Nếu có dữ liệu, hiển thị bảng kẻ đường viền với các cột: `Ma HD`, `Ma KH`, `So Dien Thoai`, `Ma Goi`, `Ngay DK`, `Ngay HH`, `Loai HD`, `Trang Thai`, `Gia Tri (VND)`.
  3. Nhấn phím bất kỳ để tiếp tục.

### 1.3 UC01.3: Tìm Kiếm Hợp Đồng
* **Mục tiêu:** Tra cứu nhanh thông tin hợp đồng theo các tiêu chí khác nhau.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[3] Tim kiem hop dong`.
  2. Hệ thống hiển thị 3 lựa chọn tra cứu:
     - `[1] Tim theo Ma hop dong (Chinh xac)`: Tìm kiếm bản ghi có mã trùng khớp tuyệt đối.
     - `[2] Tim theo So dien thoai`: Tìm kiếm tất cả hợp đồng gắn với số thuê bao này.
     - `[3] Tim theo Ma khach hang`: Tìm kiếm toàn bộ hợp đồng mà khách hàng đã từng ký kết.
  3. Hiển thị kết quả tìm kiếm chi tiết dưới dạng bảng hoặc thông báo không tìm thấy.

### 1.4 UC01.4: Sắp Xếp Danh Sách Hợp Đồng
* **Mục tiêu:** Sắp xếp danh sách trong bộ nhớ phục vụ công tác tra cứu và thống kê.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[4] Sap xep danh sach`.
  2. Người dùng chọn tiêu chí sắp xếp:
     - `[1] Sap xep theo Ngay dang ky`: Mới nhất đến cũ nhất (hoặc ngược lại).
     - `[2] Sap xep theo Gia tri goi cuoc`: Giảm dần (hoặc tăng dần).
  3. Hệ thống áp dụng thuật toán `std::sort` kết hợp hàm so sánh Lambda chuẩn.
  4. Hiển thị lại bảng danh sách đã được sắp xếp.

### 1.5 UC01.5: Cập Nhật Thông Tin Hợp Đồng
* **Mục tiêu:** Sửa đổi thông tin gói cước, gia hạn thời gian hiệu lực, hoặc chuyển trạng thái hợp đồng.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[5] Cap nhat thong tin hop dong`.
  2. Nhập mã hợp đồng cần sửa. Nếu không tìm thấy, báo lỗi.
  3. Hiển thị thông tin hiện tại của hợp đồng.
  4. Cho phép sửa từng trường (Nhấn phím Enter để giữ nguyên giá trị cũ):
     - Đổi mã gói cước mới.
     - Gia hạn ngày hết hạn mới.
     - Cập nhật trạng thái: `1. HieuLuc`, `2. TamDung`, `3. ThanhLy`.
  5. Cập nhật đối tượng và ghi đè an toàn xuống tệp `data/hopdong.txt`.

### 1.6 UC01.6: Xóa / Thanh Lý Hợp Đồng
* **Mục tiêu:** Xóa hợp đồng khỏi hệ thống dữ liệu.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[6] Xoa hop dong`.
  2. Nhập mã hợp đồng cần xóa. Nếu không tìm thấy, báo lỗi.
  3. Hệ thống hiển thị thông tin bản ghi và đưa ra cảnh báo: `Ban co chac chan muon xoa hop dong nay? (y/n): `.
  4. Nếu người dùng chọn `y`, loại bỏ khỏi vector và cập nhật tệp `data/hopdong.txt`. Ngược lại hủy thao tác.

---

## 2. Phân Hệ UC02: Quản Lý Thiết Bị Đầu Cuối & IMEI (`ThietBiIMEI`)

### 2.1 UC02.1: Thêm Thiết Bị Mới (Xác Thực Thuật Toán Luhn 3GPP)
* **Mục tiêu:** Tiếp nhận và đăng ký thiết bị phần cứng di động vào mạng viễn thông.
* **Tác nhân:** Kỹ sư mạng viễn thông / Quản trị viên thiết bị.
* **Luồng sự kiện chính:**
  1. Người dùng chọn `[1] Them thiet bi moi`.
  2. Hệ thống yêu cầu nhập `Ma IMEI (15 chu so)`.
     * *Kiểm tra 2a:* Độ dài khác 15 chữ số hoặc chứa ký tự không phải số $\to$ báo lỗi định dạng.
     * *Kiểm tra 2b (Thuật toán Luhn Mod-10):* Tính toán checksum. Nếu tổng không chia hết cho 10 $\to$ từ chối: `Loi: Ma IMEI khong hop le theo chuan 3GPP (Sai ma kiem tra Luhn)!`.
     * *Kiểm tra 2c:* Trùng mã IMEI đã tồn tại $\to$ báo lỗi trùng lặp.
  3. Yêu cầu nhập `Ten thiet bi` (VD: `iPhone 15 Pro Max`).
  4. Yêu cầu nhập `Hang san xuat` (VD: `Apple`).
  5. Yêu cầu nhập `So dien thoai gan kem` (Nếu chưa gắn SIM, nhập `ChuaGan`).
  6. Yêu cầu nhập `Tram BTS gan nhat` (Mã trạm phát sóng định vị thiết bị, VD: `BTS-HN-001`).
  7. Ngày kích hoạt tự động lấy ngày hiện tại hoặc cho phép người dùng nhập. Trạng thái mặc định: `HoatDong`.
  8. Lưu vào vector và cập nhật tệp `data/imei.txt`.

### 2.2 UC02.2: Xem Danh Sách Thiết Bị
* **Mục tiêu:** Hiển thị danh sách thiết bị kèm thông tin trạm BTS và trạng thái hoạt động trên mạng.
* **Luồng sự kiện chính:** Hiển thị bảng định dạng chuẩn gồm các cột: `Ma IMEI (15 so)`, `Ten Thiet Bi`, `Hang SX`, `So Dien Thoai`, `Ngay Kich Hoat`, `Trang Thai`, `Tram BTS Gan Nhat`.

### 2.3 UC02.3: Tra Cứu Thiết Bị Đa Tiêu Chí
* **Mục tiêu:** Phục vụ công tác hỗ trợ khách hàng và điều tra viễn thông.
* **Lựa chọn tra cứu:**
  - `[1] Tra cuu theo Ma IMEI (Exact 15 digits)`.
  - `[2] Tra cuu theo So dien thoai dang gan tren may`.
  - `[3] Tra cuu theo Hang san xuat (Liet ke toan bo may cua Apple, Samsung...)`.

### 2.4 UC02.4: Quản Lý Danh Sách Đen Khóa Mạng (EIR Blacklist)
* **Bối cảnh viễn thông:** Theo chuẩn 3GPP EIR (Equipment Identity Register), các thiết bị bị báo mất trộm, cướp giật, hoặc thiết bị phát tán tin nhắn rác/gian lận cước sẽ bị đưa vào danh sách đen (`Blacklist` - `KhoaMang`) để toàn bộ trạm BTS từ chối kết nối vô tuyến.
* **Luồng sự kiện chính:**
  1. Hệ thống lọc và hiển thị danh sách tất cả các thiết bị có `trangThai == "KhoaMang"`.
  2. Báo cáo tổng số thiết bị đang bị chặn truy cập mạng lưới.

### 2.5 UC02.5: Cập Nhật Trạng Thái Thiết Bị & Gán Lại SIM
* **Mục tiêu:** Thay đổi SIM, cập nhật trạm BTS khi máy di chuyển, hoặc Khóa / Mở khóa thiết bị.
* **Lựa chọn cập nhật:**
  - `[1] Gan lai SIM`: Nhập số điện thoại mới gắn vào thiết bị.
  - `[2] Cap nhat tram BTS`: Nhập mã trạm BTS mới nhất ghi nhận tín hiệu thiết bị.
  - `[3] Khoa may (Dua vao EIR Blacklist)`: Chuyển trạng thái sang `KhoaMang`.
  - `[4] Mo khoa may (Whitelist)`: Chuyển trạng thái trở lại `HoatDong`.

### 2.6 UC02.6: Xóa Thiết Bị Khỏi Hệ Thống
* **Mục tiêu:** Loại bỏ thiết bị hỏng vĩnh viễn hoặc thanh lý phần cứng khỏi cơ sở dữ liệu.
* **Luồng sự kiện chính:** Nhập IMEI, xác nhận bảo mật `(y/n)`, xóa khỏi bộ nhớ và đồng bộ tệp `data/imei.txt`.
