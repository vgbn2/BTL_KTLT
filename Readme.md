# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Trần Đức Anh
- **Mã sinh viên:** B24DCVT021
- **Lớp:** D24CQVT01-B | Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git phát triển:** `DucAnh-B24DCVT021`

---

## Phân Công Chức Năng (Use Cases)
1. **Quản lý Hợp đồng đăng ký (UC01 - `HopDong`):** Khởi tạo, ký kết, tìm kiếm đa tiêu chí, sắp xếp, cập nhật thông tin, gia hạn và thanh lý hợp đồng dịch vụ viễn thông.
2. **Quản lý Thiết bị đầu cuối & IMEI (UC02 - `ThietBiIMEI`):** Ghi nhận phần cứng, kiểm tra hợp chuẩn 3GPP (Thuật toán Luhn Checksum Mod-10), định vị trạm BTS và quản trị danh sách đen khóa mạng (EIR Blacklist).

---

## Cấu Trúc Dự Án
```text
├── data/                               # Cơ sở dữ liệu file phẳng (pipe-delimited |)
│   ├── hopdong.txt                     # Bảng dữ liệu hợp đồng viễn thông
│   └── imei.txt                        # Bảng dữ liệu thiết bị phần cứng IMEI
├── docs/                               # Bộ tài liệu học thuật và kỹ thuật đầy đủ
│   ├── Overview/                       # Kiến trúc hệ thống và đặc tả yêu cầu
│   │   └── ARCHITECTURE_AND_REQUIREMENTS.md
│   ├── Operational/                    # Đặc tả thiết kế kỹ thuật và use case chi tiết
│   │   ├── TECHNICAL_DESIGN_SPECIFICATION.md
│   │   └── USE_CASE_SPEC_DUCANH.md
│   ├── Display/                        # Đặc tả giao diện console
│   │   └── CONSOLE_UI_SPEC.md
│   ├── UserGuide/                      # Hướng dẫn biên dịch và sử dụng đa nền tảng
│   │   └── USER_MANUAL.md
│   └── ducanh.md                       # Tài liệu hướng dẫn kỹ thuật phân hệ
├── src/                                # Mã nguồn ứng dụng C++
│   ├── lib/                            # Lớp cơ sở Entity, Repository, Date, InputHelper, Models, Menus
│   └── main.cpp                        # Điểm khởi chạy chương trình chính (Main Menu 0-9)
├── tests/                              # Bộ kiểm thử tự động
│   └── test_runner.cpp                 # Kiểm thử Luhn, Date, Repository CRUD (50 assertions)
├── run_windows.bat                     # Tệp chạy 1-Click trên Windows
├── CMakeLists.txt                      # Cấu hình xây dựng CMake đa nền tảng
└── Makefile                            # Tệp cấu hình biên dịch GNU Make (-std=c++11 -Wall -Wextra)
```

---

## Hướng Dẫn Khởi Chạy Đa Nền Tảng

### 1. Trên Windows (1-Click Nhanh Nhất)
* Nhấp đúp chuột vào tệp tin **`run_windows.bat`**.
* Chương trình sẽ tự động bật UTF-8, biên dịch toàn bộ mã nguồn và mở giao diện console.

### 2. Trên Linux / macOS (GNU Make)
```bash
# Biên dịch và khởi chạy chương trình chính
make run

# Chạy bộ kiểm thử tự động (50/50 test cases PASS)
make test
```

### 3. Trên Visual Studio / CLion / IDE hỗ trợ CMake
```bash
cmake -B build -S .
cmake --build build
```

### 4. Trong Visual Studio Code
* Mở tệp `src/main.cpp` $\to$ Bấm nút **▶️ Play** ở góc trên bên phải hoặc nhấn **`F5`** / **`Ctrl + F5`**.
