#include "NapTienMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <iostream>

using namespace NapTienMenuChoices;

//lap phieu nap tien va cap nhat so du
void NapTienMenu::lapPhieuNapTien() {
    std::cout << "\n>>> LAP PHIEU NAP TIEN VAO TAI KHOAN <<<\n";
    std::string maGD = InputHelper::getString("Nhap ma giao dich (VD: GD0010): ");
    if (std::cin.eof() || maGD.empty()) return;
    if (napTienStore.findById(maGD) != nullptr) {
        std::cout << "[CANH BAO] Ma giao dich da ton tai trong he thong!\n";
        return;
    }

    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai can nap: ", false);
    if (std::cin.eof() || sdt.empty()) return;
    ThueBao* tb = thueBaoStore.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[LOI] So thue bao '" << sdt << "' chua duoc dang ky tren he thong!\n";
        return;
    }

    std::cout << "So du hien tai cua thue bao " << sdt << ": "
              << DisplayHelper::formatCurrency(tb->getSoDuTaiKhoan(), "VND") << "\n";

    double soTien = InputHelper::getDouble("Nhap so tien nap (VND): ", 10000.0);
    if (std::cin.eof()) return;

    std::cout << "Chon hinh thuc nap:\n"
              << "  [1] " << NapTienConstants::METHOD_SCRATCH_CARD << " (The cao)\n"
              << "  [2] " << NapTienConstants::METHOD_BANKING << " (Ngan hang)\n"
              << "  [3] " << NapTienConstants::METHOD_E_WALLET << " (Vi dien tu)\n";
    int hinhThucChoice = InputHelper::getInt("Nhap lua chon [1-3]: ", 1, 3);
    if (std::cin.eof()) return;
    std::string hinhThuc = (hinhThucChoice == 1) ? NapTienConstants::METHOD_SCRATCH_CARD
                         : (hinhThucChoice == 2) ? NapTienConstants::METHOD_BANKING
                                                 : NapTienConstants::METHOD_E_WALLET;

    std::string ngayNap = InputHelper::getString("Nhap ngay nap (DD/MM/YYYY, 'now' hoac '0' de huy): ");
    if (ngayNap == "0" || std::cin.eof()) {
        std::cout << "[DA HUY] Thao tac nap tien da duoc huy bo.\n";
        return;
    }
    try {
        ngayNap = Date::parse(ngayNap).toString();
    } catch (const std::exception& e) {
        std::cout << "[LOI] " << e.what() << "\n";
        return;
    }

    try {
        NapTien nt(maGD, sdt, ngayNap, soTien, hinhThuc, NapTienConstants::STATUS_SUCCESS);
        napTienStore.add(nt);

        // Cong tien truc tiep vao so du thue bao
        ThueBao updated = *tb;
        updated.congTien(soTien);
        thueBaoStore.update(sdt, updated);

        std::cout << "[THANH CONG] Da lap phieu nap tien " << maGD << " thanh cong!\n"
                  << "So du moi cua thue bao " << sdt << ": "
                  << DisplayHelper::formatCurrency(updated.getSoDuTaiKhoan(), "VND") << "\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the thuc hien nap tien: " << e.what() << "\n";
    }
}

//hien thi danh sach phieu nap tien
void NapTienMenu::xemDanhSachPhieu() {
    std::cout << "\n>>> DANH SACH PHIEU NAP TIEN <<<\n";
    if (napTienStore.empty()) {
        std::cout << "[THONG BAO] Danh sach phieu nap tien hien dang trong!\n";
        return;
    }
    DisplayHelper::printTable(napTienStore.getAll());
    std::cout << "Tong so: " << napTienStore.size() << " phieu nap tien.\n";
}

