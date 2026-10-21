#ifndef HOADON_MENU_H
#define HOADON_MENU_H

#include "Repository.h"
#include "hoadon.h"
#include "thuebao.h"
#include "khieunai.h"
#include "Date.h"

namespace HoaDonMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_CREATE_INVOICE = 1;
    const int MENU_LIST_INVOICES = 2;
    const int MENU_PAY_INVOICE = 3;
    const int MENU_SEARCH_BY_PHONE = 4;
    const int MENU_ADD_COMPLAINT = 5;
    const int MENU_PROCESS_COMPLAINT = 6;
    const int MENU_LIST_COMPLAINTS = 7;
}

class HoaDonMenu {
private:
    Repository<HoaDon>& hoaDonStore;
    Repository<ThueBao>& thueBaoStore;
    Repository<KhieuNai>& khieuNaiStore;

    //lap hoa don cuoc phat sinh
    void lapHoaDon();
    //hien thi danh sach hoa don
    void xemDanhSachHoaDon();
    //thanh toan hoa don
    void thanhToanHoaDon();
    //tra cuu hoa don theo so dien thoai
    void traCuuTheoSoDienThoai();
    //tiep nhan phieu khieu nai
    void tiepNhanKhieuNai();
    //xu ly phieu khieu nai
    void xuLyKhieuNai();
    //hien thi danh sach khieu nai
    void xemDanhSachKhieuNai();

public:
    //kiem tra va khoa thue bao no cuoc
    int kiemTraNo(const Date& ngay = Date::now());
    //tao menu quan ly hoa don va khieu nai
    HoaDonMenu(Repository<HoaDon>& hdRepo,
               Repository<ThueBao>& tbRepo,
               Repository<KhieuNai>& knRepo)
        : hoaDonStore(hdRepo), thueBaoStore(tbRepo), khieuNaiStore(knRepo) {}

    //hien thi menu quan ly hoa don va khieu nai
    void showMenu();
};

#endif // HOADON_MENU_H
