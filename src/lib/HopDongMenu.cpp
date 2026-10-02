#include "HopDongMenu.h"
#include "InputHelper.h"
#include <iostream>

using namespace HopDongMenuChoices;

void HopDongMenu::showMenu() {
    while (true) {
        std::cout << "\n--------------------------------------------------------------------\n"
                  << "                     QUAN LY HOP DONG DANG KY\n"
                  << "--------------------------------------------------------------------\n"
                  << "  [" << MENU_ADD << "] Them hop dong moi\n"
                  << "  [" << MENU_LIST << "] Xem danh sach hop dong\n"
                  << "  [" << MENU_SEARCH << "] Tim kiem hop dong\n"
                  << "  [" << MENU_SORT << "] Sap xep danh sach\n"
                  << "  [" << MENU_UPDATE << "] Cap nhat thong tin hop dong\n"
                  << "  [" << MENU_DELETE << "] Xoa hop dong\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "--------------------------------------------------------------------\n";

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-6]: ", MENU_BACK, MENU_DELETE);
        if (choice == MENU_BACK) {
            break;
        }

        switch (choice) {
            case MENU_ADD:
                themHopDong();
                break;
            case MENU_LIST:
                xemDanhSach();
                break;
            case MENU_SEARCH:
                timKiemHopDong();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            case MENU_UPDATE:
                capNhatHopDong();
                break;
            case MENU_DELETE:
                xoaHopDong();
                break;
            default:
                break;
        }
    }
}

void HopDongMenu::themHopDong() {
    std::cout << "\n>>> THEM HOP DONG DANG KY MOI <<<\n";
    std::string maHD;
    while (true) {
        maHD = InputHelper::getString("Nhap Ma hop dong (VD: HD0010): ", false);
        if (repo.findById(maHD) != nullptr) {
            std::cout << "  [!] Ma hop dong '" << maHD << "' da ton tai! Vui long nhap ma khac.\n";
            continue;
        }
        break;
    }

    std::string maKH = InputHelper::getString("Nhap Ma khach hang (VD: KH0001): ", false);
    std::string sdt = InputHelper::getString("Nhap So dien thoai (10 so): ", false);
    std::string maGC = InputHelper::getString("Nhap Ma goi cuoc (VD: GC001, VD149): ", false);

    Date ngayDK = InputHelper::getDate("Nhap Ngay dang ky (DD/MM/YYYY): ");
    Date ngayHH;
    while (true) {
        ngayHH = InputHelper::getDate("Nhap Ngay het han (DD/MM/YYYY): ");
        if (ngayHH < ngayDK) {
            std::cout << "  [!] Ngay het han khong duoc nho hon ngay dang ky (" << ngayDK.toString() << ")!\n";
            continue;
        }
        break;
    }

    std::cout << "Chon Loai hop dong:\n"
              << "  [" << TYPE_CHOICE_PREPAID << "] Tra truoc (Prepaid)\n"
              << "  [" << TYPE_CHOICE_POSTPAID << "] Tra sau (Postpaid)\n";
    int loaiChoice = InputHelper::getInt("Lua chon [1-2]: ", TYPE_CHOICE_PREPAID, TYPE_CHOICE_POSTPAID);
    std::string loaiHD = (loaiChoice == TYPE_CHOICE_PREPAID) ? HopDongConstants::TYPE_PREPAID : HopDongConstants::TYPE_POSTPAID;

    double gia = InputHelper::getDouble("Nhap Gia tri goi cuoc (VND): ", 0.0);

    HopDong hd(maHD, maKH, sdt, maGC, ngayDK, ngayHH, loaiHD, HopDongConstants::STATUS_ACTIVE, gia);
    try {
        repo.add(hd);
        std::cout << "[THANH CONG] Da them hop dong " << maHD << " vao he thong!\n";
    } catch (const AppException& e) {
        std::cout << "[LOI] " << e.what() << "\n";
    }
    InputHelper::pause();
}

