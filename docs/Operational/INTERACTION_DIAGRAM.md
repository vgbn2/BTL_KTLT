# Sơ Đồ Tương Tác Hệ Thống (Interaction & Sequence Diagrams)
## Dự án: Quản Lý Thuê Bao Di Động (Topic 1 - PTIT KTLT)
### Phụ trách: Trần Đức Anh (B24DCVT021) | Nhánh: `DucAnh-B24DCVT021`

---

## 1. Tổng Quan Kiến Trúc Tương Tác Đa Tầng (Multi-Tier Interaction)

Hệ thống được thiết kế theo mô hình phân lớp rõ ràng (**Layered Architecture**), phân tách hoàn toàn giữa giao diện người dùng console, tầng xử lý nghiệp vụ, tầng xác thực dữ liệu và tầng lưu trữ tệp tin nguyên tử.

```mermaid
graph TD
    User([Người dùng / Giảng viên]) <-->|Nhập 0-9 / Xem bảng| Main[src/main.cpp - Menu Điều Khiển Chính]
    
    subgraph UI_Layer [Tầng Giao Diện Console UI - src/lib/menus/]
        Main <-->|Choice 3| HDMenu[HopDongMenu]
        Main <-->|Choice 4| IMEIMenu[ThietBiIMEIMenu]
        Main -.->|Choice 1,2,5,6| Stubs[Placeholder Stubs: goicuoc, thuebao, naptien, hoadon]
    end

    subgraph Validation_Layer [Tầng Xác Thực & Tiện Ích - src/lib/shared/]
        HDMenu & IMEIMenu -->|Nhập chuỗi / số an toàn| InputHelper[InputHelper]
        HDMenu & IMEIMenu -->|Xác thực lịch / Tuổi| DateEngine[Date Engine]
        InputHelper -->|Loại bỏ khoảng trắng / Tách chuỗi| FileIO[FileIO Helper]
        HDMenu & IMEIMenu -.->|Ném & Bắt lỗi| Exceptions[Custom Exceptions]
    end

    subgraph Domain_Layer [Tầng Nghiệp Vụ - src/lib/models/]
        HDMenu <-->|Khởi tạo / Gia hạn / Thanh lý| ModelHD[HopDong : Entity]
        IMEIMenu <-->|3GPP Luhn Mod-10 / EIR Blacklist| ModelIMEI[ThietBiIMEI : Entity]
    end

    subgraph Data_Layer [Tầng Lưu Trữ File Phẳng Nguyên Tử - src/lib/shared/]
        HDMenu <-->|CRUD trong RAM| RepoHD["Repository&lt;HopDong&gt;"]
        IMEIMenu <-->|CRUD trong RAM| RepoIMEI["Repository&lt;ThietBiIMEI&gt;"]
        RepoHD <-->|Đọc / Ghi nguyên tử .tmp| DiskHD[("data/hopdong.txt")]
        RepoIMEI <-->|Đọc / Ghi nguyên tử .tmp| DiskIMEI[("data/imei.txt")]
    end
```

---

## 2. Sơ Đồ Tuần Tự (Sequence Diagram) — UC01: Thêm Mới Hợp Đồng Đăng Ký

Quy trình người dùng tạo hợp đồng mới, xác thực số điện thoại 10 chữ số, ngày tháng hợp lệ và lưu trữ vào cơ sở dữ liệu:

```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as HopDongMenu
    participant Input as InputHelper
    participant DateMod as Date
    participant HD as HopDong
    participant Repo as Repository<HopDong>
    participant File as Disk (data/hopdong.txt)

    User->>Menu: Chọn [1] Thêm mới hợp đồng
    Menu->>Input: getString("Nhap ma hop dong")
    Input-->>Menu: "HD0011"
    
    Menu->>Repo: findById("HD0011")
    alt Mã trùng lặp
        Repo-->>Menu: Con trỏ != nullptr
        Menu-->>User: Báo lỗi "Ma hop dong da ton tai!"
    else Mã hợp lệ
        Repo-->>Menu: nullptr
    end

    Menu->>Input: getPhoneNumber("Nhap so dien thoai")
    Note over Input: Kiểm tra 10 chữ số & đầu số 03, 05, 07, 08, 09
    Input-->>Menu: "0912345678"

    Menu->>Input: getDate("Nhap ngay dang ky (DD/MM/YYYY)")
    Input->>DateMod: parse("01/10/2024")
    DateMod-->>Input: Date(01, 10, 2024)
    Input-->>Menu: ngayDangKy

    Menu->>Input: getDate("Nhap ngay het han (DD/MM/YYYY)")
    Input-->>Menu: ngayHetHan

    Menu->>HD: HopDong("HD0011", "KH001", "0912345678", ..., ngayDangKy, ngayHetHan, ...)
    Note over HD: Kiểm tra: ngayHetHan >= ngayDangKy & giaTri >= 0

    Menu->>Repo: add(newHopDong)
    Repo->>Repo: items.push_back(newHopDong)
    Repo->>File: saveToFile() [Ghi data/hopdong.txt.tmp -> rename sang data/hopdong.txt]
    File-->>Repo: Ghi file thành công
    Repo-->>Menu: return true
    Menu-->>User: "[THANH CONG] Da them hop dong moi vao he thong!"
```

---

## 3. Sơ Đồ Tuần Tự (Sequence Diagram) — UC02: Ghi Nhận IMEI & Kiểm Tra Chuẩn 3GPP Luhn

