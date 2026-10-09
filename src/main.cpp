#include <iostream>
#include "lib/shared/fileio.h"
#include "lib/shared/DataStore.h"
#include "lib/models/hopdong.h"
#include "lib/models/imei.h"
#include "lib/menus/HopDongMenu.h"
#include "lib/menus/ThietBiIMEIMenu.h"
#include "lib/shared/InputHelper.h"

namespace MainMenuChoices {
    const int MENU_EXIT = 0;
    const int MENU_GOICUOC = 1;
    const int MENU_THUEBAO = 2;
    const int MENU_HOPDONG = 3;
    const int MENU_IMEI = 4;
    const int MENU_NAPTIEN = 5;
    const int MENU_HOADON = 6;
}

int main() {
    DataStore<HopDong> hopDongStore(FilePaths::HOPDONG_DATA);
    DataStore<ThietBiIMEI> imeiStore(FilePaths::IMEI_DATA);

    try {
        hopDongStore.loadFromFile();
        imeiStore.loadFromFile();
    } catch (const std::exception& e) {
        std::cerr << "[CANH BAO] Loi khi tai du lieu ban dau: " << e.what() << "\n";
    }

    HopDongMenu hopDongMenu(hopDongStore);
    ThietBiIMEIMenu imeiMenu(imeiStore);

    while (true) {
        std::cout << "\n====================================================================\n"
                  << "               HE THONG QUAN LY THUE BAO DI DONG\n"
                  << "====================================================================\n"
                  << "  [" << MainMenuChoices::MENU_GOICUOC << "] Quan ly goi cuoc\n"
                  << "  [" << MainMenuChoices::MENU_THUEBAO << "] Quan ly thue bao\n"
                  << "  [" << MainMenuChoices::MENU_HOPDONG << "] Quan ly hop dong\n"
                  << "  [" << MainMenuChoices::MENU_IMEI << "] Quan ly thiet bi IMEI\n"
                  << "  [" << MainMenuChoices::MENU_NAPTIEN << "] Quan ly phieu nap tien\n"
                  << "  [" << MainMenuChoices::MENU_HOADON << "] Quan ly hoa don\n"
                  << "  [" << MainMenuChoices::MENU_EXIT << "] Thoat chuong trinh\n"
                  << "====================================================================\n";

        int choice = InputHelper::getInt("Nhap lua chon cua ban [0-6]: ",
                                         MainMenuChoices::MENU_EXIT,
                                         MainMenuChoices::MENU_HOADON);

        if (choice == MainMenuChoices::MENU_EXIT) {
            // ponytail: simple sync on exit, fileio handles atomicity via tmp file
            try {
                hopDongStore.saveToFile();
                imeiStore.saveToFile();
                std::cout << "\n[Luu du lieu] Da dong bo du lieu.\n";
            } catch (const std::exception& e) {
                std::cerr << "\n[CANH BAO] Khong the luu du lieu khi thoat: " << e.what() << "\n";
            }
            std::cout << "Cam on ban da su dung chuong trinh! Tam biet.\n";
            break;
        }

        switch (choice) {
            case MainMenuChoices::MENU_GOICUOC:
                std::cout << "\n[THONG BAO] Chuc nang Quan ly goi cuoc dang duoc phat trien (Phu trach: Nguyen Tung Duong - B24DCVT106).\n";
                InputHelper::pause();
                break;
            case MainMenuChoices::MENU_THUEBAO:
                std::cout << "\n[THONG BAO] Chuc nang Quan ly thue bao dang duoc phat trien (Phu trach: Nguyen Tat Thang - B24DCVT331).\n";
                InputHelper::pause();
                break;
            case MainMenuChoices::MENU_HOPDONG:
                hopDongMenu.showMenu();
                break;
            case MainMenuChoices::MENU_IMEI:
                imeiMenu.showMenu();
                break;
            case MainMenuChoices::MENU_NAPTIEN:
                std::cout << "\n[THONG BAO] Chuc nang Quan ly phieu nap tien dang duoc phat trien (Phu trach: Nguyen Manh Dung - B24DCVT094).\n";
                InputHelper::pause();
                break;
            case MainMenuChoices::MENU_HOADON:
                std::cout << "\n[THONG BAO] Chuc nang Quan ly hoa don dang duoc phat trien (Phu trach: Dang Viet Hung - B24DCVT167).\n";
                InputHelper::pause();
                break;
            default:
                break;
        }
    }

    return 0;
}
