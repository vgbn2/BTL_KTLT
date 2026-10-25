/**
 * @file GoiCuocMenu.cpp
 * Thanh vien phu trach: Tùng Dương (B24DCVT106)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#include "GoiCuocMenu.h"
#include "InputHelper.h"
#include "DisplayHelper.h"
#include "Normalized.h"
#include <iostream>
#include <algorithm>

using namespace GoiCuocMenuChoices;

//them goi cuoc moi
void GoiCuocMenu::themGoiCuoc() {
    std::cout << "\n>>> THEM GOI CUOC MOI <<<\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc: ");
    if (std::cin.eof() || maGoi.empty()) return;
    if (store.findById(maGoi) != nullptr) {
        std::cout << "[CANH BAO] Ma goi cuoc da ton tai!\n";
        return;
    }

    std::string tenGoi = InputHelper::getName("Nhap ten goi cuoc: ");
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
        std::cout << "[THANH CONG] Da them goi cuoc thanh cong!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the them goi cuoc: " << e.what() << "\n";
    }
}

//hien thi danh sach goi cuoc
void GoiCuocMenu::xemDanhSach() {
    std::cout << "\n>>> DANH SACH GOI CUOC HIEN CO <<<\n";
    if (store.empty()) {
        std::cout << "[THONG BAO] Danh sach goi cuoc hien dang trong!\n";
        return;
    }
    DisplayHelper::printTable(store.getAll());
    std::cout << "Tong so: " << store.size() << " goi cuoc.\n";
}

//tim kiem goi cuoc theo tu khoa
void GoiCuocMenu::timKiemGoiCuoc() {
    std::cout << "\n>>> TIM KIEM GOI CUOC <<<\n";
    std::string keyword = InputHelper::getString("Nhap ma goi hoac ten goi can tim: ");
    std::string normKeyword = Normalized::toLower(Normalized::trim(keyword));

    auto results = store.filter([&normKeyword](const GoiCuoc& gc) {
        std::string maNorm = Normalized::toLower(gc.getMaGoi());
        std::string tenNorm = Normalized::toLower(gc.getTenGoi());
        return maNorm.find(normKeyword) != std::string::npos ||
               tenNorm.find(normKeyword) != std::string::npos;
    });

    if (results.empty()) {
        std::cout << "[THONG BAO] Khong tim thay goi cuoc phu hop voi tu khoa.\n";
        return;
    }

    DisplayHelper::printTable(results);
    std::cout << "Tim thay " << results.size() << " goi cuoc.\n";
}

//sap xep danh sach goi cuoc
void GoiCuocMenu::sapXepDanhSach() {
    std::cout << "\n>>> SAP XEP DANH SACH GOI CUOC <<<\n"
              << "  [" << SORT_BY_PRICE_ASC << "] Gia cuoc tang dan\n"
              << "  [" << SORT_BY_PRICE_DESC << "] Gia cuoc giam dan\n"
              << "  [" << SORT_BY_DATA_DESC << "] Dung luong Data giam dan\n"
              << "  [" << SORT_BY_ID_ASC << "] Ma goi tang dan\n"
              << "  [" << SORT_BY_ID_DESC << "] Ma goi giam dan\n"
              << "  [0] Quay lai\n";

    int choice = InputHelper::getInt("Nhap lua chon [0-5]: ", 0, 5);
    if (choice == 0) return;
    try {
        if (choice == SORT_BY_PRICE_ASC) {
            store.sortAndSave([](const GoiCuoc& a, const GoiCuoc& b) {
                return a.getGiaCuoc() < b.getGiaCuoc();
            });
        } else if (choice == SORT_BY_PRICE_DESC) {
            store.sortAndSave([](const GoiCuoc& a, const GoiCuoc& b) {
                return a.getGiaCuoc() > b.getGiaCuoc();
            });
        } else if (choice == SORT_BY_DATA_DESC) {
            store.sortAndSave([](const GoiCuoc& a, const GoiCuoc& b) {
                return a.getDungLuongData() > b.getDungLuongData();
            });
        } else {
            store.sortById(choice == SORT_BY_ID_ASC);
        }
        std::cout << "[THANH CONG] Da sap xep danh sach goi cuoc!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thu tu sap xep: " << e.what() << "\n";
    }
    xemDanhSach();
}

//cap nhat thong tin goi cuoc
void GoiCuocMenu::capNhatGoiCuoc() {
    std::cout << "\n>>> CAP NHAT THONG TIN GOI CUOC <<<\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc can sua: ");
    if (std::cin.eof()) return;
    GoiCuoc* gc = store.findById(maGoi);
    if (gc == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay goi cuoc co ma: " << maGoi << "\n";
        return;
    }

    gc->displayDetail();
    GoiCuoc updated = *gc;
    std::string tenMoi = InputHelper::getName("Nhap ten goi moi (bo trong de giu nguyen): ", true);
    if (std::cin.eof()) return;
    if (!tenMoi.empty()) updated.setTenGoi(tenMoi);

    bool doiGia = InputHelper::getConfirm("Ban co muon thay doi gia cuoc khong?");
    if (std::cin.eof()) return;
    if (doiGia) {
        double giaMoi = InputHelper::getDouble("Nhap gia cuoc moi (VND): ", 0.0);
        if (std::cin.eof()) return;
        updated.setGiaCuoc(giaMoi);
    }

    try {
        //cap nhat ban sao,dam bao rollback neu loi
        store.update(maGoi, updated);
        std::cout << "[THANH CONG] Da cap nhat goi cuoc!\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thay doi: " << e.what() << "\n";
    }
}

//xoa goi cuoc khoi he thong
void GoiCuocMenu::xoaGoiCuoc() {
    std::cout << "\n>>> XOA GOI CUOC KHOI HE THONG <<<\n";
    std::string maGoi = InputHelper::getString("Nhap ma goi cuoc can xoa: ");
    if (std::cin.eof()) return;
    GoiCuoc* gc = store.findById(maGoi);
    if (gc == nullptr) {
        std::cout << "[CANH BAO] Khong tim thay goi cuoc co ma: " << maGoi << "\n";
        return;
    }

    gc->displayDetail();

    if (thueBaoStore != nullptr) {
        auto usingTbs = thueBaoStore->filter([&maGoi](const ThueBao& tb) {
            return tb.getMaGoiCuoc() == maGoi;
        });
        if (!usingTbs.empty()) {
            std::cout << "[LOI] Khong the xoa goi cuoc! Hien dang co " << usingTbs.size()
                      << " thue bao dang su dung goi cuoc nay.\n";
            return;
        }
    }

    if (hopDongStore != nullptr) {
        auto usingHds = hopDongStore->filter([&maGoi](const HopDong& hd) {
            return hd.getMaGoiCuoc() == maGoi;
        });
        if (!usingHds.empty()) {
            std::cout << "[LOI] Khong the xoa goi cuoc! Hien dang co " << usingHds.size()
                      << " hop dong dang su dung goi cuoc nay.\n";
            return;
        }
    }

    bool confirm = InputHelper::getConfirm("Ban co chac chan muon xoa goi cuoc nay khoi he thong?");
    if (std::cin.eof()) return;
    if (confirm) {
        try {
            store.remove(maGoi);
            std::cout << "[THANH CONG] Da xoa goi cuoc " << maGoi << " thanh cong!\n";
        } catch (const std::exception& e) {
            std::cout << "[LOI] Khong the xoa goi cuoc: " << e.what() << "\n";
        }
    } else {
        std::cout << "[DA HUY] Thao tac xoa da duoc huy bo.\n";
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

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-6]: ", MENU_BACK, MENU_DELETE);
        if (choice == MENU_BACK || std::cin.eof()) {
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
