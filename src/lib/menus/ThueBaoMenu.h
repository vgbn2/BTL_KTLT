#ifndef THUEBAOMENU_H
#define THUEBAOMENU_H

#include "Repository.h"
#include "thuebao.h"
#include "goicuoc.h"
#include "khachhang.h"

namespace ThueBaoMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_REGISTER_PACKAGE = 4;
    const int MENU_LOCK_UNLOCK = 5;
    const int MENU_TOPUP = 6;
    const int MENU_DELETE = 7;
}

class ThueBaoMenu {
private:
    Repository<ThueBao>& store;
    Repository<GoiCuoc>& goiCuocStore;
    Repository<KhachHang>& khachHangStore;

    //them thue bao moi
    void themThueBao();
    //hien thi danh sach thue bao
    void xemDanhSach();
    //tim kiem thue bao
    void timKiemThueBao();
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
    ThueBaoMenu(Repository<ThueBao>& s, Repository<GoiCuoc>& gs, Repository<KhachHang>& ks)
        : store(s), goiCuocStore(gs), khachHangStore(ks) {}
    //hien thi menu quan ly thue bao
    void showMenu();
};

#endif // THUEBAOMENU_H
