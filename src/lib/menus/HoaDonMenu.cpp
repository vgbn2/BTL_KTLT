#include "HoaDonMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <algorithm>
#include <iostream>

using namespace HoaDonMenuChoices;

//kiem tra va khoa thue bao no cuoc
int HoaDonMenu::kiemTraNo(const Date& ngay) {
    std::vector<std::string> no;
    for (const auto& hd : hoaDonStore.getAll()) {
        ThueBao* tb = thueBaoStore.findById(hd.getSoDienThoai());
        if (tb == nullptr || tb->getLoaiThueBao() != ThueBaoConstants::TYPE_POSTPAID ||
            hd.getTrangThai() != HoaDonConstants::STATUS_UNPAID || !hd.quaHan(ngay)) {
            continue;
        }
        if (std::find(no.begin(), no.end(), tb->getSoDienThoai()) == no.end()) {
            no.push_back(tb->getSoDienThoai());
        }
    }
    for (const auto& tb : thueBaoStore.getAll()) {
        const bool hasDebt = std::find(no.begin(), no.end(), tb.getSoDienThoai()) != no.end();
        if (hasDebt && tb.getTrangThai() != ThueBaoConstants::STATUS_LOCKED_DEBT) {
            ThueBao updatedTb = tb;
            updatedTb.setTrangThai(ThueBaoConstants::STATUS_LOCKED_DEBT);
            thueBaoStore.update(tb.getId(), updatedTb);
        } else if (!hasDebt && tb.getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT) {
            ThueBao updatedTb = tb;
            updatedTb.setTrangThai(ThueBaoConstants::STATUS_ACTIVE);
            thueBaoStore.update(tb.getId(), updatedTb);
        }
    }
    return static_cast<int>(no.size());
}

//lap hoa don cuoc phat sinh
void HoaDonMenu::lapHoaDon() {
    std::cout << "\n>>> LAP HOA DON CUOC PHAT SINH TRONG THANG <<<\n";
    std::string maHD = InputHelper::getString("Nhap ma hoa don (VD: HD11): ");
    if (std::cin.eof() || maHD.empty()) return;
    if (hoaDonStore.findById(maHD) != nullptr) {
        std::cout << "[CANH BAO] Ma hoa don da ton tai trong he thong!\n";
        return;
    }

    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai thue bao: ", false);
    if (std::cin.eof() || sdt.empty()) return;
    ThueBao* tb = thueBaoStore.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[LOI] Thue bao '" << sdt << "' chua duoc dang ky tren he thong!\n";
        return;
    }

    std::string kyCuoc = InputHelper::getString("Nhap ky cuoc (MM/YYYY): ");
    if (std::cin.eof() || kyCuoc.empty()) return;

    // Tra cuu va hien thi cuoc phat sinh thuc te tu CDR trong ky cuoc
    double tienGoiCDR = 0.0, tienDataCDR = 0.0;
    for (const auto& cdr : cdrStore.getAll()) {
        // thoiGianBatDau co dang "DD/MM/YYYY HH:MM:SS", lay MM/YYYY tu vi tri 3
        if (cdr.getSoGoi() != sdt) continue;
        if (cdr.getThoiGianBatDau().size() < 10) continue;
        if (cdr.getThoiGianBatDau().substr(3, 7) != kyCuoc) continue;
        if (cdr.getLoaiDichVu() == CDRConstants::TYPE_DATA)
            tienDataCDR += cdr.getCuocPhatSinh();
        else
            tienGoiCDR += cdr.getCuocPhatSinh();
    }

    std::cout << "Thong ke cuoc tu ban ghi CDR ky " << kyCuoc << ":\n"
              << "  - Cuoc thoai/SMS tu CDR: " << DisplayHelper::formatCurrency(tienGoiCDR, "VND") << "\n"
              << "  - Cuoc data tu CDR:      " << DisplayHelper::formatCurrency(tienDataCDR, "VND") << "\n"
              << "  - Tong cuoc tu CDR:      " << DisplayHelper::formatCurrency(tienGoiCDR + tienDataCDR, "VND") << "\n";

    double tienGoi = InputHelper::getDouble("Nhap tien cuoc goi co ban (VND): ", 0.0);
    if (std::cin.eof()) return;

    double tienData = InputHelper::getDouble("Nhap tien cuoc data phat sinh (VND): ", 0.0);
    if (std::cin.eof()) return;

    double tongTien = tienGoi + tienData;
    std::cout << "Tong tien cuoc can thanh toan: "
              << DisplayHelper::formatCurrency(tongTien, "VND") << "\n";

    try {
        HoaDon hd(maHD, sdt, kyCuoc, tienGoi, tienData, tongTien, HoaDonConstants::STATUS_UNPAID);
        hoaDonStore.add(hd);
        std::cout << "[THANH CONG] Da lap hoa don " << maHD << " thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the lap hoa don: " << e.what() << "\n";
    }
}

