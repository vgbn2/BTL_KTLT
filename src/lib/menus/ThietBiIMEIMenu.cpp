#include "ThietBiIMEIMenu.h"
#include "InputHelper.h"
#include <iostream>
#include <random>

namespace {
//tao IMEI mo phong cho du lieu demo;menu giu quyen so huu vi ham nay can kiem tra kho hien tai.
//sinh 14 chu so,phep Luhn tao chu so thu 15, sau do loai trung voi IMEI da luu.
std::string generateSimulatedImei(const Repository<ThietBiIMEI>& store) {
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> digit(0, 9);
    while (true) {
        std::string prefix;
        prefix.reserve(14);
        prefix.push_back(static_cast<char>('3' + digit(generator) % 7));
        for (int i = 1; i < 14; ++i) {
            prefix.push_back(static_cast<char>('0' + digit(generator)));
        }

        int sum = 0;
        for (int i = 0; i < 14; ++i) {
            int value = prefix[i] - '0';
            if ((15 - i) % 2 == 0) {
                value *= 2;
                if (value > 9) value -= 9;
            }
            sum += value;
        }
        const std::string imei = prefix + std::to_string((10 - sum % 10) % 10);
        if (store.findById(imei) == nullptr && ThietBiIMEI::validateLuhn(imei)) {
            return imei;
        }
    }
}
}

using namespace IMEIMenuChoices;

//hien thi menu quan ly thiet bi IMEI
void ThietBiIMEIMenu::showMenu() {
    while (true) {
        std::cout << "\n--------------------------------------------------------------------\n"
                  << "                     QUAN LY THIET BI DAU CUOI (IMEI)\n"
                  << "--------------------------------------------------------------------\n"
                  << "  [" << MENU_ADD << "] Them thiet bi moi\n"
                  << "  [" << MENU_LIST << "] Xem danh sach thiet bi\n"
                  << "  [" << MENU_SEARCH << "] Tim kiem thiet bi\n"
                  << "  [" << MENU_SORT << "] Sap xep thiet bi theo Ma IMEI\n"
                  << "  [" << MENU_BLACKLIST << "] Danh sach thiet bi bi khoa mang\n"
                  << "  [" << MENU_UPDATE << "] Cap nhat thiet bi\n"
                  << "  [" << MENU_DELETE << "] Xoa thiet bi\n"
                  << "  [" << MENU_BACK << "] Quay lai menu chinh\n"
                  << "--------------------------------------------------------------------\n";

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-7]: ", MENU_BACK, MENU_DELETE);
        if (choice == MENU_BACK) {
            break;
        }

        switch (choice) {
            case MENU_ADD:
                themThietBi();
                break;
            case MENU_LIST:
                xemDanhSach();
                break;
            case MENU_SEARCH:
                timKiemThietBi();
                break;
            case MENU_SORT:
                sapXepDanhSach();
                break;
            case MENU_BLACKLIST:
                danhSachKhoaMang();
                break;
            case MENU_UPDATE:
                capNhatThietBi();
                break;
            case MENU_DELETE:
                xoaThietBi();
                break;
            default:
                break;
        }
    }
}

