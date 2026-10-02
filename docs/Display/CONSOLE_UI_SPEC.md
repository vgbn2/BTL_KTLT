# ĐẶC TẢ GIAO DIỆN CONSOLE (CONSOLE UI SPECIFICATION)
## HỆ THỐNG QUẢN LÝ THUÊ BAO DI ĐỘNG (KTLT - PTIT)

---

## 1. Nguyên Tắc Thiết Kế Giao Diện

1. **Chuẩn mực đồ án học thuật:** Tuân thủ cấu trúc menu số 0–9 trực quan, rõ ràng, đúng theo ví dụ minh họa tại Mục 5 Đề bài BTL KTLT.
2. **Không rác giao diện (Zero AI Fluff):** Không chèn các biểu ngữ màu mè, không in thông tin cá nhân/watermark làm rối mắt giảng viên khi chấm điểm console.
3. **Căn chỉnh hoàn hảo:** Sử dụng thư viện chuẩn `<iomanip>` (`std::setw`, `std::left`, `std::right`, `std::setfill`) để mọi bảng dữ liệu hiển thị thẳng hàng tuyệt đối.
4. **Trải nghiệm nhập liệu an toàn:** Luôn có hướng dẫn rõ ràng, nhắc lại khi nhập sai kiểu dữ liệu, xác nhận `(y/n)` trước các thao tác phá hủy dữ liệu (Xóa/Thanh lý).

---

## 2. Thiết Kế Menu Chính (Main Menu)

```text
====================================================================
               HE THONG QUAN LY THUE BAO DI DONG
====================================================================
  [1] Quan ly goi cuoc
  [2] Quan ly thue bao
  [3] Quan ly hop dong
  [4] Quan ly thiet bi IMEI
  [5] Quan ly phieu nap tien
  [6] Quan ly hoa don
  [0] Thoat chuong trinh
====================================================================
Nhap lua chon cua ban [0-6]: 
```

---

## 3. Thiết Kế Phân Hệ Quản Lý Hợp Đồng (`HopDongMenu`)

### 3.1 Menu Điều Hướng Hợp Đồng
```text
--------------------------------------------------------------------
                     QUAN LY HOP DONG DANG KY
--------------------------------------------------------------------
  [1] Them hop dong moi
  [2] Xem danh sach hop dong
  [3] Tim kiem hop dong
  [4] Sap xep danh sach
  [5] Cap nhat thong tin hop dong
  [6] Xoa hop dong
  [0] Quay lai menu chinh
--------------------------------------------------------------------
Nhap lua chon cua ban [0-6]: 
```

### 3.2 Định Dạng Bảng Dữ Liệu Hợp Đồng (`<iomanip>`)
* **Độ rộng các cột:**
  - `Ma HD`: 10 ký tự
  - `Ma KH`: 10 ký tự
  - `So Dien Thoai`: 14 ký tự
  - `Ma Goi`: 10 ký tự
  - `Ngay DK`: 13 ký tự
  - `Ngay HH`: 13 ký tự
  - `Loai HD`: 11 ký tự
  - `Trang Thai`: 12 ký tự
  - `Gia Cuoc (VND)`: 14 ký tự (Căn phải)

```text
+----------+----------+--------------+----------+-------------+-------------+-----------+------------+--------------+
|  Ma HD   |  Ma KH   | So Dien Thoai|  Ma Goi  |   Ngay DK   |   Ngay HH   |  Loai HD  | Trang Thai |Gia Cuoc (VND)|
+----------+----------+--------------+----------+-------------+-------------+-----------+------------+--------------+
| HD0001   | KH0001   | 0981234567   | GC001    | 15/01/2024  | 15/01/2025  | TraSau    | HieuLuc    |       150000 |
| HD0002   | KH0002   | 0978999888   | GC002    | 20/02/2024  | 20/02/2025  | TraTruoc  | HieuLuc    |        90000 |
| HD0003   | KH0003   | 0912345678   | VD149    | 01/03/2024  | 01/09/2024  | TraTruoc  | TamDung    |       149000 |
+----------+----------+--------------+----------+-------------+-------------+-----------+------------+--------------+
Tong so: 3 hop dong.
```

---

## 4. Thiết Kế Phân Hệ Quản Lý Thiết Bị IMEI (`ThietBiIMEIMenu`)

### 4.1 Menu Điều Hướng Thiết Bị IMEI
```text
--------------------------------------------------------------------
                     QUAN LY THIET BI DAU CUOI (IMEI)
--------------------------------------------------------------------
  [1] Them thiet bi moi (Kiem tra Luhn Checksum 3GPP)
  [2] Xem danh sach thiet bi
  [3] Tim kiem thiet bi (Theo IMEI / So DT / Hang SX)
  [4] Danh sach thiet bi bi khoa mang (EIR Blacklist)
  [5] Cap nhat thiet bi (Gan SIM / Doi BTS / Khoa may)
  [6] Xoa thiet bi
  [0] Quay lai menu chinh
--------------------------------------------------------------------
Nhap lua chon cua ban [0-6]: 
```

### 4.2 Định Dạng Bảng Dữ Liệu Thiết Bị IMEI (`<iomanip>`)
* **Độ rộng các cột:**
  - `Ma IMEI`: 18 ký tự
  - `Ten Thiet Bi`: 20 ký tự
  - `Hang SX`: 12 ký tự
  - `So Dien Thoai`: 14 ký tự
  - `Ngay Kich Hoat`: 15 ký tự
  - `Trang Thai`: 12 ký tự
  - `Tram BTS Gan Nhat`: 16 ký tự

```text
+------------------+--------------------+------------+--------------+---------------+------------+----------------+
|     Ma IMEI      |    Ten Thiet Bi    |  Hang SX   | So Dien Thoai| Ngay Kich Hoat| Trang Thai |  Tram BTS Gan  |
+------------------+--------------------+------------+--------------+---------------+------------+----------------+
| 860123456789012  | iPhone 15 Pro Max  | Apple      | 0981234567   | 15/01/2024    | HoatDong   | BTS-HN-001     |
| 351234567890123  | Galaxy S24 Ultra   | Samsung    | 0978999888   | 20/02/2024    | HoatDong   | BTS-DN-005     |
| 861987654321098  | Xiaomi 14 Pro      | Xiaomi     | ChuaGan      | 10/03/2024    | KhoaMang   | BTS-HCM-012    |
+------------------+--------------------+------------+--------------+---------------+------------+----------------+
Tong so: 3 thiet bi.
```

---

## 5. Mẫu Thông Báo Trạng Thái & Hộp Thoại Lỗi

### 5.1 Thông Báo Thành Công
```text
[THANH CONG] Da them hop dong HD0004 vao he thong va dong bo xuong file!
[THANH CONG] Thiet bi IMEI 860123456789012 da duoc khoa mang (EIR Blacklist)!
```

### 5.2 Thông Báo Lỗi Nghiệp Vụ
```text
[LOI] Ma hop dong 'HD0001' da ton tai tren he thong!
[LOI] Ma IMEI '860123456789011' khong hop le theo chuan 3GPP (Sai ma kiem tra Luhn Mod-10)!
[LOI] Ngay het han (15/01/2023) khong duoc nho hon ngay dang ky (15/01/2024)!
[LOI] Gia tri nhap vao khong dung dinh dang so! Vui long thu lai.
```
