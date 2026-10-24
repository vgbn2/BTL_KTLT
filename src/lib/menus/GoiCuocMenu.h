/**
 * @file GoiCuocMenu.h
 * Thanh vien phu trach: Tùng Dương (B24DCVT106)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef GOICUOCMENU_H
#define GOICUOCMENU_H

#include "Repository.h"
#include "goicuoc.h"

namespace GoiCuocMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_SORT = 4;
    const int MENU_UPDATE = 5;
    const int MENU_DELETE = 6;

    const int SORT_BY_PRICE_ASC = 1;
    const int SORT_BY_PRICE_DESC = 2;
    const int SORT_BY_DATA_DESC = 3;
    const int SORT_BY_ID_ASC = 4;
    const int SORT_BY_ID_DESC = 5;
}

class GoiCuocMenu {
private:
    Repository<GoiCuoc>& store;

    //them goi cuoc moi
    void themGoiCuoc();
    //hien thi danh sach goi cuoc
    void xemDanhSach();
    //tim kiem goi cuoc
    void timKiemGoiCuoc();
    //sap xep danh sach goi cuoc
    void sapXepDanhSach();
    //cap nhat goi cuoc
    void capNhatGoiCuoc();
    //xoa goi cuoc
    void xoaGoiCuoc();

public:
    //tao menu quan ly goi cuoc
    explicit GoiCuocMenu(Repository<GoiCuoc>& s) : store(s) {}
    //hien thi menu quan ly goi cuoc
    void showMenu();
};

#endif // GOICUOCMENU_H
