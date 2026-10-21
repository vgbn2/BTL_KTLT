# Hệ Thống Quản Lý Thuê Bao Di Động (KTLT - PTIT)

## Thông Tin Sinh Viên Thực Hiện
- **Sinh viên:** Phạm Đức Chính
- **Mã sinh viên:** B24DCVT047
- **Lớp:** D24CQVT01-B
- **Khoa:** Khoa Viễn thông 1 — Học viện Công nghệ Bưu chính Viễn thông (PTIT)
- **Nhánh Git:** `DucChinh-B24DCVT047`

---

## Phân Công Chức Năng (Use Cases)
- **Quản lý Phiếu nạp tiền (`NapTien`):**
  - Xử lý các giao dịch nạp tiền qua mã thẻ cào, ví điện tử hoặc ngân hàng vào tài khoản thuê bao.
  - Quản lý lịch sử biến động số dư và trạng thái nạp tiền.
- **ghicuocgoi (`CDR` - Call Detail Record):**
  - Ghi nhận từng cuộc gọi thoại, tin nhắn SMS hoặc phiên truy cập Data trong `data/ghicuocgoi.txt`.
  - Lưu mã bản ghi, số gọi, số nhận, loại dịch vụ, thời gian bắt đầu, thời lượng/dung lượng và cước phát sinh.
  - Khấu trừ cước từ số dư thuê bao trả trước khi tài khoản đủ tiền.

---

## Cấu Trúc Mã Nguồn Đóng Góp
- **Models:** `src/lib/models/naptien.h/cpp`, `src/lib/models/ghicuocgoi.h/cpp`
- **Menu Controller:** `src/lib/menus/NapTienMenu.h/cpp`
- **Dữ liệu mẫu:** `data/naptien.txt`, `data/ghicuocgoi.txt`