//them thiet bi IMEI moi
void ThietBiIMEIMenu::themThietBi() {
    std::cout << "\n>>> THEM THIET BI DAU CUOI MOI (IMEI) <<<\n"
              << "  [1] Nhap IMEI that cua thiet bi\n"
              << "  [2] Sinh IMEI mo phong cho du lieu demo\n";
    int imeiChoice = InputHelper::getInt("Chon cach tao IMEI [1-2]: ", 1, 2);
    if (std::cin.eof()) return;
    std::string imei;
    if (imeiChoice == 2) {
        imei = generateSimulatedImei(store);
        std::cout << "[MO PHONG] Da sinh IMEI duy nhat: " << imei << "\n";
    } else {
        while (true) {
            imei = InputHelper::getString("Nhap Ma IMEI (15 chu so): ", false);
            if (std::cin.eof()) return;
            if (imei.length() != IMEIConstants::REQUIRED_IMEI_LENGTH) {
                std::cout << "  [!] Do dai IMEI phai dung 15 chu so! Vui long nhap lai.\n";
                continue;
            }
            if (!ThietBiIMEI::validateLuhn(imei)) {
                std::cout << "  [!] Ma IMEI khong hop le. Vui long kiem tra lai.\n";
                continue;
            }
            if (store.findById(imei) != nullptr) {
                std::cout << "  [!] Ma IMEI '" << imei << "' da ton tai trong he thong! Vui long nhap ma khac.\n";
                continue;
            }
            break;
        }
    }
    std::string tenTB = InputHelper::getString("Nhap Ten thiet bi (VD: iPhone 15 Pro, Galaxy S24): ", false);
    if (std::cin.eof() || tenTB.empty()) return;
    std::string hangSX = InputHelper::getString("Nhap Hang san xuat (VD: Apple, Samsung, Xiaomi): ", false);
    if (std::cin.eof() || hangSX.empty()) return;
    std::string sdt = InputHelper::getPhoneNumber("Nhap So dien thoai gan kem: ", true);
    if (std::cin.eof()) return;
    //kiem tra thue bao gan kem ton tai
    if (!sdt.empty() && sdt != IMEIConstants::UNASSIGNED_PHONE && thueBaoStore.findById(sdt) == nullptr) {
        std::cout << "  [!] So dien thoai '" << sdt << "' chua duoc dang ky tren he thong!\n";
        InputHelper::pause();
        return;
    }

    Date ngayKH = InputHelper::getDate("Nhap Ngay kich hoat (DD/MM/YYYY): ");
    if (std::cin.eof()) return;
    std::string bts = InputHelper::getString("Nhap Tram BTS gan nhat (VD: BTS-HN-001): ", false);
    if (std::cin.eof() || bts.empty()) return;

    try {
        ThietBiIMEI tb(imei, tenTB, hangSX, sdt, ngayKH, IMEIConstants::STATUS_ACTIVE, bts);
        store.add(tb);
        std::cout << "[THANH CONG] Da them thiet bi IMEI " << imei << " vao he thong!\n";
    } catch (const AppException& e) {
        std::cout << "[LOI] " << e.what() << "\n";
    }
    InputHelper::pause();
}

//hien thi danh sach thiet bi
void ThietBiIMEIMenu::xemDanhSach() {
    std::cout << "\n>>> DANH SACH THIET BI DAU CUOI (IMEI) <<<\n";
    const auto& list = store.getAll();
    if (list.empty()) {
        std::cout << "[THONG BAO] Danh sach thiet bi hien dang trong!\n";
        InputHelper::pause();
        return;
    }

    list[0].displayHeader();
    for (const auto& tb : list) {
        tb.displayRow();
    }
    std::cout << "Tong so: " << list.size() << " thiet bi.\n";
    InputHelper::pause();
}

