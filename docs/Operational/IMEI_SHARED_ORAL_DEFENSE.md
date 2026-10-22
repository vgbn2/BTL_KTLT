# Bảo vệ miệng: hạ tầng dùng chung và IMEI

## Mục đích

Tài liệu này giải thích các tương tác dễ bị hỏi trong phần bảo vệ mã nguồn:

- `Repository<T>` đọc, ghi và khôi phục dữ liệu như thế nào;
- vì sao phải tách dữ liệu tạm khỏi dữ liệu đang dùng;
- ranh giới trách nhiệm giữa model, menu và repository;
- quyền sở hữu các trường của `ThietBiIMEI`;
- sự khác nhau giữa kiểm tra Luhn và kiểm tra trùng mã;
- vị trí hợp lý của hàm sinh IMEI mô phỏng.

Đây là giải thích cho hành vi hiện tại,không phải đề xuất thay đổi định dạng dữ liệu.

## 1. Sơ đồ trách nhiệm

```text
menu
  -> nhận lựa chọn và dữ liệu từ người dùng
  -> kiểm tra khóa ngoại khi cần
  -> gọi model và repository

model
  -> sở hữu trường dữ liệu của một bản ghi
  -> kiểm tra định dạng và bất biến nghiệp vụ
  -> chuyển bản ghi sang/từ dòng văn bản

repository
  -> sở hữu tập hợp các bản ghi trong bộ nhớ
  -> đọc toàn bộ tệp trước khi công bố dữ liệu mới
  -> ghi snapshot tạm trước khi thay thế tệp thật
  -> khôi phục bộ nhớ nếu thao tác lưu thất bại

FileIO
  -> tách trường, tạo đường dẫn staging và commit tệp
```

Điểm quan trọng: mỗi lớp kiểm tra một loại quy tắc khác nhau. Không nên đưa toàn bộ quy tắc vào một lớp chỉ vì lớp đó đang gọi hàm.

## 2. Đọc tệp: vì sao cần `loadedItems`

Luồng trong `Repository<T>::loadFromFile()` là:

```text
mở tệp
  -> đọc từng dòng
  -> bỏ dòng rỗng và dòng chú thích
  -> gọi T::fromFileString()
  -> đưa bản ghi hợp lệ vào loadedItems
  -> chỉ khi toàn bộ tệp hợp lệ mới swap vào items
```

Ví dụ tệp có:

```text
bản ghi hợp lệ 1
bản ghi hợp lệ 2
dòng sai định dạng
```

Kết quả đúng:

- tập hợp đang có trong bộ nhớ vẫn được giữ nguyên;
- hai bản ghi tạm được đọc trước đó bị bỏ đi;
- `loadFromFile()` trả về thất bại;
- thao tác đọc không ghi đè hay xóa tệp nguồn.

`loadedItems` là một vùng kiểm tra trung gian. Nó ngăn trạng thái đọc dở dang được công bố như trạng thái chính thức.

Không nên giải thích cơ chế này bằng SQL `VACUUM`, garbage collector hoặc việc giữ nguyên địa chỉ bộ nhớ. Đây là bảo đảm an toàn trạng thái của `std::vector` và giao dịch đọc tệp trong C++.

## 3. Ghi tệp: snapshot tạm trước khi commit

`Repository<T>::saveToFile()` dùng thứ tự:

```text
serialize -> flush/check -> close/check -> commit
```

Ý nghĩa từng bước:

1. `serialize`: ghi toàn bộ tập hợp hiện tại vào tệp tạm.
2. `flush/check`: đẩy dữ liệu đang nằm trong buffer và phát hiện lỗi ghi.
3. `close/check`: đóng stream và kiểm tra lỗi khi kết thúc ghi.
4. `commit`: chỉ sau khi snapshot tạm hoàn chỉnh mới thay thế tệp dữ liệu thật.

`flush()` không xóa dữ liệu cũ và `close()` không phải là thao tác làm sạch input. Đây là các bước kiểm tra an toàn của output stream.

Nếu ghi trực tiếp vào tệp thật rồi gặp lỗi giữa chừng, tệp thật có thể bị cắt ngắn hoặc chỉ chứa một phần snapshot mới. Cách staging giữ tệp cũ nguyên vẹn cho đến khi snapshot mới sẵn sàng.

Trên Linux, dự án ưu tiên thư mục staging trong RAM thông qua `tmps/`/`/dev/shm`; đây là tối ưu giảm ghi trực tiếp lên SSD. Tính đúng đắn cốt lõi vẫn là chỉ commit một snapshot đã ghi và kiểm tra thành công.

## 4. Rollback sau khi lưu thất bại