//hien thi danh sach hoa don
void HoaDonMenu::xemDanhSachHoaDon() {
    std::cout << "\n>>> DANH SACH HOA DON CUOC <<<\n";
    if (hoaDonStore.empty()) {
        std::cout << "[THONG BAO] Danh sach hoa don hien dang trong!\n";
        return;
    }
    DisplayHelper::printTable(hoaDonStore.getAll());
    std::cout << "Tong so: " << hoaDonStore.size() << " hoa don.\n";
}

//thanh toan hoa don
void HoaDonMenu::thanhToanHoaDon() {
    std::cout << "\n>>> THANH TOAN HOA DON CUOC <<<\n";
    std::string maHD = InputHelper::getString("Nhap ma hoa don can thanh toan: ");
    if (std::cin.eof() || maHD.empty()) return;
    HoaDon* hd = hoaDonStore.findById(maHD);
    if (hd == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay hoa don: " << maHD << "\n";
        return;
    }

    hd->displayDetail();
    if (hd->getTrangThai() == HoaDonConstants::STATUS_PAID) {
        std::cout << "[THONG BAO] Hoa don nay da duoc thanh toan truoc do!\n";
        return;
    }

    if (InputHelper::getConfirm("Xac nhan thanh toan hoa don nay?")) {
        HoaDon updated = *hd;
        updated.thanhToan();
        try {
            hoaDonStore.update(maHD, updated);
            ThueBao* tb = thueBaoStore.findById(updated.getSoDienThoai());
            if (tb != nullptr && tb->getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT) {
                bool hasRemainingDebt = false;
                for (const auto& h : hoaDonStore.getAll()) {
                    if (h.getSoDienThoai() == updated.getSoDienThoai() &&
                        h.getTrangThai() == HoaDonConstants::STATUS_UNPAID &&
                        h.quaHan(Date::now())) {
                        hasRemainingDebt = true;
                        break;
                    }
                }
                if (!hasRemainingDebt) {
                    ThueBao updatedTb = *tb;
                    updatedTb.setTrangThai(ThueBaoConstants::STATUS_ACTIVE);
                    thueBaoStore.update(tb->getId(), updatedTb);
                }
            }
            std::cout << "[THANH CONG] Da thanh toan hoa don " << maHD << " thanh cong!\n";
        } catch (const std::exception& e) {
            std::cout << "[LOI] Khong the cap nhat trang thai thanh toan: " << e.what() << "\n";
        }
    } else {
        std::cout << "[DA HUY] Thao tac thanh toan da duoc huy bo.\n";
    }
}

//tra cuu hoa don theo so dien thoai
void HoaDonMenu::traCuuTheoSoDienThoai() {
    std::cout << "\n>>> TRA CUU HOA DON THEO SO THUE BAO <<<\n";
    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai: ", false);
    if (std::cin.eof() || sdt.empty()) return;

    auto results = hoaDonStore.filter([&sdt](const HoaDon& hd) {
        return hd.getSoDienThoai() == sdt;
    });

    if (results.empty()) {
        std::cout << "[THONG BAO] Khong co hoa don nao cua thue bao: " << sdt << "\n";
    } else {
        std::cout << "\nTim thay " << results.size() << " hoa don:\n";
        DisplayHelper::printTable(results);
    }
}