//tra cuu lich su nap tien theo so dien thoai
void NapTienMenu::traCuuTheoSoDienThoai() {
    std::cout << "\n>>> TRA CUU LICH SU NAP TIEN THEO SO THUE BAO <<<\n";
    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai: ", false);
    if (std::cin.eof() || sdt.empty()) return;

    auto results = napTienStore.filter([&sdt](const NapTien& nt) {
        return nt.getSoDienThoai() == sdt;
    });

    if (results.empty()) {
        std::cout << "[THONG BAO] Khong co giao dich nap tien nao cua thue bao: " << sdt << "\n";
    } else {
        std::cout << "\nTim thay " << results.size() << " giao dich nap tien:\n";
        DisplayHelper::printTable(results);
    }
}

//sap xep phieu nap hoac CDR theo ma
void NapTienMenu::sapXepDanhSach() {
    std::cout << "\n>>> SAP XEP DU LIEU NAP TIEN / CDR <<<\n"
              << "  [" << SORT_NAPTIEN_ID_ASC << "] Ma giao dich nap tien tang dan\n"
              << "  [" << SORT_NAPTIEN_ID_DESC << "] Ma giao dich nap tien giam dan\n"
              << "  [" << SORT_CDR_ID_ASC << "] Ma CDR tang dan\n"
              << "  [" << SORT_CDR_ID_DESC << "] Ma CDR giam dan\n"
              << "  [0] Quay lai\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-4]: ", 0, SORT_CDR_ID_DESC);
    if (choice == 0) return;
    try {
        if (choice == SORT_NAPTIEN_ID_ASC || choice == SORT_NAPTIEN_ID_DESC) {
            napTienStore.sortById(choice == SORT_NAPTIEN_ID_ASC);
        } else {
            cdrStore.sortById(choice == SORT_CDR_ID_ASC);
        }
        std::cout << "[THANH CONG] Da sap xep va luu du lieu.\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thu tu sap xep: " << e.what() << "\n";
    }
    if (choice == SORT_NAPTIEN_ID_ASC || choice == SORT_NAPTIEN_ID_DESC) {
        xemDanhSachPhieu();
    } else {
        xemDanhSachCDR();
    }
}

//ghi nhan ban ghi cuoc goi hoac phien data
void NapTienMenu::ghiNhanCDR() {
    std::cout << "\n>>> GHI NHAN BAN GHI CUOC GOI / PHIEN DATA (CDR) <<<\n";
    std::string maCDR = InputHelper::getString("Nhap ma ban ghi CDR (VD: CDR0001): ");
    if (std::cin.eof() || maCDR.empty()) return;
    if (cdrStore.findById(maCDR) != nullptr) {
        std::cout << "[CANH BAO] Ma ban ghi CDR da ton tai!\n";
        return;
    }

    std::string soGoi = InputHelper::getPhoneNumber("Nhap so thue bao chu goi: ", false);
    if (std::cin.eof() || soGoi.empty()) return;
    ThueBao* tbGoi = thueBaoStore.findById(soGoi);
    if (tbGoi == nullptr) {
        std::cout << "[LOI] Thue bao chu goi '" << soGoi << "' khong ton tai trong he thong!\n";
        return;
    }

    std::cout << "Chon loai dich vu CDR:\n"
              << "  [1] " << CDRConstants::TYPE_VOICE << " (Cuoc goi thoai)\n"
              << "  [2] " << CDRConstants::TYPE_SMS << " (Tin nhan SMS)\n"
              << "  [3] " << CDRConstants::TYPE_DATA << " (Truy cap Data)\n";
    int typeChoice = InputHelper::getInt("Nhap lua chon [1-3]: ", 1, 3);
    if (std::cin.eof()) return;
    std::string loaiDV = (typeChoice == 1) ? CDRConstants::TYPE_VOICE
                       : (typeChoice == 2) ? CDRConstants::TYPE_SMS
                                           : CDRConstants::TYPE_DATA;

    std::string soNhan = "DATA_SESSION";
    if (loaiDV != CDRConstants::TYPE_DATA) {
        soNhan = InputHelper::getPhoneNumber("Nhap so thue bao nhan: ", false);
        if (std::cin.eof() || soNhan.empty()) return;
    }

    std::string timeStr = InputHelper::getString("Nhap thoi gian bat dau (DD/MM/YYYY HH:MM:SS): ");
    if (std::cin.eof() || timeStr.empty()) return;

    int duration = InputHelper::getInt((loaiDV == CDRConstants::TYPE_DATA ? "Nhap dung luong (MB): " : "Nhap thoi luong (giay): "), 0);
    if (std::cin.eof()) return;

    double cost = InputHelper::getDouble("Nhap cuoc phat sinh (VND): ", 0.0);
    if (std::cin.eof()) return;

    try {
        CDR cdr(maCDR, soGoi, soNhan, loaiDV, timeStr, duration, cost);
        cdrStore.add(cdr);

        // Neu thue bao tra truoc va co cuoc phat sinh -> tru tien tai khoan
        if (tbGoi->getLoaiThueBao() == ThueBaoConstants::TYPE_PREPAID && cost > 0.0) {
            ThueBao updated = *tbGoi;
            if (updated.getSoDuTaiKhoan() >= cost) {
                updated.truTien(cost);
                thueBaoStore.update(soGoi, updated);
                std::cout << "[THONG TIN] Da tru " << DisplayHelper::formatCurrency(cost, "VND")
                          << " tu tai khoan. So du con lai: "
                          << DisplayHelper::formatCurrency(updated.getSoDuTaiKhoan(), "VND") << "\n";
            }
        }

        std::cout << "[THANH CONG] Da ghi nhan ban ghi CDR " << maCDR << " vao he thong!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the ghi nhan CDR: " << e.what() << "\n";
    }
}

