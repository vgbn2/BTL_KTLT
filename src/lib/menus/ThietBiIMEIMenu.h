#ifndef THIETBIIMEIMENU_H
#define THIETBIIMEIMENU_H

#include "DataStore.h"
#include "imei.h"

namespace IMEIMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_BLACKLIST = 4;
    const int MENU_UPDATE = 5;
    const int MENU_DELETE = 6;

    const int SEARCH_BY_IMEI = 1;
    const int SEARCH_BY_PHONE = 2;
    const int SEARCH_BY_BRAND = 3;

    const int UPDATE_ASSIGN_SIM = 1;
    const int UPDATE_BTS = 2;
    const int UPDATE_LOCK = 3;
    const int UPDATE_UNLOCK = 4;
    const int UPDATE_CANCEL = 0;
}

class ThietBiIMEIMenu {
private:
    DataStore<ThietBiIMEI>& store;

    void themThietBi();
    void xemDanhSach();
    void timKiemThietBi();
    void danhSachKhoaMang();
    void capNhatThietBi();
    void xoaThietBi();

public:
    explicit ThietBiIMEIMenu(DataStore<ThietBiIMEI>& s) : store(s) {}
    void showMenu();
};

#endif // THIETBIIMEIMENU_H