//tim kiem thiet bi theo dieu kien
void ThietBiIMEIMenu::timKiemThietBi() {
    std::cout << "\n>>> TIM KIEM THIET BI <<<\n"
              << "  [" << SEARCH_BY_IMEI << "] Tra cuu theo Ma IMEI\n"
              << "  [" << SEARCH_BY_PHONE << "] Tra cuu theo So dien thoai dang gan\n"
              << "  [" << SEARCH_BY_BRAND << "] Tra cuu theo Hang san xuat\n"
              << "  [0] Quay lai\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-3]: ", 0, SEARCH_BY_BRAND);
    if (choice == 0) return;
    if (choice == SEARCH_BY_IMEI) {
        std::string imei = InputHelper::getString("Nhap Ma IMEI can tim: ", false);
        ThietBiIMEI* tb = store.findById(imei);
        if (tb == nullptr) {
            std::cout << "[THONG BAO] Khong tim thay thiet bi co IMEI: " << imei << "\n";
        } else {
            tb->displayHeader();
            tb->displayRow();
            tb->displayDetail();
        }
    } else if (choice == SEARCH_BY_PHONE) {
        std::string sdt = InputHelper::getString("Nhap So dien thoai can tim: ", false);
        auto results = store.filter([&sdt](const ThietBiIMEI& tb) {
            return tb.getSoDienThoai() == sdt;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong co thiet bi nao dang gan so: " << sdt << "\n";
        } else {
            results[0].displayHeader();
            for (const auto& tb : results) {
                tb.displayRow();
            }
            std::cout << "Tim thay " << results.size() << " thiet bi.\n";
        }
    } else if (choice == SEARCH_BY_BRAND) {
        std::string brand = InputHelper::getString("Nhap Hang san xuat: ", false);
        auto results = store.filter([&brand](const ThietBiIMEI& tb) {
            return tb.getHangSanXuat() == brand;
        });
        if (results.empty()) {
            std::cout << "[THONG BAO] Khong co thiet bi nao cua hang: " << brand << "\n";
        } else {
            results[0].displayHeader();
            for (const auto& tb : results) {
                tb.displayRow();
            }
            std::cout << "Tim thay " << results.size() << " thiet bi.\n";
        }
    }
    InputHelper::pause();
}

//sap xep danh sach thiet bi theo ma IMEI
void ThietBiIMEIMenu::sapXepDanhSach() {
    std::cout << "\n>>> SAP XEP DANH SACH THIET BI <<<\n"
              << "  [" << SORT_BY_ID_ASC << "] Ma IMEI tang dan\n"
              << "  [" << SORT_BY_ID_DESC << "] Ma IMEI giam dan\n";
    int choice = InputHelper::getInt("Nhap kieu sap xep [1-2]: ", SORT_BY_ID_ASC, SORT_BY_ID_DESC);
    try {
        store.sortById(choice == SORT_BY_ID_ASC);
        std::cout << "[THANH CONG] Da sap xep va luu danh sach thiet bi.\n";
    } catch (const std::exception& e) {
        std::cout << "[LOI] Khong the luu thu tu sap xep: " << e.what() << "\n";
    }
    xemDanhSach();
}

//hien thi danh sach thiet bi bi khoa mang
void ThietBiIMEIMenu::danhSachKhoaMang() {
    std::cout << "\n>>> DANH SACH THIET BI BI KHOA MANG <<<\n";
    auto blacklisted = store.filter([](const ThietBiIMEI& tb) {
        return tb.isBlacklisted();
    });

    if (blacklisted.empty()) {
        std::cout << "[THONG BAO] Hien khong co thiet bi nao bi khoa mang\n";
    } else {
        blacklisted[0].displayHeader();
        for (const auto& tb : blacklisted) {
            tb.displayRow();
        }
        std::cout << "Tong so thiet bi bi khoa mang: " << blacklisted.size() << ".\n";
    }
    InputHelper::pause();
}

