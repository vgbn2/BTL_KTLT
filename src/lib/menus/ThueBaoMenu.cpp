#include "ThueBaoMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <iostream>

using namespace ThueBaoMenuChoices;

//them thue bao moi va kiem tra lien ket
void ThueBaoMenu::themThueBao() {
    std::cout << "\n--- THEM THUE BAO MOI ---\n";
    std::string sdt = InputHelper::getString("Nhap so dien thoai (10 so): ");
    if (std::cin.eof() || sdt.empty()) return;
    if (!Normalized::isValidPhoneNumber(sdt)) {
        std::cout << "[LOI] So dien thoai khong hop le theo quy chuan Viet Nam!\n";
        return;
    }
    if (store.findById(sdt) != nullptr) {
        std::cout << "[CANH BAO] So dien thoai da ton tai trong he thong!\n";
        return;
    }

    std::cout << "Chon loai thue bao:\n  [1] Tra truoc (Prepaid)\n  [2] Tra sau (Postpaid)\n";
    int loaiChoice = InputHelper::getInt("Nhap lua chon [1-2]: ", 1, 2);
    if (std::cin.eof()) return;
    std::string loaiTB = (loaiChoice == 1) ? ThueBaoConstants::TYPE_PREPAID : ThueBaoConstants::TYPE_POSTPAID;

    std::string maKH = InputHelper::getString("Nhap ma khach hang so huu (vd KH0000001): ");
    if (std::cin.eof() || maKH.empty()) return;
    //kiem tra khoa ngoai khach hang,tranh tham chieu sai
    if (khachHangStore.findById(maKH) == nullptr) {
        std::cout << "[LOI] Ma khach hang khong ton tai trong he thong!\n";
        return;
    }

    std::string maGC = InputHelper::getString("Nhap ma goi cuoc ban dau (hoac bo trong): ", true);
    if (std::cin.eof()) return;
    if (!maGC.empty() && goiCuocStore.findById(maGC) == nullptr) {
        std::cout << "[LOI] Ma goi cuoc khong ton tai trong he thong!\n";
        return;
    }

    std::string ngayKH = InputHelper::getString("Nhap ngay kich hoat (DD/MM/YYYY): ");
    if (std::cin.eof() || ngayKH.empty()) return;
    try {
        Date::parse(ngayKH);
    } catch (const std::exception& e) {
        std::cout << "[LOI] " << e.what() << "\n";
        return;
    }

    double soDu = InputHelper::getDouble("Nhap so du ban dau (VND): ", 0.0);
    if (std::cin.eof()) return;

    try {
        ThueBao tb(sdt, loaiTB, maKH, maGC, ngayKH, ThueBaoConstants::STATUS_ACTIVE, soDu);
        store.add(tb);
        std::cout << "\n[THANH CONG] Da them thue bao moi thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the them thue bao: " << e.what() << "\n";
    }
}

//hien thi danh sach thue bao
void ThueBaoMenu::xemDanhSach() {
    std::cout << "\n--- DANH SACH THUE BAO DI DONG ---\n";
    if (store.empty()) {
        std::cout << "Chua co thue bao nao trong he thong.\n";
        return;
    }
    store.getAll()[0].displayHeader();
    for (const auto& tb : store.getAll()) {
        tb.displayRow();
    }
}

//tim kiem thue bao theo tu khoa
void ThueBaoMenu::timKiemThueBao() {
    std::cout << "\n--- TIM KIEM THUE BAO ---\n";
    std::string keyword = InputHelper::getString("Nhap so dien thoai hoac ma khach hang can tim: ");
    std::string normKeyword = Normalized::toLower(Normalized::trim(keyword));

    auto results = store.filter([&normKeyword](const ThueBao& tb) {
        std::string sdtNorm = Normalized::toLower(tb.getSoDienThoai());
        std::string khNorm = Normalized::toLower(tb.getMaKhachHang());
        return sdtNorm.find(normKeyword) != std::string::npos ||
               khNorm.find(normKeyword) != std::string::npos;
    });

    if (results.empty()) {
        std::cout << "Khong tim thay thue bao phu hop.\n";
        return;
    }

    results[0].displayHeader();
    for (const auto& tb : results) {
        tb.displayRow();
    }
}

//dang ky goi cuoc cho thue bao
void ThueBaoMenu::dangKyGoiCuoc() {
    std::cout << "\n--- DANG KY / DOI GOI CUOC CHO THUE BAO ---\n";
    std::string sdt = InputHelper::getString("Nhap so dien thoai thue bao: ");
    if (std::cin.eof()) return;
    ThueBao* tb = store.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay thue bao co so: " << sdt << "\n";
        return;
    }

    tb->displayDetail();
    std::string maGC = InputHelper::getString("Nhap ma goi cuoc moi muon dang ky: ");
    if (std::cin.eof()) return;
    GoiCuoc* gc = goiCuocStore.findById(maGC);
    if (gc == nullptr) {
        std::cout << "[CANH BAO] Ma goi cuoc khong ton tai trong he thong!\n";
        return;
    }

    ThueBao updated = *tb;
    //kiem tra so du cho thue bao tra truoc
    if (updated.getLoaiThueBao() == ThueBaoConstants::TYPE_PREPAID) {
        if (updated.getSoDuTaiKhoan() < gc->getGiaCuoc()) {
            std::cout << "[LOI] So du tai khoan (" << updated.getSoDuTaiKhoan()
                      << " VND) khong du de dang ky goi cuoc (" << gc->getGiaCuoc() << " VND)!\n";
            return;
        }
        updated.truTien(gc->getGiaCuoc());
    }

    updated.setMaGoiCuoc(maGC);
    try {
        //luu ban sao,tranh sua truc tiep khi loi
        store.update(sdt, updated);
        std::cout << "\n[THANH CONG] Dang ky goi cuoc " << gc->getTenGoi() << " (" << maGC
                  << ") cho thue bao " << sdt << " thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the luu thay doi: " << e.what() << "\n";
    }
}

