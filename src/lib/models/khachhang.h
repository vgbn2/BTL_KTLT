/**
 * @file khachhang.h
 * Thanh vien phu trach: Trương Khánh (B24DCVT201)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef KHACHHANG_H
#define KHACHHANG_H

#include <string>
#include "Entity.h"

namespace KhachHangConstants {
    const int COL_WIDTH_ID = 10;
    const int COL_WIDTH_NAME = 22;
    const int COL_WIDTH_DOC_TYPE = 12;
    const int COL_WIDTH_DOC_NUMBER = 18;
    const int COL_WIDTH_DATE = 13;
    const int COL_WIDTH_ISSUER = 25;
    const int COL_WIDTH_PHONE = 15;
    const int COL_WIDTH_ADDRESS = 20;
}

class KhachHang : public Entity {
private:
    std::string hoTen;
    std::string loaiGiayTo;
    std::string soGiayTo;
    std::string ngayCap;
    std::string noiCap;
    std::string soDienThoai;
    std::string diaChi;

public:
    //tao khach hang mac dinh
    KhachHang();
    //tao khach hang day du thong tin
    KhachHang(
        const std::string& maKH,
        const std::string& hoTen,
        const std::string& loaiGiayTo,
        const std::string& soGiayTo,
        const std::string& ngayCap,
        const std::string& noiCap,
        const std::string& soDienThoai,
        const std::string& diaChi
    );

    //lay ma khach hang
    std::string getMaKhachHang() const { return id; }
    //lay ho ten
    std::string getHoTen() const { return hoTen; }
    //lay loai giay to
    std::string getLoaiGiayTo() const { return loaiGiayTo; }
    //lay so giay to
    std::string getSoGiayTo() const { return soGiayTo; }
    //lay ngay cap
    std::string getNgayCap() const { return ngayCap; }
    //lay noi cap
    std::string getNoiCap() const { return noiCap; }
    //lay so dien thoai
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay dia chi
    std::string getDiaChi() const { return diaChi; }

    //cap nhat ho ten khach hang
    void setHoTen(const std::string& value);
    //cap nhat loai giay to
    void setLoaiGiayTo(const std::string& value);
    //cap nhat so giay to
    void setSoGiayTo(const std::string& value);
    //cap nhat ngay cap
    void setNgayCap(const std::string& value);
    //cap nhat noi cap
    void setNoiCap(const std::string& value);
    //cap nhat so dien thoai
    void setSoDienThoai(const std::string& value);
    //cap nhat dia chi
    void setDiaChi(const std::string& value);

    //in tieu de danh sach khach hang
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong khach hang
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet khach hang
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen khach hang thanh dong tep
    std::string toFileString() const override;
    //nap khach hang tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // KHACHHANG_H
