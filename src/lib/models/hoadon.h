/**
 * @file hoadon.h
 * Thanh vien phu trach: Nguyễn Minh Quang (B24DCVT308)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef HOADON_H
#define HOADON_H

#include <string>
#include "Entity.h"
#include "Date.h"

namespace HoaDonConstants {
    const std::string STATUS_PAID = "DaThanhToan";
    const std::string STATUS_UNPAID = "ChuaThanhToan";

    const int COL_WIDTH_ID = 10;
    const int COL_WIDTH_PHONE = 15;
    const int COL_WIDTH_CYCLE = 12;
    const int COL_WIDTH_CALL = 14;
    const int COL_WIDTH_DATA = 14;
    const int COL_WIDTH_TOTAL = 16;
    const int COL_WIDTH_STATUS = 15;
}

class HoaDon : public Entity {
private:
    std::string soDienThoai;
    std::string kyCuoc;//ky cuoc mm/yyyy
    double tienGoi;
    double tienData;
    double tongTien;
    std::string trangThai;

public:
    //tao hoa don mac dinh
    HoaDon();
    //tao hoa don day du thong tin
    HoaDon(
        const std::string& maHD,
        const std::string& sdt,
        const std::string& ky,
        double tGoi,
        double tData,
        double tong,
        const std::string& tThai = HoaDonConstants::STATUS_PAID
    );

    //lay ma hoa don
    std::string getMaHoaDon() const { return id; }
    //lay so dien thoai
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay ky cuoc
    std::string getKyCuoc() const { return kyCuoc; }
    //lay tien goi
    double getTienGoi() const { return tienGoi; }
    //lay tien data
    double getTienData() const { return tienData; }
    //lay tong tien
    double getTongTien() const { return tongTien; }
    //lay trang thai hoa don
    std::string getTrangThai() const { return trangThai; }
    //kiem tra ky cuoc da qua thang hien tai
    bool quaHan(const Date& ngay) const;

    //cap nhat trang thai hoa don
    void setTrangThai(const std::string& tThai);
    //chuyen hoa don sang da thanh toan
    void thanhToan();

    //in tieu de danh sach hoa don
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong hoa don
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet hoa don
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen hoa don thanh dong tep
    std::string toFileString() const override;
    //nap hoa don tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // HOADON_H