//cap nhat trang thai khoa mo chieu thue bao
void ThueBaoMenu::khoaMoChieu() {
    std::cout << "\n--- KHOA / MO CHIEU HOAT DONG THUE BAO ---\n";
    std::string sdt = InputHelper::getString("Nhap so dien thoai: ");
    if (std::cin.eof()) return;
    ThueBao* tb = store.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay thue bao co so: " << sdt << "\n";
        return;
    }

    std::cout << "Trang thai hien tai: " << tb->getTrangThai() << "\n";
    std::cout << "Chon trang thai moi:\n"
              << "  [1] Hoat dong binh thuong (" << ThueBaoConstants::STATUS_ACTIVE << ")\n"
              << "  [2] Khoa 1 chieu (" << ThueBaoConstants::STATUS_LOCKED_1WAY << ")\n"
              << "  [3] Khoa 2 chieu (" << ThueBaoConstants::STATUS_LOCKED_2WAY << ")\n"
              << "  [4] Huy thue bao (" << ThueBaoConstants::STATUS_CANCELLED << ")\n";

    int choice = InputHelper::getInt("Nhap lua chon [1-4]: ", 1, 4);
    if (std::cin.eof()) return;
    std::string newStatus;
    if (choice == 1) newStatus = ThueBaoConstants::STATUS_ACTIVE;
    else if (choice == 2) newStatus = ThueBaoConstants::STATUS_LOCKED_1WAY;
    else if (choice == 3) newStatus = ThueBaoConstants::STATUS_LOCKED_2WAY;
    else if (choice == 4) newStatus = ThueBaoConstants::STATUS_CANCELLED;

    ThueBao updated = *tb;
    updated.setTrangThai(newStatus);
    try {
        store.update(sdt, updated);
        std::cout << "\n[THANH CONG] Da cap nhat trang thai thue bao thanh: " << newStatus << "\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the luu thay doi: " << e.what() << "\n";
    }
}

//nap tien vao tai khoan thue bao
void ThueBaoMenu::napTien() {
    std::cout << "\n--- NAP TIEN VAO TAI KHOAN THUE BAO ---\n";
    std::string sdt = InputHelper::getString("Nhap so dien thoai can nap: ");
    if (std::cin.eof()) return;
    ThueBao* tb = store.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay thue bao co so: " << sdt << "\n";
        return;
    }

    std::cout << "So du hien tai: " << tb->getSoDuTaiKhoan() << " VND\n";
    double soTien = InputHelper::getDouble("Nhap so tien muon nap (VND): ", 1000.0);
    if (std::cin.eof()) return;
    ThueBao updated = *tb;
    updated.congTien(soTien);

    try {
        store.update(sdt, updated);
        std::cout << "\n[THANH CONG] Da nap " << soTien << " VND thanh cong! So du moi: "
                  << updated.getSoDuTaiKhoan() << " VND\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the luu thay doi: " << e.what() << "\n";
    }
}

//xoa thue bao khoi he thong
void ThueBaoMenu::xoaThueBao() {
    std::cout << "\n--- XOA THUE BAO ---\n";
    std::string sdt = InputHelper::getString("Nhap so dien thoai can xoa: ");
    if (std::cin.eof()) return;
    ThueBao* tb = store.findById(sdt);
    if (tb == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay thue bao: " << sdt << "\n";
        return;
    }

    tb->displayDetail();
    int confirm = InputHelper::getInt("Xac nhan xoa thue bao nay khoi he thong? (1: Co, 0: Khong): ", 0, 1);
    if (std::cin.eof()) return;
    if (confirm == 1) {
        try {
            store.remove(sdt);
            std::cout << "\n[THANH CONG] Da xoa thue bao khoi he thong!\n";
        } catch (const std::exception& e) {
            std::cout << "\n[LOI] Khong the xoa thue bao: " << e.what() << "\n";
        }
    }
}

//hien thi menu quan ly thue bao
void ThueBaoMenu::showMenu() {
    while (true) {
        std::cout << "\n====================================================================\n"
                  << "                     QUAN LY THUE BAO DI DONG\n"
                  << "====================================================================\n"
                  << "  [" << MENU_ADD << "] Them thue bao moi\n"
                  << "  [" << MENU_LIST << "] Xem danh sach thue bao\n"
                  << "  [" << MENU_SEARCH << "] Tim kiem thue bao theo SĐT / Ma KH\n"
                  << "  [" << MENU_REGISTER_PACKAGE << "] Dang ky / Doi goi cuoc cho thue bao\n"
                  << "  [" << MENU_LOCK_UNLOCK << "] Khoa / Mo chieu thue bao\n"
                  << "  [" << MENU_TOPUP << "] Nap tien vao tai khoan thue bao\n"
                  << "  [" << MENU_DELETE << "] Xoa thue bao\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon [0-7]: ", MENU_BACK, MENU_DELETE);
        if (choice == MENU_BACK) {
            break;
        }

        switch (choice) {
            case MENU_ADD:
                themThueBao();
                break;
            case MENU_LIST:
                xemDanhSach();
                break;
            case MENU_SEARCH:
                timKiemThueBao();
                break;
            case MENU_REGISTER_PACKAGE:
                dangKyGoiCuoc();
                break;
            case MENU_LOCK_UNLOCK:
                khoaMoChieu();
                break;
            case MENU_TOPUP:
                napTien();
                break;
            case MENU_DELETE:
                xoaThueBao();
                break;
            default:
                break;
        }
        InputHelper::pause();
    }
}
