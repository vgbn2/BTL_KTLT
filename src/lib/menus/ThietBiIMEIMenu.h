/**
 * @file ThietBiIMEIMenu.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef THIETBIIMEIMENU_H
#define THIETBIIMEIMENU_H

#include "Repository.h"
#include "imei.h"
#include "thuebao.h"

namespace IMEIMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_SORT = 4;
    const int MENU_BLACKLIST = 5;
    const int MENU_UPDATE = 6;
    const int MENU_DELETE = 7;

    const int SORT_BY_ID_ASC = 1;
    const int SORT_BY_ID_DESC = 2;

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
    Repository<ThietBiIMEI>& store;
    Repository<ThueBao>& thueBaoStore;

    //them thiet bi IMEI
    void themThietBi();
    //hien thi danh sach thiet bi
    void xemDanhSach();
    //tim kiem thiet bi
    void timKiemThietBi();
    //sap xep thiet bi theo ma IMEI
    void sapXepDanhSach();
    //hien thi thiet bi bi khoa mang
    void danhSachKhoaMang();
    //cap nhat thiet bi
    void capNhatThietBi();
    //xoa thiet bi
    void xoaThietBi();

public:
    //tao menu quan ly thiet bi IMEI
    ThietBiIMEIMenu(Repository<ThietBiIMEI>& s, Repository<ThueBao>& ts)
        : store(s), thueBaoStore(ts) {}
    //hien thi menu quan ly thiet bi IMEI
    void showMenu();
};

#endif // THIETBIIMEIMENU_H
