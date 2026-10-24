#ifndef THUEBAOMENU_H
#define THUEBAOMENU_H

#include "Repository.h"
#include "thuebao.h"
#include "goicuoc.h"
#include "khachhang.h"
#include "imei.h"

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
                Repository<ThietBiIMEI>& is)
        : store(s), goiCuocStore(gs), khachHangStore(ks), imeiStore(is) {}
    //hien thi menu quan ly thue bao
    void showMenu();
};

#endif // THUEBAOMENU_H
