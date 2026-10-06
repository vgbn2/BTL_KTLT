# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Nguyễn Tùng Dương
- **Mã sinh viên:** B24DCVT106
- **Lớp:** D24CQVT05-B | Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git phát triển:** `TungDuong-`

---

## Phân Công Chức Năng (Use Cases)
1. **Quản lý Gói cước (UC01 - `GoiCuoc`):** Thêm gói cước(Create), Xem danh sách gói cước(Read), Tìm kiếm gói cước(Read/Search), Cập nhật/Sửa gói cước(Update), Xóa gói cước(Delete).
2. **Quản lý Loại hình Thuê Bao (UC02 - `LoaiThueBao`):**Thêm thuê bao(Create), Xem danh sách thuê bao(Read), Tìm kiếm thuê bao(Read/Search), Cập nhật/Sửa thuê bao(Update), Xóa thuê bao(Delete).

---

## Cấu Trúc Dự Án
```text
QuanLyThueBao/
├── include/                  # Thư mục chứa các file khai báo Header (.h)
│   ├── GoiCuoc.h             # Khai báo lớp GoiCuoc
│   ├── ThueBao.h             # Khai báo lớp cơ sở ThueBao
│   ├── ThueBaoTraTruoc.h     # Lớp ThueBaoTraTruoc (kế thừa ThueBao)
│   ├── ThueBaoTraSau.h       # Lớp ThueBaoTraSau (kế thừa ThueBao)
│   ├── QuanLyGoiCuoc.h       # Khai báo lớp quản lý CRUD Gói Cước
│   ├── QuanLyThueBao.h       # Khai báo lớp quản lý CRUD Thuê Bao
│   └── FileManager.h         # Khai báo các hàm đọc/ghi File
│
├── src/                      # Thư mục chứa các file định nghĩa C++ (.cpp)
│   ├── GoiCuoc.cpp
│   ├── ThueBao.cpp
│   ├── ThueBaoTraTruoc.cpp
│   ├── ThueBaoTraSau.cpp
│   ├── QuanLyGoiCuoc.cpp
│   ├── QuanLyThueBao.cpp
│   ├── FileManager.cpp
│   └── main.cpp              # Điểm chạy chính (Menu Console)
│
├── data/                     # Thư mục chứa file dữ liệu lưu trữ
│   ├── goicuoc.txt           # Dữ liệu gói cước lưu trữ
│   └── thuebao.txt           # Dữ liệu thuê bao lưu trữ
│
├── .gitignore                # Bỏ qua các file biên dịch (.exe, .o, .obj)
├── Makefile                  # File hỗ trợ biên dịch tự động (nếu dùng Linux/Terminal)
└── README.md                 # Tài liệu hướng dẫn cài đặt và giới thiệu dự án
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
* Mở tệp `src/main.cpp` $\to$ Bấm nút ** Play ** ở góc trên bên phải hoặc nhấn **`F5`** / **`Ctrl + F5`**.
