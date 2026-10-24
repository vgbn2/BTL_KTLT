/**
 * @file KhachHangMenu.h
 * Thanh vien phu trach: Trương Khánh (B24DCVT201)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef KHACHHANG_MENU_H
#define KHACHHANG_MENU_H

#include "Repository.h"
#include "khachhang.h"
#include "thuebao.h"
#include "hopdong.h"

namespace KhachHangMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_UPDATE = 4;
    const int MENU_DELETE = 5;
    const int MENU_VIEW_SIMS = 6;
    const int MENU_SORT = 7;

    const int SEARCH_BY_ID = 1;
    const int SORT_BY_ID_ASC = 1;
    const int SORT_BY_ID_DESC = 2;
    const int SEARCH_BY_CCCD = 2;
    const int SEARCH_BY_NAME = 3;
}

class KhachHangMenu {
private:
    Repository<KhachHang>& store;
    Repository<ThueBao>& thueBaoStore;
    Repository<HopDong>& hopDongStore;

    //them khach hang moi
    void themKhachHang();
    //hien thi danh sach khach hang
    void xemDanhSach();
    //tim kiem khach hang
    void timKiemKhachHang();
    //sap xep khach hang theo ma
    void sapXepDanhSach();
    //cap nhat khach hang
    void capNhatKhachHang();
    //xoa khach hang
    void xoaKhachHang();
    //hien thi danh sach SIM cua khach hang
    void xemDanhSachSIM();

public:
    //tao menu quan ly khach hang
    KhachHangMenu(Repository<KhachHang>& khRepo,
                  Repository<ThueBao>& tbRepo,
                  Repository<HopDong>& hdRepo)
        : store(khRepo), thueBaoStore(tbRepo), hopDongStore(hdRepo) {}

    //hien thi menu quan ly khach hang
    void showMenu();
};

#endif // KHACHHANG_MENU_H
