#include "KhachHangMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <iostream>
#include <algorithm>

using namespace KhachHangMenuChoices;

//them khach hang moi va xac thuc thong tin
void KhachHangMenu::themKhachHang() {
    std::cout << "\n--- THEM KHACH HANG MOI (DANG KY THONG TIN EKYC) ---\n";
    std::string maKH = InputHelper::getString("Nhap ma khach hang (KH0000001-KH9999999): ");
    if (std::cin.eof() || maKH.empty()) return;
    maKH = Normalized::NormalizedCustomerId(maKH);
    if (!Normalized::isValidCustomerId(maKH)) {
        std::cout << "[LOI] Ma khach hang phai co dang KH0001 hoac KH0000001.\n";
        return;
    }
    if (store.findById(maKH) != nullptr) {
        std::cout << "[CANH BAO] Ma khach hang da ton tai trong he thong!\n";
        return;
    }

    std::string hoTen = InputHelper::getName("Nhap ho va ten: ");
    if (std::cin.eof() || hoTen.empty()) return;
    hoTen = Normalized::NormalizedName(hoTen);

    std::string loaiGT = "CCCD";
    std::string soGT = InputHelper::getString("Nhap so giay to CCCD (12 chu so): ");
    if (std::cin.eof() || soGT.empty()) return;
    if (!Normalized::isValidCredential(soGT)) {
        std::cout << "[LOI] So CCCD khong hop le.\n";
        return;
    }

    std::string ngayCap = InputHelper::getString("Nhap ngay cap (DD/MM/YYYY): ");
    if (std::cin.eof() || ngayCap.empty()) return;
    try {
        Date::parse(ngayCap);
    } catch (const std::exception& e) {
        std::cout << "[LOI] Ngay cap khong hop le: " << e.what() << "\n";
        return;
    }

    std::string noiCap = InputHelper::getString("Nhap noi cap (VD: Cuc C06): ");
    if (std::cin.eof() || noiCap.empty()) return;

    std::string sdt = InputHelper::getPhoneNumber("Nhap so dien thoai lien he (bo trong neu chua co): ", true);
    if (std::cin.eof()) return;

    std::string diaChi = InputHelper::getString("Nhap dia chi thuong tru: ");
    if (std::cin.eof() || diaChi.empty()) return;

    try {
        KhachHang kh(maKH, hoTen, loaiGT, soGT, ngayCap, noiCap, sdt, diaChi);
        store.add(kh);
        std::cout << "\n[THANH CONG] Da them khach hang " << hoTen << " (" << maKH << ") thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the them khach hang: " << e.what() << "\n";
    }
}

//hien thi danh sach khach hang
void KhachHangMenu::xemDanhSach() {
    std::cout << "\n--- DANH SACH KHACH HANG (KYC PROFILE) ---\n";
    if (store.empty()) {
        std::cout << "Chua co khach hang nao trong he thong.\n";
        return;
    }
    DisplayHelper::printTable(store.getAll());
    std::cout << "Tong so: " << store.size() << " khach hang.\n";
}

