# HƯỚNG DẪN SỬ DỤNG VÀ CHẠY ĐA NỀN TẢNG
## HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG - PTIT KTLT

---

## 1. Hướng Dẫn Chạy Trên Mọi Nền Tảng

Dự án được cấu hình sẵn để bất kỳ thành viên, giảng viên hay người dùng nào cũng có thể biên dịch và khởi chạy dễ dàng trên **Windows**, **Linux**, và **macOS**.

---

### 1.1 Cách 1: Trên Windows (1-Click Chạy Ngay)

1. **Cách nhanh nhất (Batch Script):**
   * Nhấp đúp chuột vào tệp tin **`run_windows.bat`** tại thư mục gốc của dự án.
   * Tập lệnh sẽ tự động kiểm tra trình biên dịch `g++`, bật mã hóa tiếng Việt UTF-8 (`chcp 65001`), biên dịch toàn bộ các module và khởi chạy cửa sổ console ngay lập tức.
2. **Dành cho người dùng Dev-C++ / Code::Blocks trên Windows:**
   * Mở Dev-C++ hoặc Code::Blocks $\to$ Chọn **File > New > Project (Console Application C++)**.
   * Thêm tệp `src/main.cpp` và toàn bộ các tệp `.cpp` trong thư mục `src/lib/` vào dự án.
   * Vào **Project Options > Compiler Options** $\to$ Thêm cờ include: `-Isrc -Isrc/lib -std=c++11`.
   * Bấm phím **F9** (hoặc **F11**) để Build & Run.
3. **Dành cho người dùng Visual Studio (2019 / 2022):**
   * Mở Visual Studio $\to$ Chọn **Open a Local Folder** $\to$ Trỏ tới thư mục `KTLT/`.
   * Visual Studio sẽ tự động nhận diện tệp `CMakeLists.txt` $\to$ Chọn `quanlythuebao.exe` trên thanh công cụ và bấm **Run (F5)**.

---

### 1.2 Cách 2: Trên Linux & macOS (GNU Make)

Mở Terminal tại thư mục gốc của dự án (`KTLT/`):
```bash
# Biên dịch và chạy ứng dụng chính
make run

# Hoặc dọn dẹp và biên dịch lại từ đầu
make clean && make

# Chạy bộ kiểm thử tự động (50 unit tests)
make test
```

---

### 1.3 Cách 3: Sử Dụng CMake (Độc lập nền tảng)

Dành cho mọi IDE hỗ trợ CMake (CLion, VS Code, Qt Creator, Xcode):
```bash
# Cấu hình và biên dịch bằng CMake
cmake -B build -S .
cmake --build build

# Chạy chương trình
./build/quanlythuebao       # Trên Linux/macOS
.\build\Debug\quanlythuebao # Trên Windows Visual Studio
```

---

### 1.4 Cách 4: Chạy Trực Tiếp Trong VS Code (1-Click Play Button)

1. Mở thư mục dự án trong VS Code.
2. Mở tệp `src/main.cpp`.
3. Bấm vào nút **▶️ Play (Run C/C++ File)** ở góc trên bên phải, hoặc bấm phím **F5** / **Ctrl + F5**.
4. Chương trình sẽ tự động biên dịch toàn bộ module và mở bảng điều khiển tương tác ngay trong Terminal tích hợp của VS Code.

---

## 2. Kịch Bản Thao Tác Nghiệp Vụ

### 2.1 Quản Lý Hợp Đồng Đăng Ký (UC01)
1. Tại **Menu chính**, chọn `[3] Quan ly hop dong`.
2. Lựa chọn các chức năng:
   - `[1] Them hop dong moi`: Nhập mã hợp đồng duy nhất (`HDxxxx`), thông tin khách hàng, số điện thoại, gói cước, ngày đăng ký, ngày hết hạn và cước phí.
   - `[2] Xem danh sach hop dong`: Hiển thị bảng định dạng chuẩn `<iomanip>` toàn bộ hợp đồng.
   - `[3] Tim kiem hop dong`: Tra cứu theo Mã HĐ, Số điện thoại hoặc Mã khách hàng.
   - `[4] Sap xep danh sach`: Sắp xếp theo ngày đăng ký (mới nhất) hoặc giá trị cước (giảm dần).
   - `[5] Cap nhat thong tin`: Gia hạn hợp đồng, đổi gói cước, chuyển trạng thái (`HieuLuc`, `TamDung`, `ThanhLy`).
   - `[6] Xoa hop dong`: Xóa an toàn kèm hộp thoại xác nhận `(y/n)`.

### 2.2 Quản Lý Thiết Bị Đầu Cuối & IMEI (UC02)
1. Tại **Menu chính**, chọn `[4] Quan ly thiet bi IMEI`.
2. Lựa chọn các chức năng:
   - `[1] Them thiet bi moi`: Nhập 15 chữ số IMEI $\to$ Hệ thống tự động xác thực thuật toán **3GPP Luhn Checksum (Mod-10)**; từ chối và yêu cầu nhập lại nếu sai mã kiểm tra.
   - `[2] Xem danh sach thiet bi`: Hiển thị danh sách thiết bị kèm trạm BTS và trạng thái mạng.
   - `[3] Tim kiem thiet bi`: Tra cứu theo IMEI, Số điện thoại hoặc Hãng sản xuất.
   - `[4] Danh sach thiet bi bi khoa mang`: Lọc danh sách đen EIR Blacklist (`KhoaMang`).
   - `[5] Cap nhat thiet bi`: Gán lại SIM (đổi máy), cập nhật vị trí trạm BTS, Khóa mạng / Mở khóa mạng.
   - `[6] Xoa thiet bi`: Xóa phần cứng hỏng khỏi cơ sở dữ liệu.

---

## 3. Cấu Trúc Tệp Tin Cơ Sở Dữ Liệu (`data/`)

Dữ liệu được lưu trữ tự động dạng tệp tin văn bản thuần túy (`pipe-delimited |`), tự động sao lưu an toàn (Atomic Write qua file `.tmp`):
* `data/hopdong.txt`: Bảng lưu trữ hợp đồng đăng ký dịch vụ.
* `data/imei.txt`: Bảng lưu trữ định danh thiết bị đầu cuối IMEI.
