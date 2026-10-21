# ghicuocgoi (CDR)

## Mục đích

`ghicuocgoi` là tên nghiệp vụ tiếng Việt của mô hình kỹ thuật `CDR` (`Call Detail Record`). Mỗi bản ghi mô tả một sự kiện sử dụng dịch vụ: thoại, SMS hoặc Data.

## Thành phần

- Model: `src/lib/models/ghicuocgoi.h`, `src/lib/models/ghicuocgoi.cpp`
- Menu: `src/lib/menus/NapTienMenu.h`, `src/lib/menus/NapTienMenu.cpp`
- Kho dữ liệu: `data/ghicuocgoi.txt`
- Repository: `Repository<CDR>` được khởi tạo trong `src/main.cpp`

## Schema dữ liệu

Mỗi dòng trong `data/ghicuocgoi.txt` có bảy trường, phân cách bằng `|`:

```text
maCDR|soGoi|soNhan|loaiDichVu|thoiGianBatDau|thoiLuong|cuocPhatSinh
```

Ví dụ:

```text
CDR0001|0981234567|0912345678|Thoai|15/01/2024 08:30:00|120|2000
```

- `maCDR`: mã bản ghi duy nhất.
- `soGoi`: số thuê bao phát sinh sử dụng.
- `soNhan`: số thuê bao nhận; Data dùng `DATA_SESSION`.
- `loaiDichVu`: `Thoai`, `SMS` hoặc `Data`.
- `thoiGianBatDau`: thời gian bắt đầu do người dùng nhập.
- `thoiLuong`: số giây theo schema hiện tại cho thoại/SMS; số MB cho Data.
- `cuocPhatSinh`: cước phát sinh, tính bằng VND.

## Luồng menu

Từ menu chính chọn `[6] Quan ly Nap tien & Ban ghi CDR`, sau đó:

- `[4]` ghi nhận một bản ghi `ghicuocgoi`.
- `[5]` xem danh sách CDR.

Khi ghi nhận, menu kiểm tra thuê bao gọi tồn tại, kiểm tra mã CDR không trùng, tạo mô hình với các trường bắt buộc và thêm vào repository. Nếu thuê bao trả trước có đủ số dư, cước được khấu trừ; bản ghi vẫn được lưu độc lập.

## Kiểm tra và lưu trữ

Constructor từ chối trường bắt buộc rỗng, thời lượng âm và cước âm. `fromFileString()` yêu cầu đủ bảy trường và giá trị số hợp lệ. `Repository<CDR>` dùng chung cơ chế staging và lưu nguyên tử của `Repository<T>`.

Kiểm thử liên quan nằm trong `tests/test_runner.cpp`, gồm khởi tạo, getter, tuần tự hóa, đọc lại bản ghi và các trường hợp thời lượng/cước âm. Tên file/module và fixture dùng chữ thường `ghicuocgoi`; lớp C++ vẫn là `CDR`. Lần kiểm tra gần nhất: **283 passed, 0 failed**; CTest **3/3 passed**.