Quy trình nhập mã IMEI 15 chữ số, chạy thuật toán Luhn Mod-10, gán SIM và lưu trữ:

```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as ThietBiIMEIMenu
    participant Input as InputHelper
    participant IMEI as ThietBiIMEI
    participant Repo as Repository<ThietBiIMEI>
    participant File as Disk (data/imei.txt)

    User->>Menu: Chọn [1] Ghi nhận thiết bị IMEI mới
    Menu->>Input: getString("Nhap ma IMEI (15 chu so)")
    Input-->>Menu: "860123456789012"

    Menu->>IMEI: validateLuhn("860123456789012")
    Note over IMEI: Nhân đôi vị trí chẵn từ phải sang (Mod-10 Sum)
    alt Luhn Checksum Thất Bại
        IMEI-->>Menu: false
        Menu-->>User: "[LOI] Ma IMEI khong hop le theo tieu chuan 3GPP!"
    else Luhn Checksum Hợp Lệ
        IMEI-->>Menu: true
    end

    Menu->>Repo: findById("860123456789012")
    alt IMEI đã tồn tại
        Repo-->>Menu: Con trỏ != nullptr
        Menu-->>User: "[LOI] Ma IMEI da ton tai trong he thong!"
    else IMEI mới hợp lệ
        Repo-->>Menu: nullptr
    end

    Menu->>Input: getPhoneNumber("Nhap SDT gan SIM (de trong neu ChuaGan)")
    Input-->>Menu: "0987654321"

    Menu->>IMEI: ThietBiIMEI(imei, tenTB, hangSX, sdt, ngayKH, "HoatDong", bts)
    Menu->>Repo: add(newIMEI)
    Repo->>File: saveToFile() [Atomic rename]
    File-->>Repo: Ghi thành công
    Repo-->>Menu: return true
    Menu-->>User: "[THANH CONG] Da ghi nhan thiet bi IMEI thanh cong!"
```

---

## 4. Sơ Đồ Tương Tác Lưu Trữ Nguyên Tử (Atomic Persistence Engine)

Quy trình đảm bảo cơ sở dữ liệu file phẳng không bao giờ bị hỏng (corrupted) kể cả khi chương trình bị tắt đột ngột:

```mermaid
sequenceDiagram
    autonumber
    participant App as Ứng dụng / Repository
    participant TmpFile as Tệp tạm (data/hopdong.txt.tmp)
    participant LiveFile as Tệp chính (data/hopdong.txt)

    App->>TmpFile: 1. Mở tệp tạm data/hopdong.txt.tmp để ghi
    loop Ghi toàn bộ bản ghi trong RAM
        App->>TmpFile: 2. outFile << item.toFileString() << "\n"
    end
    App->>TmpFile: 3. outFile.close() (Xả toàn bộ buffer vào đĩa)
    App->>LiveFile: 4. std::remove("data/hopdong.txt")
    App->>LiveFile: 5. std::rename("data/hopdong.txt.tmp", "data/hopdong.txt")
    Note over LiveFile: Thao tác đổi tên nguyên tử (Atomic Replace) hoàn tất
```

---

## 5. Sơ Đồ Chuyển Trạng Thái Vòng Đời (State Transition Diagram)

### 5.1 Vòng Đời Hợp Đồng (`HopDong`)
```mermaid
stateDiagram-v2
    [*] --> HieuLuc : Khởi tạo hợp đồng mới (Add)
    HieuLuc --> TamDung : Tạm dừng dịch vụ (tamDung)
    TamDung --> HieuLuc : Kích hoạt lại (kichHoatLai)
    HieuLuc --> HieuLuc : Gia hạn ngày hết hạn (giaHan)
    HieuLuc --> ThanhLy : Chấm dứt / Thanh lý hợp đồng (chamDut)
    TamDung --> ThanhLy : Thanh lý hợp đồng
    ThanhLy --> [*] : Đóng hợp đồng vĩnh viễn
```

### 5.2 Vòng Đời Trạng Thái Thiết Bị IMEI (`ThietBiIMEI`)
```mermaid
stateDiagram-v2
    [*] --> HoatDong : Nhập thiết bị mới (Hợp lệ Luhn Mod-10)
    HoatDong --> KhoaMang : Phát hiện mất trộm / Vi phạm (Khóa EIR Blacklist)
    KhoaMang --> HoatDong : Mở khóa mạng (Gỡ khỏi Blacklist)
    HoatDong --> TamKhoa : Tạm khóa thuê bao
    TamKhoa --> HoatDong : Kích hoạt lại
```

---

## 6. Công Cụ Trực Quan Hóa Sơ Đồ Có Sẵn Trong Dự Án

Bên cạnh tài liệu Markdown này, dự án cung cấp 2 công cụ trực quan hóa tương tác:

1. **`graphify-out/KTLT-callflow.html`**:
   - Chứa 11 sơ đồ Mermaid và 10 bảng phân tích quan hệ gọi hàm (Call tables) tự động trích xuất từ toàn bộ mã nguồn C++.
   - Mở xem trực tiếp trên trình duyệt: `xdg-open graphify-out/KTLT-callflow.html`.
2. **`graphify-out/GRAPH_TREE.html`**:
   - Cây phân cấp mã nguồn tương tác D3.js dạng Collapsible Tree.
   - Mở xem: `xdg-open graphify-out/GRAPH_TREE.html`.
