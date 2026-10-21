#include "GoiCuocMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <iostream>
#include <algorithm>

using namespace GoiCuocMenuChoices;

//them goi cuoc moi
void GoiCuocMenu::themGoiCuoc() {
    std::cout << "\n--- THEM GOI CUOC MOI ---\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc: ");
    if (std::cin.eof() || maGoi.empty()) return;
    if (store.findById(maGoi) != nullptr) {
        std::cout << "[CANH BAO] Ma goi cuoc da ton tai!\n";
        return;
    }

    std::string tenGoi = InputHelper::getString("Nhap ten goi cuoc: ");
    if (std::cin.eof() || tenGoi.empty()) return;
    double giaCuoc = InputHelper::getDouble("Nhap gia cuoc (VND): ", 0.0);
    if (std::cin.eof()) return;
    int data = InputHelper::getInt("Nhap dung luong data (GB): ", 0);
    if (std::cin.eof()) return;
    int phut = InputHelper::getInt("Nhap so phut thoai: ", 0);
    if (std::cin.eof()) return;
    int chuKy = InputHelper::getInt("Nhap chu ky ngay: ", 1, 365);
    if (std::cin.eof()) return;

    try {
        GoiCuoc gc(maGoi, tenGoi, giaCuoc, data, phut, chuKy);
        store.add(gc);
        std::cout << "\n[THANH CONG] Da them goi cuoc thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the them goi cuoc: " << e.what() << "\n";
    }
}

//hien thi danh sach goi cuoc
void GoiCuocMenu::xemDanhSach() {
    std::cout << "\n--- DANH SACH GOI CUOC HIEN CO ---\n";
    if (store.empty()) {
        std::cout << "Chua co goi cuoc nao trong he thong.\n";
        return;
    }
    store.getAll()[0].displayHeader();
    for (const auto& gc : store.getAll()) {
        gc.displayRow();
    }
}

//tim kiem goi cuoc theo tu khoa
void GoiCuocMenu::timKiemGoiCuoc() {
    std::cout << "\n--- TIM KIEM GOI CUOC ---\n";
    std::string keyword = InputHelper::getString("Nhap ma goi hoac ten goi can tim: ");
    std::string normKeyword = Normalized::toLower(Normalized::trim(keyword));

    auto results = store.filter([&normKeyword](const GoiCuoc& gc) {
        std::string maNorm = Normalized::toLower(gc.getMaGoi());
        std::string tenNorm = Normalized::toLower(gc.getTenGoi());
        return maNorm.find(normKeyword) != std::string::npos ||
               tenNorm.find(normKeyword) != std::string::npos;
    });

    if (results.empty()) {
        std::cout << "Khong tim thay goi cuoc phu hop voi tu khoa.\n";
        return;
    }

    results[0].displayHeader();
    for (const auto& gc : results) {
        gc.displayRow();
    }
}

//sap xep danh sach goi cuoc
void GoiCuocMenu::sapXepDanhSach() {
    std::cout << "\n--- SAP XEP DANH SACH GOI CUOC ---\n";
    std::cout << "  [" << SORT_BY_PRICE_ASC << "] Gia cuoc tang dan\n";
    std::cout << "  [" << SORT_BY_PRICE_DESC << "] Gia cuoc giam dan\n";
    std::cout << "  [" << SORT_BY_DATA_DESC << "] Dung luong Data giam dan\n";

    int choice = InputHelper::getInt("Nhap kieu sap xep: ", 1, 3);
    if (choice == SORT_BY_PRICE_ASC) {
        store.sort([](const GoiCuoc& a, const GoiCuoc& b) {
            return a.getGiaCuoc() < b.getGiaCuoc();
        });
    } else if (choice == SORT_BY_PRICE_DESC) {
        store.sort([](const GoiCuoc& a, const GoiCuoc& b) {
            return a.getGiaCuoc() > b.getGiaCuoc();
        });
    } else if (choice == SORT_BY_DATA_DESC) {
        store.sort([](const GoiCuoc& a, const GoiCuoc& b) {
            return a.getDungLuongData() > b.getDungLuongData();
        });
    }
    std::cout << "\n[THANH CONG] Da sap xep danh sach goi cuoc!\n";
    xemDanhSach();
}