//cap nhat thong tin thiet bi
void ThietBiIMEIMenu::capNhatThietBi() {
    std::cout << "\n>>> CAP NHAT THONG TIN THIET BI (IMEI) <<<\n";
    std::string imei = InputHelper::getString("Nhap Ma IMEI can cap nhat: ", false);
    if (std::cin.eof()) return;
    ThietBiIMEI* tb = store.findById(imei);
    if (tb == nullptr) {
        std::cout << "[LOI] Khong tim thay thiet bi co IMEI: " << imei << "\n";
        InputHelper::pause();
        return;
    }

    std::cout << "\nThong tin hien tai cua thiet bi:\n";
    tb->displayDetail();
    ThietBiIMEI updated = *tb;

    std::cout << "\nChon thao tac cap nhat:\n"
              << "  [" << UPDATE_ASSIGN_SIM << "] Gan lai So dien thoai\n"
              << "  [" << UPDATE_BTS << "] Cap nhat vi tri tram BTS\n"
              << "  [" << UPDATE_LOCK << "] Khoa mang thiet bi\n"
              << "  [" << UPDATE_UNLOCK << "] Mo khoa mang thiet bi\n"
              << "  [" << UPDATE_CANCEL << "] Huy bo thao tac\n";
    int choice = InputHelper::getInt("Nhap lua chon [0-4]: ", UPDATE_CANCEL, UPDATE_UNLOCK);

    if (choice == UPDATE_CANCEL || std::cin.eof()) {
        std::cout << "[DA HUY] Thao tac cap nhat da duoc huy bo.\n";
        InputHelper::pause();
        return;
    }

    try {
        if (choice == UPDATE_ASSIGN_SIM) {
            std::string sdtMoi = InputHelper::getPhoneNumber("Nhap So dien thoai moi (hoac de trong de go SIM): ", true);
            if (std::cin.eof()) return;
            if (!sdtMoi.empty() && sdtMoi != IMEIConstants::UNASSIGNED_PHONE && thueBaoStore.findById(sdtMoi) == nullptr) {
                std::cout << "  [!] So dien thoai '" << sdtMoi << "' chua duoc dang ky tren he thong!\n";
                InputHelper::pause();
                return;
            }
            updated.ganSIM(sdtMoi);
            store.update(imei, updated);
            std::cout << "[THANH CONG] Da cap nhat so SIM gan voi thiet bi " << imei << "!\n";
        }

        else if (choice == UPDATE_BTS) {
            std::string btsMoi = InputHelper::getString("Nhap Ma tram BTS moi: ", false);
            if (std::cin.eof() || btsMoi.empty()) return;
            updated.capNhatBTS(btsMoi);
            store.update(imei, updated);
            std::cout << "[THANH CONG] Da cap nhat vi tri tram BTS thanh " << btsMoi << "!\n";
        }

        else if (choice == UPDATE_LOCK) {
            updated.setBlacklist(true);
            store.update(imei, updated);
            std::cout << "[THANH CONG] Da KHOA MANG thiet bi " << imei << " (EIR Blacklist)!\n";
        }

        else if (choice == UPDATE_UNLOCK) {
            updated.setBlacklist(false);
            store.update(imei, updated);
            std::cout << "[THANH CONG] Da MO KHOA MANG thiet bi " << imei << " (Trang thai: HoatDong)!\n";
        }
    } catch (const AppException& e) {
        std::cout << "[LOI] " << e.what() << "\n";
    }
    InputHelper::pause();
}

//xoa thiet bi khoi he thong
void ThietBiIMEIMenu::xoaThietBi() {
    std::cout << "\n>>> XOA THIET BI KHOI HE THONG <<<\n";
    std::string imei = InputHelper::getString("Nhap Ma IMEI can xoa: ", false);
    if (std::cin.eof()) return;
    ThietBiIMEI* tb = store.findById(imei);
    if (tb == nullptr) {
        std::cout << "[LOI] Khong tim thay thiet bi co IMEI: " << imei << "\n";
        InputHelper::pause();
        return;
    }

    std::cout << "\nThong tin thiet bi can xoa:\n";
    tb->displayDetail();

    bool confirm = InputHelper::getConfirm("Ban co chac chan muon xoa thiet bi nay khoi he thong?");
    if (std::cin.eof()) return;
    if (confirm) {
        try {
            store.remove(imei);
            std::cout << "[THANH CONG] Da xoa thiet bi " << imei << " khoi he thong va cap nhat file!\n";
        } catch (const AppException& e) {
            std::cout << "[LOI] " << e.what() << "\n";
        }
    } else {
        std::cout << "[DA HUY] Thao tac xoa da duoc huy bo.\n";
    }
    InputHelper::pause();
}