Repository thay đổi bộ nhớ trước rồi mới lưu tệp. Vì vậy, nếu lưu thất bại,repository phải hoàn tác thay đổi logic:

| Thao tác | Bản sao khôi phục | Hoàn tác |
|---|---|---|
| `add()` | không cần bản sao riêng | `pop_back()` bản ghi vừa thêm |
| `update()` | bản ghi cũ | gán lại bản ghi cũ |
| `remove()` | `T previous = *it` và vị trí cũ | chèn lại tại vị trí ban đầu |
| `sortAndSave()` | thứ tự cũ | khôi phục thứ tự cũ |
| `clear()` | toàn bộ collection cũ | gán lại collection cũ |

Đây là rollback trạng thái nghiệp vụ,không phải tự giải phóng bộ nhớ. `std::vector` và RAII quản lý bộ nhớ; repository phải khôi phục nội dung logic mà người dùng nhìn thấy.

Nếu bộ nhớ đã thêm bản ghi nhưng tệp vẫn là snapshot cũ, hai nguồn dữ liệu bị lệch nhau. Lần đọc hoặc khởi động sau có thể làm mất bản ghi chỉ tồn tại trong bộ nhớ. Vì vậy, đường ghi phải báo lỗi và rollback thay vì giả vờ thành công.

## 5. Tìm kiếm ID và chuẩn hóa

`Repository<T>::findById()` là hàm dùng chung và so sánh ID chính xác. Nó không tự ý viết hoa, viết thường hoặc trim mọi loại ID.

Menu khách hàng có thể áp dụng chính sách riêng:

```text
kh0000001 -> trim + uppercase -> KH0000001 -> Repository::findById()
```

Chuẩn hóa nằm ở biên nhận input vì đó là chính sách của ID khách hàng. Nếu repository tự chuẩn hóa tất cả ID, nó có thể làm thay đổi ngữ nghĩa của những entity khác.

## 6. Quyền sở hữu trường IMEI

`ThietBiIMEI` sở hữu một bản ghi thiết bị đầu cuối gồm:

| Trường | Ý nghĩa |
|---|---|
| IMEI | định danh thiết bị, đồng thời là ID của entity |
| `tenThietBi` | tên hoặc dòng thiết bị |
| `hangSanXuat` | nhà sản xuất |
| `soDienThoai` | số thuê bao đang gắn với thiết bị |
| `ngayKichHoat` | ngày kích hoạt thiết bị |
| `trangThai` | trạng thái hoạt động, khóa hoặc tạm khóa |
| `tramBTSGanNhat` | trạm BTS gần nhất được ghi nhận |

Model kiểm tra các bất biến nội tại của bản ghi: IMEI hợp lệ, số điện thoại đúng dạng nếu có, trạng thái hợp lệ và các trường bắt buộc không rỗng.

Menu sở hữu luồng tương tác: hỏi người dùng, chọn IMEI thật hay mô phỏng, kiểm tra số thuê bao có tồn tại trong `thueBaoStore`, rồi tạo và thêm model.

## 7. Luhn và tính duy nhất là hai quy tắc khác nhau

Hai dòng sau không kiểm tra cùng một việc:

```cpp
ThietBiIMEI::validateLuhn(imei)
store.findById(imei)
```

`validateLuhn()` kiểm tra dạng toán học của chuỗi IMEI:

- có đúng 15 chữ số theo hợp đồng hiện tại;
- chữ số kiểm tra phù hợp với 14 chữ số trước đó;
- không chứa ký tự không phải chữ số;
- không chấp nhận chuỗi toàn số 0 theo validator hiện tại.

`findById()` kiểm tra nghiệp vụ của kho:

- ID này đã có trong collection chưa;
- một mã hợp lệ về mặt Luhn vẫn không được phép trùng với bản ghi đã tồn tại.

Ví dụ một IMEI có checksum đúng nhưng đã tồn tại trong `data/imei.txt`. Nó hợp lệ về format nhưng không hợp lệ để thêm bản ghi mới.

## 8. `ChuaGan` và khóa ngoại

`ChuaGan` là sentinel biểu thị thiết bị chưa được gắn thuê bao. Nó không phải là một số điện thoại thật.

Vì vậy:

- model cho phép `ChuaGan` ở vị trí số điện thoại;
- nếu có số thật, model kiểm tra format số điện thoại;
- menu còn kiểm tra số đó có tồn tại trong `thueBaoStore` hay không.

Hai kiểm tra này có phạm vi khác nhau:

```text
model: chuỗi có đúng dạng số điện thoại không?
menu: số điện thoại đó có phải thuê bao đã đăng ký không?
```

