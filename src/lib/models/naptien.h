#ifndef NAPTIEN_H
#define NAPTIEN_H

#include <string>
#include "Entity.h"

namespace NapTienConstants {
    const std::string METHOD_SCRATCH_CARD = "TheCao";
    const std::string METHOD_BANKING = "NganHang";
    const std::string METHOD_E_WALLET = "ViDienTu";

    const std::string STATUS_SUCCESS = "ThanhCong";
    const std::string STATUS_FAILED = "ThatBai";

    const int COL_WIDTH_ID = 12;
    const int COL_WIDTH_PHONE = 15;
    const int COL_WIDTH_DATE = 14;
    const int COL_WIDTH_AMOUNT = 16;
    const int COL_WIDTH_METHOD = 15;
    const int COL_WIDTH_STATUS = 14;
}

class NapTien : public Entity {
private:
    std::string soDienThoai;
    std::string ngayNap;
    double soTien;
    std::string hinhThucNap;
    std::string trangThai;

public:
    //tao phieu nap tien mac dinh
    NapTien();
    //tao phieu nap tien day du thong tin
    NapTien(
        const std::string& maGD,
        const std::string& sdt,
        const std::string& ngay,
        double tien,
        const std::string& hinhThuc = NapTienConstants::METHOD_BANKING,
        const std::string& tThai = NapTienConstants::STATUS_SUCCESS
    );

    //lay ma giao dich
    std::string getMaGiaoDich() const { return id; }
    //lay so dien thoai
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay ngay nap
    std::string getNgayNap() const { return ngayNap; }
    //lay so tien nap
    double getSoTien() const { return soTien; }
    //lay hinh thuc nap
    std::string getHinhThucNap() const { return hinhThucNap; }
    //lay trang thai phieu nap
    std::string getTrangThai() const { return trangThai; }

    //cap nhat trang thai phieu nap
    void setTrangThai(const std::string& tThai);

    //in tieu de danh sach phieu nap
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong phieu nap
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet phieu nap
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen phieu nap thanh dong tep
    std::string toFileString() const override;
    //nap phieu nap tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // NAPTIEN_H
