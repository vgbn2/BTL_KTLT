# Sơ Đồ Tương Tác Hệ Thống
## Dự án: Quản Lý Thuê Bao Di Động - PTIT KTLT
### Phụ trách: Trần Đức Anh - B24DCVT021 | Nhánh: DucAnh-B24DCVT021

---

> [Thông tin] Đặc tả luồng gọi hàm chi tiết: Xem tài liệu [FUNCTION_CALLFLOW_SPEC.md](FUNCTION_CALLFLOW_SPEC.md).
> [Thông tin] Công cụ xem sơ đồ tương tác đa màn hình: [INTERACTIVE_DIAGRAMS.html](../Display/INTERACTIVE_DIAGRAMS.html).

---

## 1. Tổng Quan Kiến Trúc Tương Tác Đa Tầng

Hệ thống được thiết kế theo mô hình phân lớp rõ ràng, phân tách hoàn toàn giữa giao diện người dùng console, tầng xử lý nghiệp vụ, tầng xác thực dữ liệu và tầng lưu trữ tệp tin nguyên tử.

```mermaid
flowchart TD
    User([Nguoi dung / Giang vien]) -->|Nhap lua chon 0-6| Main["src/main.cpp (Menu Chinh)"]

    subgraph UI ["1. TANG GIAO DIEN CONSOLE (src/lib/menus/)"]
        direction TB
        Main -->|Choice 3| HDMenu["HopDongMenu"]
        Main -->|Choice 4| IMEIMenu["ThietBiIMEIMenu"]
        Main -.->|Choice 1,2,5,6| Stubs["Stubs: GoiCuoc, ThueBao, NapTien, HoaDon"]
    end

    subgraph Core ["2. TANG NGHIEP VU & TIEN ICH XAC THUC (src/lib/models/ & src/lib/shared/)"]
        direction TB
        subgraph Models ["Mo Hinh Nghiep Vu"]
            direction TB
            HDModel["HopDong : Entity"]
            IMEIModel["ThietBiIMEI : Entity (Luhn Mod-10)"]
        end
        subgraph Helpers ["Xac Thuc & Tien Ich"]
            direction TB
            InputHelper["InputHelper (Loc '|', EOF Safe)"]
            DateEngine["Date Engine (Lich Nhuan & Tuoi)"]
            FileIO["FileIO Helper (Trim / Split)"]
            InputHelper --> DateEngine
            InputHelper --> FileIO
        end
        HDMenu -->|Khoi tao / Gia han| HDModel
        IMEIMenu -->|Gan SIM / Khoa may| IMEIModel
        HDMenu & IMEIMenu -->|Nhap du lieu an toan| InputHelper
    end

    subgraph Storage ["3. TANG LUU TRU NGUYEN TU (src/lib/shared/ & data/)"]
        direction TB
        subgraph StorageHD ["Phan He Hop Dong"]
            direction TB
            RepoHD["DataStore&lt;HopDong&gt;"]
            DiskHD[("data/hopdong.txt")]
            RepoHD -->|Ghi .tmp -> Rename| DiskHD
        end
        subgraph StorageIMEI ["Phan He Thiet Bi IMEI"]
            direction TB
            RepoIMEI["DataStore&lt;ThietBiIMEI&gt;"]
            DiskIMEI[("data/imei.txt")]
            RepoIMEI -->|Ghi .tmp -> Rename| DiskIMEI
        end
    end

    HDMenu -->|CRUD trong RAM & Sync| RepoHD
    IMEIMenu -->|CRUD trong RAM & Sync| RepoIMEI
```

---

## 2. Sơ Đồ Tuần Tự — UC01: Thêm Mới Hợp Đồng Đăng Ký

Quy trình người dùng tạo hợp đồng mới, xác thực số điện thoại 10 chữ số, ngày tháng hợp lệ và lưu trữ vào cơ sở dữ liệu:

```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as HopDongMenu
    participant Input as InputHelper
    participant DateMod as Date
    participant HD as HopDong
    participant Repo as DataStore<HopDong>
    participant File as Disk

    User->>Menu: Chọn 1 Thêm mới hợp đồng
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
    Input-->>Menu: "0912345678"

    Menu->>Input: getDate("Nhap ngay dang ky")
    Input->>DateMod: parse("01/10/2024")
    DateMod-->>Input: Date(01, 10, 2024)
    Input-->>Menu: ngayDangKy

    Menu->>Input: getDate("Nhap ngay het han")
    Input-->>Menu: ngayHetHan

    Menu->>HD: HopDong("HD0011", "KH001", "0912345678", ..., ngayDangKy, ngayHetHan, ...)
    Menu->>Repo: add(newHopDong)
    Repo->>Repo: items.push_back(newHopDong)
    Repo->>File: saveToFile()
    File-->>Repo: Ghi file thành công
    Repo-->>Menu: return true
    Menu-->>User: "[THANH CONG] Da them hop dong moi vao he thong!"
```

---

## 3. Sơ Đồ Tuần Tự — UC02: Ghi Nhận IMEI & Kiểm Tra Chuẩn 3GPP Luhn

Quy trình nhập mã IMEI 15 chữ số, chạy thuật toán Luhn Mod-10, gán SIM và lưu trữ:

```mermaid
sequenceDiagram
    autonumber
    actor User as Người dùng
    participant Menu as ThietBiIMEIMenu
    participant Input as InputHelper
    participant IMEI as ThietBiIMEI
    participant Repo as DataStore<ThietBiIMEI>
    participant File as Disk

    User->>Menu: Chọn 1 Ghi nhận thiết bị IMEI mới
    Menu->>Input: getString("Nhap ma IMEI")
    Input-->>Menu: "860123456789012"

    Menu->>IMEI: validateLuhn("860123456789012")
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

    Menu->>Input: getPhoneNumber("Nhap SDT gan SIM")
    Input-->>Menu: "0987654321"

    Menu->>IMEI: ThietBiIMEI(imei, tenTB, hangSX, sdt, ngayKH, "HoatDong", bts)
    Menu->>Repo: add(newIMEI)
    Repo->>File: saveToFile()
    File-->>Repo: Ghi thành công
    Repo-->>Menu: return true
    Menu-->>User: "[THANH CONG] Da ghi nhan thiet bi IMEI thanh cong!"
```

---

## 4. Sơ Đồ Tương Tác Lưu Trữ
Quy trình đảm bảo cơ sở dữ liệu file phẳng không bao giờ bị hỏng kể cả khi chương trình bị tắt đột ngột:

```mermaid
sequenceDiagram
    autonumber
    participant App as Ứng dụng / DataStore
    participant TmpFile as Tệp tạm
    participant LiveFile as Tệp chính

    App->>TmpFile: 1. Mở tệp tạm để ghi
    loop Ghi toàn bộ bản ghi trong RAM
        App->>TmpFile: 2. outFile << item.toFileString() << "\n"
    end
    App->>TmpFile: 3. outFile.close()
    App->>LiveFile: 4. std::remove("data/hopdong.txt")
    App->>LiveFile: 5. std::rename("data/hopdong.txt.tmp", "data/hopdong.txt")
```

---

## 5. Sơ Đồ Chuyển Trạng Thái Vòng Đời

### 5.1 Vòng Đời Hợp Đồng
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

### 5.2 Vòng Đời Trạng Thái Thiết Bị IMEI
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