Nếu xóa một thuê bao trong khi IMEI vẫn giữ số của thuê bao đó, quan hệ ngược bị mồ côi: `imei.soDienThoai` trỏ tới một thuê bao không còn tồn tại. Vì vậy, menu thuê bao phải từ chối xóa khi còn IMEI tham chiếu; không tự động tháo SIM nếu chính sách chưa yêu cầu.

## 9. Vị trí của `generateSimulatedImei()`

Hàm sinh IMEI mô phỏng hiện nằm trong anonymous namespace của `ThietBiIMEIMenu.cpp`. Đây là vị trí hợp lý vì:

- chỉ menu này đang gọi hàm;
- hàm phục vụ dữ liệu demo,không phải hành vi nội tại của thiết bị thật;
- hàm cần `Repository<ThietBiIMEI>` để kiểm tra tính duy nhất;
- model chỉ nên xác thực IMEI và bản ghi thiết bị,không nên biết repository dùng để cấp mã demo.

Hàm nên trả về **chuỗi IMEI 15 chữ số**,không nên tự tạo toàn bộ `ThietBiIMEI` vì lúc sinh mã chưa có tên thiết bị,nhà sản xuất,ngày kích hoạt,BTS và trạng thái đầy đủ.

Luồng đúng là:

```text
menu sinh chuỗi IMEI
  -> menu hỏi các trường còn lại
  -> constructor ThietBiIMEI kiểm tra bản ghi hoàn chỉnh
  -> repository.add() kiểm tra ID và lưu atomically
```

Hàm không nên chỉ trả về prefix 14 chữ số vì phần còn thiếu là chữ số kiểm tra cần thiết để tạo IMEI hoàn chỉnh.

## 10. Vì sao không dùng `while (true)` vô hạn

Không gian ứng viên hiện tại rất lớn nên va chạm thực tế thấp. Tuy nhiên,`while (true)` không có giới hạn vẫn là một điểm khó chứng minh:

- nếu quy tắc sinh thay đổi, vòng lặp có thể chạy mãi;
- nếu repository bị đầy hoặc dữ liệu bị lỗi,menu không có đường thoát rõ ràng;
- kiểm thử không thể chứng minh giới hạn thời gian.

Cách an toàn hơn là giới hạn số lần thử rồi báo lỗi rõ ràng. Khi hết số lần thử:

- không tạo `ThietBiIMEI`;
- không gọi `repository.add()`;
- không ghi tệp;
- menu thông báo không thể sinh IMEI mô phỏng và cho người dùng quay lại hoặc thử lại.

Không nên đổi IMEI 15 chữ số thành định dạng dài hơn để xử lý vấn đề này. Đó là thay đổi hợp đồng schema và phá vỡ dữ liệu hiện tại; nếu namespace thực sự không đủ thì phải thiết kế cơ chế cấp định danh mới có migration rõ ràng.

## 11. Câu trả lời ngắn khi bảo vệ

### Vì sao hàm sinh nằm ở menu?

> Vì đây là chức năng tạo dữ liệu demo của luồng tương tác và cần repository để kiểm tra mã đã tồn tại. Model chỉ chịu trách nhiệm xác thực và biểu diễn một thiết bị hoàn chỉnh.

### Luhn khác gì kiểm tra trùng?

> Luhn kiểm tra mã có đúng về mặt toán học hay không. Repository kiểm tra mã đó đã được sử dụng trong hệ thống hay chưa. Một mã có thể đúng Luhn nhưng vẫn bị từ chối vì trùng ID.

### Vì sao dùng tệp tạm?

> Tệp tạm là snapshot ứng viên. Hệ thống chỉ thay thế tệp thật sau khi serialize,flush và close đều thành công,nhờ đó tránh công bố tệp bị cắt ngắn.

### Vì sao cần rollback?

> Vì repository thay đổi bộ nhớ trước khi ghi đĩa. Nếu ghi thất bại, phải khôi phục collection trong bộ nhớ để bộ nhớ và tệp không mô tả hai trạng thái khác nhau.

### Nếu sinh IMEI thất bại thì sao?

> Dừng sau số lần thử hữu hạn,báo lỗi rõ ràng và không tạo hoặc lưu bản ghi dở dang.

## Source map

| Nội dung | Nguồn |
|---|---|
| Sinh IMEI mô phỏng | `src/lib/menus/ThietBiIMEIMenu.cpp` |
| Model và Luhn | `src/lib/models/imei.cpp`, `src/lib/models/imei.h` |
| Collection, load/save, rollback | `src/lib/shared/Repository.h` |
| Staging và commit tệp | `src/lib/shared/fileio.cpp` |
| Kiểm tra thuê bao tham chiếu | `src/lib/menus/ThueBaoMenu.cpp` |
| Regression tests | `tests/test_runner.cpp`, `tests/repro/` |