//tim kiem khach hang theo dieu kien
void KhachHangMenu::timKiemKhachHang() {
    std::cout << "\n--- TIM KIEM KHACH HANG ---\n"
              << "  [" << SEARCH_BY_ID << "] Tim theo Ma khach hang\n"
              << "  [" << SEARCH_BY_CCCD << "] Tim theo So CCCD\n"
              << "  [" << SEARCH_BY_NAME << "] Tim theo Ho ten\n"
              << "  [0] Quay lai\n";

    int choice = InputHelper::getInt("Nhap lua chon [0-3]: ", 0, 3);
    if (choice == 0 || std::cin.eof()) return;

    if (choice == SEARCH_BY_ID) {
        std::string maKH = InputHelper::getString("Nhap ma khach hang: ");
        maKH = Normalized::NormalizedCustomerId(maKH);
        KhachHang* kh = store.findById(maKH);
        if (kh == nullptr) {
            std::cout << "[THONG BAO] Khong tim thay khach hang: " << maKH << "\n";
        } else {
            kh->displayDetail();
        }
    } else if (choice == SEARCH_BY_CCCD) {
        std::string cccd = InputHelper::getString("Nhap so CCCD: ");
        auto results = store.filter([&cccd](const KhachHang& kh) {
            return kh.getSoGiayTo() == cccd;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong tim thay khach hang co CCCD: " << cccd << "\n";
        } else {
            DisplayHelper::printTable(results);
        }
    } else if (choice == SEARCH_BY_NAME) {
        std::string keyword = InputHelper::getString("Nhap tu khoa ten khach hang: ");
        std::string normKeyword = Normalized::toLower(Normalized::trim(keyword));
        auto results = store.filter([&normKeyword](const KhachHang& kh) {
            std::string nameNorm = Normalized::toLower(kh.getHoTen());
            return nameNorm.find(normKeyword) != std::string::npos;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong tim thay khach hang phu hop.\n";
        } else {
            DisplayHelper::printTable(results);
        }
    }
}

//sap xep danh sach khach hang theo ma
void KhachHangMenu::sapXepDanhSach() {
    std::cout << "\n--- SAP XEP DANH SACH KHACH HANG ---\n"
              << "  [" << SORT_BY_ID_ASC << "] Ma khach hang tang dan\n"
              << "  [" << SORT_BY_ID_DESC << "] Ma khach hang giam dan\n";
    int choice = InputHelper::getInt("Nhap kieu sap xep [1-2]: ", SORT_BY_ID_ASC, SORT_BY_ID_DESC);
    try {
        store.sortById(choice == SORT_BY_ID_ASC);
        std::cout << "[THANH CONG] Da sap xep va luu danh sach khach hang.\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thu tu sap xep: " << e.what() << "\n";
    }
    xemDanhSach();
}

//cap nhat thong tin khach hang
void KhachHangMenu::capNhatKhachHang() {
    std::cout << "\n--- CAP NHAT THONG TIN KHACH HANG ---\n";
    std::string maKH = InputHelper::getString("Nhap ma khach hang can sua: ");
    if (std::cin.eof()) return;
    maKH = Normalized::NormalizedCustomerId(maKH);
    KhachHang* kh = store.findById(maKH);
    if (kh == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay khach hang co ma: " << maKH << "\n";
        return;
    }

    kh->displayDetail();
    KhachHang updated = *kh;

    std::string hoTenMoi = InputHelper::getName("Nhap ho ten moi (bo trong de giu nguyen): ", true);
    if (std::cin.eof()) return;
    if (!hoTenMoi.empty()) updated.setHoTen(Normalized::NormalizedName(hoTenMoi));

    std::string sdtMoi = InputHelper::getPhoneNumber("Nhap SĐT moi (bo trong de giu nguyen): ", true);
    if (std::cin.eof()) return;
    if (!sdtMoi.empty() && sdtMoi != InputLimits::UNASSIGNED_PHONE_TAG) updated.setSoDienThoai(sdtMoi);

    std::string diaChiMoi = InputHelper::getString("Nhap dia chi moi (bo trong de giu nguyen): ", true);
    if (std::cin.eof()) return;
    if (!diaChiMoi.empty()) updated.setDiaChi(diaChiMoi);

    try {
        store.update(maKH, updated);
        std::cout << "\n[THANH CONG] Da cap nhat thong tin khach hang " << maKH << "!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the cap nhat: " << e.what() << "\n";
    }
}

//xoa khach hang va kiem tra khoa ngoai
void KhachHangMenu::xoaKhachHang() {
    std::cout << "\n--- XOA KHACH HANG KHOI HE THONG ---\n";
    std::string maKH = InputHelper::getString("Nhap ma khach hang can xoa: ");
    if (std::cin.eof()) return;
    maKH = Normalized::NormalizedCustomerId(maKH);
    KhachHang* kh = store.findById(maKH);
    if (kh == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay khach hang: " << maKH << "\n";
        return;
    }

    // Kiem tra rang buoc toan ven khoa ngoai: khong duoc xoa neu con thue bao hoac hop dong
    auto activeSubscribers = thueBaoStore.filter([&maKH](const ThueBao& tb) {
        return Normalized::NormalizedCustomerId(tb.getMaKhachHang()) == maKH;
    });
    if (!activeSubscribers.empty()) {
        std::cout << "[LOI] Khong the xoa khach hang! Hien dang co " << activeSubscribers.size()
                  << " thue bao dang lien ket voi khach hang nay.\n";
        return;
    }

    auto activeContracts = hopDongStore.filter([&maKH](const HopDong& hd) {
        return Normalized::NormalizedCustomerId(hd.getMaKhachHang()) == maKH;
    });
    if (!activeContracts.empty()) {
        std::cout << "[LOI] Khong the xoa khach hang! Hien dang co " << activeContracts.size()
                  << " hop dong dang lien ket voi khach hang nay.\n";
        return;
    }

    kh->displayDetail();
    if (InputHelper::getConfirm("Ban co chac chan muon xoa khach hang nay khoi he thong?")) {
        try {
            store.remove(maKH);
            std::cout << "\n[THANH CONG] Da xoa khach hang " << maKH << " thanh cong!\n";
        } catch (const std::exception& e) {
            std::cout << "\n[LOI] Khong the xoa: " << e.what() << "\n";
        }
    } else {
        std::cout << "Da huy thao tac xoa.\n";
    }
}

//hien thi danh sach SIM cua khach hang
void KhachHangMenu::xemDanhSachSIM() {
    std::cout << "\n--- TRA CUU DANH SACH SIM / THUE BAO CHINH CHU ---\n";
    std::string maKH = InputHelper::getString("Nhap ma khach hang: ");
    if (std::cin.eof()) return;
    maKH = Normalized::NormalizedCustomerId(maKH);
    KhachHang* kh = store.findById(maKH);
    if (kh == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay khach hang co ma: " << maKH << "\n";
        return;
    }

    kh->displayDetail();
    auto sims = thueBaoStore.filter([&maKH](const ThueBao& tb) {
        return Normalized::NormalizedCustomerId(tb.getMaKhachHang()) == maKH;
    });

    if (sims.empty()) {
        std::cout << "[THONG BAO] Khach hang chua dang ky so thue bao nao tren he thong.\n";
    } else {
        std::cout << "\nDanh sach " << sims.size() << " thue bao / SIM so huu:\n";
        DisplayHelper::printTable(sims);
    }
}

//hien thi menu quan ly khach hang
void KhachHangMenu::showMenu() {
    while (true) {
        std::cout << "\n====================================================================\n"
                  << "             QUAN LY KHACH HANG & SIM CHINH CHU (EKYC)\n"
                  << "====================================================================\n"
                  << "  [" << MENU_ADD << "] Dang ky khach hang moi (CCCD / Giay to)\n"
                  << "  [" << MENU_LIST << "] Xem danh sach khach hang\n"
                  << "  [" << MENU_SEARCH << "] Tra cuu khach hang theo Ma KH / CCCD / Ten\n"
                  << "  [" << MENU_UPDATE << "] Cap nhat thong tin khach hang\n"
                  << "  [" << MENU_DELETE << "] Xoa khach hang (co kiem tra khoa ngoai)\n"
                  << "  [" << MENU_VIEW_SIMS << "] Tra cuu danh sach SIM / Thue bao cua khach hang\n"
                  << "  [" << MENU_SORT << "] Sap xep khach hang theo Ma KH\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon [0-7]: ", MENU_BACK, MENU_SORT);
        if (choice == MENU_BACK || std::cin.eof()) {
            break;
        }

        switch (choice) {
            case MENU_ADD:
                themKhachHang();
                break;
            case MENU_LIST:
                xemDanhSach();
                break;
            case MENU_SEARCH:
                timKiemKhachHang();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            case MENU_UPDATE:
                capNhatKhachHang();
                break;
            case MENU_DELETE:
                xoaKhachHang();
                break;
            case MENU_VIEW_SIMS:
                xemDanhSachSIM();
                break;
            default:
                break;
        }
    }
}