//cap nhat thong tin goi cuoc
void GoiCuocMenu::capNhatGoiCuoc() {
    std::cout << "\n--- CAP NHAT GOI CUOC ---\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc can sua: ");
    if (std::cin.eof()) return;
    GoiCuoc* gc = store.findById(maGoi);
    if (gc == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay goi cuoc co ma: " << maGoi << "\n";
        return;
    }

    gc->displayDetail();
    GoiCuoc updated = *gc;
    std::string tenMoi = InputHelper::getString("Nhap ten goi moi (bo trong de giu nguyen): ", true);
    if (std::cin.eof()) return;
    if (!tenMoi.empty()) updated.setTenGoi(tenMoi);

    std::cout << "Ban co muon thay doi gia cuoc khong? (1: Co, 0: Khong): ";
    int doiGia = InputHelper::getInt("", 0, 1);
    if (std::cin.eof()) return;
    if (doiGia == 1) {
        double giaMoi = InputHelper::getDouble("Nhap gia cuoc moi (VND): ", 0.0);
        if (std::cin.eof()) return;
        updated.setGiaCuoc(giaMoi);
    }

    try {
        //cap nhat ban sao,dam bao rollback neu loi
        store.update(maGoi, updated);
        std::cout << "\n[THANH CONG] Da cap nhat goi cuoc!\n";
    } catch (const std::exception& e) {
        std::cout << "\n[LOI] Khong the luu thay doi: " << e.what() << "\n";
    }
}

//xoa goi cuoc khoi he thong
void GoiCuocMenu::xoaGoiCuoc() {
    std::cout << "\n--- XOA GOI CUOC ---\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc can xoa: ");
    if (std::cin.eof()) return;
    GoiCuoc* gc = store.findById(maGoi);
    if (gc == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay goi cuoc co ma: " << maGoi << "\n";
        return;
    }

    gc->displayDetail();
    int confirm = InputHelper::getInt("Xac nhan xoa goi cuoc nay? (1: Dong y, 0: Huy): ", 0, 1);
    if (std::cin.eof()) return;
    if (confirm == 1) {
        try {
            store.remove(maGoi);
            std::cout << "\n[THANH CONG] Da xoa goi cuoc thanh cong!\n";
        } catch (const std::exception& e) {
            std::cout << "\n[LOI] Khong the xoa goi cuoc: " << e.what() << "\n";
        }
    } else {
        std::cout << "Da huy thao tac xoa.\n";
    }
}

//hien thi menu quan ly goi cuoc
void GoiCuocMenu::showMenu() {
    while (true) {
        std::cout << "\n====================================================================\n"
                  << "                      QUAN LY GOI CUOC DI DONG\n"
                  << "====================================================================\n"
                  << "  [" << MENU_ADD << "] Them goi cuoc moi\n"
                  << "  [" << MENU_LIST << "] Xem danh sach goi cuoc\n"
                  << "  [" << MENU_SEARCH << "] Tim kiem goi cuoc theo ten / ma\n"
                  << "  [" << MENU_SORT << "] Sap xep danh sach goi cuoc (gia / dung luong)\n"
                  << "  [" << MENU_UPDATE << "] Cap nhat thong tin goi cuoc\n"
                  << "  [" << MENU_DELETE << "] Xoa goi cuoc\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon [0-6]: ", MENU_BACK, MENU_DELETE);
        if (choice == MENU_BACK) {
            break;
        }

        switch (choice) {
            case MENU_ADD:
                themGoiCuoc();
                break;
            case MENU_LIST:
                xemDanhSach();
                break;
            case MENU_SEARCH:
                timKiemGoiCuoc();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            case MENU_UPDATE:
                capNhatGoiCuoc();
                break;
            case MENU_DELETE:
                xoaGoiCuoc();
                break;
            default:
                break;
        }
        InputHelper::pause();
    }
}
