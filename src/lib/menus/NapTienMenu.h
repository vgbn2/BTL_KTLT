#ifndef NAPTIEN_MENU_H
#define NAPTIEN_MENU_H

#include "Repository.h"
#include "naptien.h"
#include "thuebao.h"
#include "ghicuocgoi.h"

namespace NapTienMenuChoices {
    const int MENU_BACK = 0;
    const int MENU_TOPUP = 1;
    const int MENU_LIST_VOUCHERS = 2;
    const int MENU_SEARCH_BY_PHONE = 3;
    const int MENU_ADD_CDR = 4;
    const int MENU_LIST_CDR = 5;
}

class NapTienMenu {
private:
    Repository<NapTien>& napTienStore;
    Repository<ThueBao>& thueBaoStore;
    Repository<CDR>& cdrStore;

    //lap phieu nap tien
    void lapPhieuNapTien();
    //hien thi danh sach phieu nap
    void xemDanhSachPhieu();
    //tra cuu nap tien theo so dien thoai
    void traCuuTheoSoDienThoai();
    //ghi nhan ban ghi CDR
    void ghiNhanCDR();
    //hien thi danh sach CDR
    void xemDanhSachCDR();

public:
    //tao menu nap tien va CDR
    NapTienMenu(Repository<NapTien>& ntRepo,
                Repository<ThueBao>& tbRepo,
                Repository<CDR>& cRepo)
        : napTienStore(ntRepo), thueBaoStore(tbRepo), cdrStore(cRepo) {}

    //hien thi menu nap tien va CDR
    void showMenu();
};

#endif // NAPTIEN_MENU_H