//hien thi danh sach ban ghi CDR
void NapTienMenu::xemDanhSachCDR() {
    std::cout << "\n>>> DANH SACH BAN GHI CUOC GOI / PHIEN DATA (CDR) <<<\n";
    if (cdrStore.empty()) {
        std::cout << "[THONG BAO] Danh sach ban ghi CDR hien dang trong!\n";
        return;
    }
    DisplayHelper::printTable(cdrStore.getAll());
    std::cout << "Tong so: " << cdrStore.size() << " ban ghi CDR.\n";
}

//hien thi menu nap tien va CDR
void NapTienMenu::showMenu() {
    while (true) {
        std::cout << "\n====================================================================\n"
                  << "          QUAN LY PHIEU NAP TIEN & BAN GHI CUOC GOI (CDR)\n"
                  << "====================================================================\n"
                  << "  [" << MENU_TOPUP << "] Lap phieu nap tien (The cao, Banking, Vi)\n"
                  << "  [" << MENU_LIST_VOUCHERS << "] Xem danh sach phieu nap tien\n"
                  << "  [" << MENU_SEARCH_BY_PHONE << "] Tra cuu lich su nap tien theo so thue bao\n"
                  << "  [" << MENU_ADD_CDR << "] Ghi nhan ban ghi cuoc goi / SMS / Data (CDR)\n"
                  << "  [" << MENU_LIST_CDR << "] Xem danh sach nhat ky cuoc goi CDR\n"
                  << "  [" << MENU_SORT << "] Sap xep phieu nap / CDR theo ma\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-6]: ", MENU_BACK, MENU_SORT);
        if (choice == MENU_BACK || std::cin.eof()) {
            break;
        }

        switch (choice) {
            case MENU_TOPUP:
                lapPhieuNapTien();
                break;
            case MENU_LIST_VOUCHERS:
                xemDanhSachPhieu();
                break;
            case MENU_SEARCH_BY_PHONE:
                traCuuTheoSoDienThoai();
                break;
            case MENU_ADD_CDR:
                ghiNhanCDR();
                break;
            case MENU_LIST_CDR:
                xemDanhSachCDR();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            default:
                break;
        }
    }
}