void HopDongMenu::xemDanhSach() {
    std::cout << "\n>>> DANH SACH HOP DONG DANG KY <<<\n";
    const auto& list = repo.getAll();
    if (list.empty()) {
        std::cout << "Danh sach hop dong hien dang trong!\n";
        InputHelper::pause();
        return;
    }

    list[0].displayHeader();
    for (const auto& hd : list) {
        hd.displayRow();
    }
    std::cout << "Tong so: " << list.size() << " hop dong.\n";
    InputHelper::pause();
}

void HopDongMenu::timKiemHopDong() {
    std::cout << "\n>>> TIM KIEM HOP DONG <<<\n"
              << "  [" << SEARCH_BY_ID << "] Tim theo Ma hop dong (chinh xac)\n"
              << "  [" << SEARCH_BY_PHONE << "] Tim theo So dien thoai\n"
              << "  [" << SEARCH_BY_CUSTOMER << "] Tim theo Ma khach hang\n"
              << "  [0] Quay lai\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-3]: ", 0, SEARCH_BY_CUSTOMER);
    if (choice == 0) return;

    if (choice == SEARCH_BY_ID) {
        std::string id = InputHelper::getString("Nhap Ma hop dong can tim: ", false);
        HopDong* hd = repo.findById(id);
        if (hd == nullptr) {
            std::cout << "[THONG BAO] Khong tim thay hop dong nao co ma: " << id << "\n";
        } else {
            hd->displayHeader();
            hd->displayRow();
            hd->displayDetail();
        }
    } else if (choice == SEARCH_BY_PHONE) {
        std::string sdt = InputHelper::getString("Nhap So dien thoai can tim: ", false);
        auto results = repo.filter([&sdt](const HopDong& hd) {
            return hd.getSoDienThoai() == sdt;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong co hop dong nao gan voi so: " << sdt << "\n";
        } else {
            results[0].displayHeader();
            for (const auto& hd : results) {
                hd.displayRow();
            }
            std::cout << "Tim thay " << results.size() << " hop dong.\n";
        }
    } else if (choice == SEARCH_BY_CUSTOMER) {
        std::string kh = InputHelper::getString("Nhap Ma khach hang: ", false);
        auto results = repo.filter([&kh](const HopDong& hd) {
            return hd.getMaKhachHang() == kh;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong co hop dong nao cua khach hang: " << kh << "\n";
        } else {
            results[0].displayHeader();
            for (const auto& hd : results) {
                hd.displayRow();
            }
            std::cout << "Tim thay " << results.size() << " hop dong.\n";
        }
    }
    InputHelper::pause();
}

void HopDongMenu::sapXepDanhSach() {
    std::cout << "\n>>> SAP XEP DANH SACH HOP DONG <<<\n"
              << "  [" << SORT_BY_DATE_DESC << "] Theo Ngay dang ky (Moi nhat -> Cu nhat)\n"
              << "  [" << SORT_BY_PRICE_DESC << "] Theo Gia tri goi cuoc (Giam dan)\n"
              << "  [0] Quay lai\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-2]: ", 0, SORT_BY_PRICE_DESC);
    if (choice == 0) return;

    if (choice == SORT_BY_DATE_DESC) {
        repo.sort([](const HopDong& a, const HopDong& b) {
            return b.getNgayDangKy() < a.getNgayDangKy();
        });
        std::cout << "[THANH CONG] Da sap xep danh sach theo ngay dang ky giam dan!\n";
    } else if (choice == SORT_BY_PRICE_DESC) {
        repo.sort([](const HopDong& a, const HopDong& b) {
            return a.getGiaTriGoi() > b.getGiaTriGoi();
        });
        std::cout << "[THANH CONG] Da sap xep danh sach theo gia tri goi cuoc giam dan!\n";
    }
    xemDanhSach();
}

