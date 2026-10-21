# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Trương Quốc Khánh
- **Mã sinh viên:** B24DCVT201
- **Khoa:** Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git phát triển:** `QuocKhanh-B24DCVT201`

---

## Phân Công Chức Năng (Use Cases)
- **Quản lý Thông tin Khách Hàng (`KhachHang`):**
  - Quản lý định danh khách hàng (Mã KH, Họ tên, Loại giấy tờ, Số CCCD/Hộ chiếu, Ngày cấp, Nơi cấp, SĐT liên hệ, Địa chỉ).
  - Tích hợp liên kết với Hợp đồng (`HopDong`) và Thuê bao (`ThueBao`).

---

## Cấu Trúc Đóng Góp Mô-đun
- **Model:** `src/lib/models/khachhang.h`, `src/lib/models/khachhang.cpp`
- **Dữ liệu:** `data/khachhang.txt`
