#ifndef THUEBAO_H
#define THUEBAO_H

#include <string>
#include "Entity.h"

namespace ThueBaoConstants {
    const std::string TYPE_PREPAID = "TraTruoc";
    const std::string TYPE_POSTPAID = "TraSau";

    const std::string STATUS_ACTIVE = "HoatDong";
    const std::string STATUS_LOCKED_1WAY = "Khoa1Chieu";
    const std::string STATUS_LOCKED_2WAY = "Khoa2Chieu";
    const std::string STATUS_LOCKED_DEBT = "KhoaNo";
    const std::string STATUS_CANCELLED = "Huy";

    const int COL_WIDTH_PHONE = 15;
    const int COL_WIDTH_TYPE = 12;
    const int COL_WIDTH_CUST = 10;
    const int COL_WIDTH_PACKAGE = 10;
    const int COL_WIDTH_DATE = 13;
    const int COL_WIDTH_STATUS = 14;
    const int COL_WIDTH_BALANCE = 16;
}

class ThueBao : public Entity {
private:
    std::string loaiThueBao;
    std::string maKhachHang;
    std::string maGoiCuoc;
    std::string ngayKichHoat;
    std::string trangThai;
    double soDuTaiKhoan;

public:
    //tao thue bao mac dinh
    ThueBao();
    //tao thue bao day du thong tin
    ThueBao(
        const std::string& soDienThoai,
        const std::string& loaiTB,
        const std::string& maKH,
        const std::string& maGC,
        const std::string& ngayKH,
        const std::string& tThai = ThueBaoConstants::STATUS_ACTIVE,
        double soDu = 0.0
    );

    //lay so dien thoai
    std::string getSoDienThoai() const { return id; }
    //lay loai thue bao
    std::string getLoaiThueBao() const { return loaiThueBao; }
    //lay ma khach hang
    std::string getMaKhachHang() const { return maKhachHang; }
    //lay ma goi cuoc
    std::string getMaGoiCuoc() const { return maGoiCuoc; }
    //lay ngay kich hoat
    std::string getNgayKichHoat() const { return ngayKichHoat; }
    //lay trang thai thue bao
    std::string getTrangThai() const { return trangThai; }
    //lay so du tai khoan
    double getSoDuTaiKhoan() const { return soDuTaiKhoan; }

    //cap nhat loai thue bao
    void setLoaiThueBao(const std::string& loai);
    //cap nhat ma khach hang
    void setMaKhachHang(const std::string& maKH);
    //cap nhat ma goi cuoc
    void setMaGoiCuoc(const std::string& maGC);
    //cap nhat ngay kich hoat
    void setNgayKichHoat(const std::string& ngay);
    //cap nhat trang thai thue bao
    void setTrangThai(const std::string& tThai);
    //cap nhat so du tai khoan
    void setSoDuTaiKhoan(double soDu);
    //cong tien vao tai khoan
    void congTien(double tien);
    //tru tien khoi tai khoan
    void truTien(double tien);

    //in tieu de danh sach thue bao
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong thue bao
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet thue bao
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen thue bao thanh dong tep
    std::string toFileString() const override;
    //nap thue bao tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // THUEBAO_H