//sap xep hoa don hoac khieu nai theo ma
void HoaDonMenu::sapXepDanhSach() {
    std::cout << "\n>>> SAP XEP HOA DON / KHIEU NAI <<<\n"
              << "  [" << SORT_INVOICE_ID_ASC << "] Ma hoa don tang dan\n"
              << "  [" << SORT_INVOICE_ID_DESC << "] Ma hoa don giam dan\n"
              << "  [" << SORT_COMPLAINT_ID_ASC << "] Ma khieu nai tang dan\n"
              << "  [" << SORT_COMPLAINT_ID_DESC << "] Ma khieu nai giam dan\n"
              << "  [0] Quay lai\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-4]: ", 0, SORT_COMPLAINT_ID_DESC);
    if (choice == 0) return;
    try {
        if (choice == SORT_INVOICE_ID_ASC || choice == SORT_INVOICE_ID_DESC) {
            hoaDonStore.sortById(choice == SORT_INVOICE_ID_ASC);
        } else {
            khieuNaiStore.sortById(choice == SORT_COMPLAINT_ID_ASC);
        }
        std::cout << "[THANH CONG] Da sap xep va luu du lieu.\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thu tu sap xep: " << e.what() << "\n";
    }
    if (choice == SORT_INVOICE_ID_ASC || choice == SORT_INVOICE_ID_DESC) {
        xemDanhSachHoaDon();
    } else {
        xemDanhSachKhieuNai();
    }
}

//tiep nhan phieu khieu nai
void HoaDonMenu::tiepNhanKhieuNai() {
    std::cout << "\n>>> TIEP NHAN PHIEU KHIEU NAI / TRA SOAT CUOC <<<\n";
    std::string maKN = InputHelper::getString("Nhap ma phieu khieu nai (VD: KN0001): ");
    if (std::cin.eof() || maKN.empty()) return;
    if (khieuNaiStore.findById(maKN) != nullptr) {
        std::cout << "[CANH BAO] Ma phieu khieu nai da ton tai!\n";
        return;
    }

    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai phan anh: ", false);
    if (std::cin.eof() || sdt.empty()) return;
    if (thueBaoStore.findById(sdt) == nullptr) {
        std::cout << "[LOI] Thue bao '" << sdt << "' khong ton tai tren he thong!\n";
        return;
    }

    std::cout << "Chon loai khieu nai:\n"
              << "  [1] " << KhieuNaiConstants::TYPE_BILLING << " (Cuoc phat sinh bat thuong)\n"
              << "  [2] " << KhieuNaiConstants::TYPE_NETWORK << " (Chat luong mang/song kem)\n"
              << "  [3] " << KhieuNaiConstants::TYPE_CARD << " (Loi the cao/nap tien)\n"
              << "  [4] " << KhieuNaiConstants::TYPE_SERVICE << " (Khong dung duoc goi cuoc)\n";
    int typeChoice = InputHelper::getInt("Nhap lua chon [1-4]: ", 1, 4);
    if (std::cin.eof()) return;
    std::string loaiKN = (typeChoice == 1) ? KhieuNaiConstants::TYPE_BILLING
                       : (typeChoice == 2) ? KhieuNaiConstants::TYPE_NETWORK
                       : (typeChoice == 3) ? KhieuNaiConstants::TYPE_CARD
                                           : KhieuNaiConstants::TYPE_SERVICE;

    std::string ngayGui = InputHelper::getString("Nhap ngay tiep nhan (DD/MM/YYYY, 'now' hoac '0' de huy): ");
    if (ngayGui == "0" || std::cin.eof()) {
        std::cout << "[DA HUY] Thao tac tiep nhan khieu nai da duoc huy bo.\n";
        return;
    }
    try {
        ngayGui = Date::parse(ngayGui).toString();
    } catch (const std::exception& e) {
        std::cout << "[LOI] " << e.what() << "\n";
        return;
    }

    std::string noiDung = InputHelper::getString("Nhap noi dung phan anh chi tiet: ");
    if (std::cin.eof() || noiDung.empty()) return;

    try {
        KhieuNai kn(maKN, sdt, loaiKN, ngayGui, noiDung, KhieuNaiConstants::STATUS_RECEIVED);
        khieuNaiStore.add(kn);
        std::cout << "[THANH CONG] Da tiep nhan phieu khieu nai " << maKN << " vao he thong!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu phieu khieu nai: " << e.what() << "\n";
    }
}

