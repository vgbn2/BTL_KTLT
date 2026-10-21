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
    bool changed = false;
    for (auto& tb : thueBaoStore.getAll()) {
        const bool hasDebt = std::find(no.begin(), no.end(), tb.getSoDienThoai()) != no.end();
        if (hasDebt && tb.getTrangThai() != ThueBaoConstants::STATUS_LOCKED_DEBT) {
            tb.setTrangThai(ThueBaoConstants::STATUS_LOCKED_DEBT);
            changed = true;
        } else if (!hasDebt && tb.getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT) {
            tb.setTrangThai(ThueBaoConstants::STATUS_ACTIVE);
            changed = true;
        }
    }
    if (changed) {
        thueBaoStore.saveToFile();
    }
    return static_cast<int>(no.size());
}

//lap hoa don cuoc phat sinh
void HoaDonMenu::lapHoaDon() {
    std::cout << "\n--- LAP HOA DON CUOC PHAT SINH TRONG THANG ---\n";
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
        std::cout << "\n[THANH CONG] Da lap hoa don " << maHD << " thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the lap hoa don: " << e.what() << "\n";
    }
}

//hien thi danh sach hoa don
void HoaDonMenu::xemDanhSachHoaDon() {
    std::cout << "\n--- DANH SACH HOA DON CUOC ---\n";
    if (hoaDonStore.empty()) {
        std::cout << "Chua co hoa don nao trong he thong.\n";
        return;
    }
    DisplayHelper::printTable(hoaDonStore.getAll());
    std::cout << "Tong so: " << hoaDonStore.size() << " hoa don.\n";
}

//thanh toan hoa don
void HoaDonMenu::thanhToanHoaDon() {
    std::cout << "\n--- THANH TOAN HOA DON CUOC ---\n";
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
            if (tb != nullptr && tb->getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT &&
                kiemTraNo(Date::now()) == 0) {
                tb->setTrangThai(ThueBaoConstants::STATUS_ACTIVE);
                thueBaoStore.saveToFile();
            }
            std::cout << "\n[THANH CONG] Da thanh toan hoa don " << maHD << " thanh cong!\n";
        } catch (const std::exception& e) {
            std::cout << "\n[LOI] Khong the cap nhat trang thai thanh toan: " << e.what() << "\n";
        }
    } else {
        std::cout << "Da huy thao tac thanh toan.\n";
    }
}

//tra cuu hoa don theo so dien thoai
void HoaDonMenu::traCuuTheoSoDienThoai() {
    std::cout << "\n--- TRA CUU HOA DON THEO SO THUE BAO ---\n";
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

//tiep nhan phieu khieu nai
void HoaDonMenu::tiepNhanKhieuNai() {
    std::cout << "\n--- TIEP NHAN PHIEU KHIEU NAI / TRA SOAT CUOC ---\n";
    std::string maKN = InputHelper::getString("Nhap ma phieu khiieu nai (VD: KN0001): ");
    if (std::cin.eof() || maKN.empty()) return;
    if (khieuNaiStore.findById(maKN) != nullptr) {
        std::cout << "[CANH BAO] Ma phieu khiieu nai da ton tai!\n";
        return;
    }

    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai phan anh: ", false);
    if (std::cin.eof() || sdt.empty()) return;
    if (thueBaoStore.findById(sdt) == nullptr) {
        std::cout << "[LOI] Thue bao '" << sdt << "' khong ton tai tren he thong!\n";
        return;
    }

    std::cout << "Chon loai khiieu nai:\n"
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

    std::string ngayGui = InputHelper::getString("Nhap ngay tiep nhan (DD/MM/YYYY): ");
    if (std::cin.eof() || ngayGui.empty()) return;
    try {
        Date::parse(ngayGui);
    } catch (const std::exception& e) {
        std::cout << "[LOI] " << e.what() << "\n";
        return;
    }

    std::string noiDung = InputHelper::getString("Nhap noi dung phan anh chi tiet: ");
    if (std::cin.eof() || noiDung.empty()) return;

    try {
        KhieuNai kn(maKN, sdt, loaiKN, ngayGui, noiDung, KhieuNaiConstants::STATUS_RECEIVED);
        khieuNaiStore.add(kn);
        std::cout << "\n[THANH CONG] Da tiep nhan phieu khiieu nai " << maKN << " vao he thong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the luu phieu khiieu nai: " << e.what() << "\n";
    }
}

//xu ly phieu khieu nai
void HoaDonMenu::xuLyKhieuNai() {
    std::cout << "\n--- XU LY / TRA SOAT PHIEU KHIEU NAI ---\n";
    std::string maKN = InputHelper::getString("Nhap ma phieu khiieu nai can xu ly: ");
    if (std::cin.eof() || maKN.empty()) return;
    KhieuNai* kn = khieuNaiStore.findById(maKN);
    if (kn == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay phieu khiieu nai: " << maKN << "\n";
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
        std::cout << "\n[THANH CONG] Da cap nhat trang thai phieu khiieu nai thanh: " << newStatus << "!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the cap nhat: " << e.what() << "\n";
    }
}

//hien thi danh sach khieu nai
void HoaDonMenu::xemDanhSachKhieuNai() {
    std::cout << "\n--- DANH SACH PHIEU KHIEU NAI / TRA SOAT ---\n";
    if (khieuNaiStore.empty()) {
        std::cout << "Chua co phieu khiieu nai nao trong he thong.\n";
        return;
    }
    DisplayHelper::printTable(khieuNaiStore.getAll());
    std::cout << "Tong so: " << khieuNaiStore.size() << " phieu khiieu nai.\n";
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
                  << "  [" << MENU_ADD_COMPLAINT << "] Tiep nhan phieu khiieu nai / tra soat cuoc\n"
                  << "  [" << MENU_PROCESS_COMPLAINT << "] Xu ly / Cap nhat trang thai khiieu nai\n"
                  << "  [" << MENU_LIST_COMPLAINTS << "] Xem danh sach phieu khiieu nai\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon [0-7]: ", MENU_BACK, MENU_LIST_COMPLAINTS);
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
                default:
                break;
        }
    }
}