void HopDongMenu::capNhatHopDong() {
    std::cout << "\n>>> CAP NHAT THONG TIN HOP DONG <<<\n";
    std::string id = InputHelper::getString("Nhap Ma hop dong can cap nhat: ", false);
    HopDong* hd = repo.findById(id);
    if (hd == nullptr) {
        std::cout << "[LOI] Khong tim thay hop dong co ma: " << id << "\n";
        InputHelper::pause();
        return;
    }

    std::cout << "\nThong tin hien tai cua hop dong:\n";
    hd->displayDetail();

    std::cout << "\nChon thong tin can cap nhat:\n"
              << "  [" << UPDATE_PACKAGE << "] Doi goi cuoc\n"
              << "  [" << UPDATE_EXPIRY << "] Gia han ngay het han\n"
              << "  [" << UPDATE_STATUS << "] Chuyen trang thai (HieuLuc / TamDung / ThanhLy)\n"
              << "  [" << UPDATE_CANCEL << "] Huy bo thao tac\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-3]: ", UPDATE_CANCEL, UPDATE_STATUS);

    if (choice == UPDATE_PACKAGE) {
        std::string maGoiMoi = InputHelper::getString("Nhap Ma goi cuoc moi: ", false);
        double giaMoi = InputHelper::getDouble("Nhap Gia tri goi moi (VND): ", 0.0);
        hd->setMaGoiCuoc(maGoiMoi);
        hd->setGiaTriGoi(giaMoi);
        repo.update(id, *hd);
        std::cout << "[THANH CONG] Da cap nhat goi cuoc moi cho hop dong " << id << "!\n";
    } else if (choice == UPDATE_EXPIRY) {
        Date ngayHHMoi = InputHelper::getDate("Nhap Ngay het han moi (DD/MM/YYYY): ");
        try {
            hd->giaHan(ngayHHMoi);
            repo.update(id, *hd);
            std::cout << "[THANH CONG] Da gia han hop dong " << id << " den ngay " << ngayHHMoi.toString() << "!\n";
        } catch (const AppException& e) {
            std::cout << "[LOI] " << e.what() << "\n";
        }
    } else if (choice == UPDATE_STATUS) {
        std::cout << "Chon trang thai moi:\n"
                  << "  [" << STATUS_CHOICE_ACTIVE << "] " << HopDongConstants::STATUS_ACTIVE << "\n"
                  << "  [" << STATUS_CHOICE_SUSPENDED << "] " << HopDongConstants::STATUS_SUSPENDED << "\n"
                  << "  [" << STATUS_CHOICE_TERMINATED << "] " << HopDongConstants::STATUS_TERMINATED << "\n";
        int sChoice = InputHelper::getInt("Nhap lua chon [1-3]: ", STATUS_CHOICE_ACTIVE, STATUS_CHOICE_TERMINATED);
        if (sChoice == STATUS_CHOICE_ACTIVE) hd->kichHoatLai();
        else if (sChoice == STATUS_CHOICE_SUSPENDED) hd->tamDung();
        else if (sChoice == STATUS_CHOICE_TERMINATED) hd->chamDut();

        repo.update(id, *hd);
        std::cout << "[THANH CONG] Da cap nhat trang thai thanh: " << hd->getTrangThai() << "!\n";
    }
    InputHelper::pause();
}

void HopDongMenu::xoaHopDong() {
    std::cout << "\n>>> XOA / THANH LY HOP DONG <<<\n";
    std::string id = InputHelper::getString("Nhap Ma hop dong can xoa: ", false);
    HopDong* hd = repo.findById(id);
    if (hd == nullptr) {
        std::cout << "[LOI] Khong tim thay hop dong co ma: " << id << "\n";
        InputHelper::pause();
        return;
    }

    std::cout << "\nThong tin hop dong can xoa:\n";
    hd->displayDetail();

    bool confirm = InputHelper::getConfirm("Ban co chac chan muon xoa hop dong nay khoi he thong?");
    if (confirm) {
        try {
            repo.remove(id);
            std::cout << "[THANH CONG] Da xoa hop dong " << id << " khoi he thong va cap nhat file!\n";
        } catch (const AppException& e) {
            std::cout << "[LOI] " << e.what() << "\n";
        }
    } else {
        std::cout << "[DA HUY] Thao tac xoa da duoc huy bo.\n";
    }
    InputHelper::pause();
}