//xu ly phieu khieu nai
void HoaDonMenu::xuLyKhieuNai() {
    std::cout << "\n>>> XU LY / TRA SOAT PHIEU KHIEU NAI <<<\n";
    std::string maKN = InputHelper::getString("Nhap ma phieu khieu nai can xu ly: ");
    if (std::cin.eof() || maKN.empty()) return;
    KhieuNai* kn = khieuNaiStore.findById(maKN);
    if (kn == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay phieu khieu nai: " << maKN << "\n";
        return;
    }

    kn->displayDetail();
    std::cout << "\nChon trang thai xu ly moi:\n"
              << "  [1] " << KhieuNaiConstants::STATUS_PROCESSING << " (Dang tra soat ky thuat)\n"
              << "  [2] " << KhieuNaiConstants::STATUS_RESOLVED << " (Da giai quyet / Hoan cuoc)\n"
              << "  [3] " << KhieuNaiConstants::STATUS_REJECTED << " (Tu choi / Khong hop le)\n";
    int stChoice = InputHelper::getInt("Nhap lua chon [1-3]: ", 1, 3);
    if (std::cin.eof()) return;
    std::string newStatus = (stChoice == 1) ? KhieuNaiConstants::STATUS_PROCESSING
                          : (stChoice == 2) ? KhieuNaiConstants::STATUS_RESOLVED
                                            : KhieuNaiConstants::STATUS_REJECTED;

    KhieuNai updated = *kn;
    updated.setTrangThai(newStatus);
    try {
        khieuNaiStore.update(maKN, updated);
        std::cout << "[THANH CONG] Da cap nhat trang thai phieu khieu nai thanh: " << newStatus << "!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the cap nhat: " << e.what() << "\n";
    }
}

//hien thi danh sach khieu nai
void HoaDonMenu::xemDanhSachKhieuNai() {
    std::cout << "\n>>> DANH SACH PHIEU KHIEU NAI / TRA SOAT <<<\n";
    if (khieuNaiStore.empty()) {
        std::cout << "[THONG BAO] Danh sach phieu khieu nai hien dang trong!\n";
        return;
    }
    DisplayHelper::printTable(khieuNaiStore.getAll());
    std::cout << "Tong so: " << khieuNaiStore.size() << " phieu khieu nai.\n";
}

//hien thi menu quan ly hoa don va khieu nai
void HoaDonMenu::showMenu() {
    while (true) {
        std::cout << "\n====================================================================\n"
                  << "              QUAN LY HOA DON CUOC & PHIEU KHIEU NAI\n"
                  << "====================================================================\n"
                  << "  [" << MENU_CREATE_INVOICE << "] Lap hoa don cuoc moi trong thang\n"
                  << "  [" << MENU_LIST_INVOICES << "] Xem danh sach tat ca hoa don\n"
                  << "  [" << MENU_PAY_INVOICE << "] Thanh toan hoa don cuoc\n"
                  << "  [" << MENU_SEARCH_BY_PHONE << "] Tra cuu hoa don theo so dien thoai\n"
                  << "  [" << MENU_ADD_COMPLAINT << "] Tiep nhan phieu khieu nai / tra soat cuoc\n"
                  << "  [" << MENU_PROCESS_COMPLAINT << "] Xu ly / Cap nhat trang thai khieu nai\n"
                  << "  [" << MENU_LIST_COMPLAINTS << "] Xem danh sach phieu khieu nai\n"
                  << "  [" << MENU_SORT << "] Sap xep hoa don / khieu nai theo ma\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-8]: ", MENU_BACK, MENU_SORT);
        if (choice == MENU_BACK || std::cin.eof()) {
            break;
        }

        switch (choice) {
            case MENU_CREATE_INVOICE:
                lapHoaDon();
                break;
            case MENU_LIST_INVOICES:
                xemDanhSachHoaDon();
                break;
            case MENU_PAY_INVOICE:
                thanhToanHoaDon();
                break;
            case MENU_SEARCH_BY_PHONE:
                traCuuTheoSoDienThoai();
                break;
            case MENU_ADD_COMPLAINT:
                tiepNhanKhieuNai();
                break;
            case MENU_PROCESS_COMPLAINT:
                xuLyKhieuNai();
                break;
            case MENU_LIST_COMPLAINTS:
                xemDanhSachKhieuNai();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            default:
                break;
        }
    }
}
