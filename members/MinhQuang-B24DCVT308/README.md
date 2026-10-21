# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Nguyễn Minh Quang
- **Mã sinh viên:** B24DCVT308
- **Lớp:** D24CQVT01-B
- **Khoa:** Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git:** `MinhQuang-B24DCVT308`

---

## Phân Công Chức Năng (Use Cases)
- **Quản lý Hóa đơn cước (`HoaDon`):**
  - Gom toàn bộ cước phát sinh trong tháng (cước gói, cước data vượt gói, cước thoại) và tạo hóa đơn thanh toán.
  - Quản lý trạng thái thanh toán và lịch sử thanh toán hóa đơn.
- **Phiếu khiếu nại cước & dịch vụ (`KhieuNai`):**
  - Ghi nhận phản ánh của khách hàng về sự cố cước, chất lượng sóng mạng, nạp thẻ và tiến hành tra soát.
  - Cập nhật quy trình xử lý: Tiếp nhận -> Đang xử lý -> Đã giải quyết / Hoàn cước -> Từ chối.

---

## Cấu Trúc Mã Nguồn Đóng Góp
- **Models:** `src/lib/models/hoadon.h/cpp`, `src/lib/models/khieunai.h/cpp`
- **Menu Controller:** `src/lib/menus/HoaDonMenu.h/cpp`
- **Dữ liệu mẫu:** `data/hoadon.txt`, `data/khieunai.txt`
