#ifndef HOPDONGMENU_H
#define HOPDONGMENU_H

#include "Repository.h"
#include "hopdong.h"
#include "khachhang.h"
#include "thuebao.h"
#include "goicuoc.h"

namespace HopDongMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_ADD = 1;
    const int MENU_LIST = 2;
    const int MENU_SEARCH = 3;
    const int MENU_SORT = 4;
    const int MENU_UPDATE = 5;
    const int MENU_DELETE = 6;

    const int SEARCH_BY_ID = 1;
    const int SEARCH_BY_PHONE = 2;
    const int SEARCH_BY_CUSTOMER = 3;

    const int SORT_BY_DATE_DESC = 1;
    const int SORT_BY_PRICE_DESC = 2;

    const int UPDATE_PACKAGE = 1;
    const int UPDATE_EXPIRY = 2;
    const int UPDATE_STATUS = 3;
    const int UPDATE_CANCEL = 0;

    const int STATUS_CHOICE_ACTIVE = 1;
    const int STATUS_CHOICE_SUSPENDED = 2;
    const int STATUS_CHOICE_TERMINATED = 3;

    const int TYPE_CHOICE_PREPAID = 1;
    const int TYPE_CHOICE_POSTPAID = 2;
}

class HopDongMenu {
private:
    Repository<HopDong>& store;
    Repository<KhachHang>& khachHangStore;
    Repository<ThueBao>& thueBaoStore;
    Repository<GoiCuoc>& goiCuocStore;

    //them hop dong moi
    void themHopDong();
    //hien thi danh sach hop dong
    void xemDanhSach();
    //tim kiem hop dong
    void timKiemHopDong();
    //sap xep danh sach hop dong
    void sapXepDanhSach();
    //cap nhat hop dong
    void capNhatHopDong();
    //xoa hop dong
    void xoaHopDong();

public:
    //tao menu quan ly hop dong
    HopDongMenu(
        Repository<HopDong>& s,
        Repository<KhachHang>& ks,
        Repository<ThueBao>& ts,
        Repository<GoiCuoc>& gs
    ) : store(s),
        khachHangStore(ks),
        thueBaoStore(ts),
        goiCuocStore(gs) {}
    //hien thi menu quan ly hop dong
    void showMenu();
};

#endif // HOPDONGMENU_H
