/**
 * @file ThueBaoMenu.h
 * Thanh vien phu trach: Tùng Dương (B24DCVT106), Trương Khánh (B24DCVT201)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef THUEBAOMENU_H
#define THUEBAOMENU_H

#include "Repository.h"
#include "thuebao.h"
#include "goicuoc.h"
#include "khachhang.h"
#include "imei.h"
#include "hopdong.h"
#include "hoadon.h"

namespace ThueBaoMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_REGISTER_PACKAGE = 4;
    const int MENU_LOCK_UNLOCK = 5;
    const int MENU_TOPUP = 6;
    const int MENU_DELETE = 7;
    const int MENU_SORT = 8;

    const int SORT_BY_ID_ASC = 1;
    const int SORT_BY_ID_DESC = 2;
}

class ThueBaoMenu {
private:
    Repository<ThueBao>& store;
    Repository<GoiCuoc>& goiCuocStore;
    Repository<KhachHang>& khachHangStore;
    Repository<ThietBiIMEI>& imeiStore;
    Repository<HopDong>* hopDongStore;
    Repository<HoaDon>* hoaDonStore;

    //them thue bao moi
    void themThueBao();
    //hien thi danh sach thue bao
    void xemDanhSach();
    //tim kiem thue bao
    void timKiemThueBao();
    //sap xep thue bao theo so dien thoai
    void sapXepDanhSach();
    //dang ky goi cuoc
    void dangKyGoiCuoc();
    //khoa hoac mo chieu thue bao
    void khoaMoChieu();
    //nap tien vao tai khoan
    void napTien();
    //xoa thue bao
    void xoaThueBao();

public:
    //tao menu quan ly thue bao
    ThueBaoMenu(Repository<ThueBao>& s,
                Repository<GoiCuoc>& gs,
                Repository<KhachHang>& ks,
                Repository<ThietBiIMEI>& is,
                Repository<HopDong>* hs = nullptr,
                Repository<HoaDon>* hds = nullptr)
        : store(s), goiCuocStore(gs), khachHangStore(ks), imeiStore(is),
          hopDongStore(hs), hoaDonStore(hds) {}
    //hien thi menu quan ly thue bao
    void showMenu();
};

#endif // THUEBAOMENU_H
