# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Nguyễn Tùng Dương
- **Mã sinh viên:** B24DCVT106
- **Lớp:** D24CQVT05-B | Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git phát triển:** `TungDuong-B24DCVT106`

---

## Phân Công Chức Năng (Use Cases)
1. **Quản lý Gói cước (UC01 - `GoiCuoc`):** Thêm gói cước (Create), Xem danh sách gói cước (Read), Tìm kiếm gói cước (Read/Search), Cập nhật/Sửa gói cước (Update), Xóa gói cước (Delete).
2. **Quản lý Loại hình Thuê Bao (UC02 - `LoaiThueBao`):** Thêm thuê bao (Create), Xem danh sách thuê bao (Read), Tìm kiếm thuê bao (Read/Search), Cập nhật/Sửa thuê bao (Update), Xóa thuê bao (Delete).

---

## Cấu Trúc Đóng Góp Mô-đun
- **Model:** `src/lib/models/goicuoc.h`, `src/lib/models/goicuoc.cpp`
- **Model:** `src/lib/models/thuebao.h`, `src/lib/models/thuebao.cpp`
- **Menu:** `src/lib/menus/GoiCuocMenu.h`, `src/lib/menus/GoiCuocMenu.cpp`
- **Dữ liệu:** `data/CacGoiCuoc.txt`, `data/thuebao.txt`
