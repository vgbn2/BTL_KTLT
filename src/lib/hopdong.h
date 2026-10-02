#ifndef HOPDONG_H
#define HOPDONG_H

#include <string>
#include "Entity.h"
#include "Date.h"

namespace HopDongConstants {
    const std::string STATUS_ACTIVE = "HieuLuc";
    const std::string STATUS_SUSPENDED = "TamDung";
    const std::string STATUS_TERMINATED = "ThanhLy";

    const std::string TYPE_PREPAID = "TraTruoc";
    const std::string TYPE_POSTPAID = "TraSau";

    const int COL_WIDTH_ID = 10;
    const int COL_WIDTH_CUST = 10;
    const int COL_WIDTH_PHONE = 14;
    const int COL_WIDTH_PACKAGE = 10;
    const int COL_WIDTH_DATE = 13;
    const int COL_WIDTH_TYPE = 11;
    const int COL_WIDTH_STATUS = 12;
    const int COL_WIDTH_PRICE = 14;
}

class HopDong : public Entity {
private:
    std::string maKhachHang;
    std::string soDienThoai;
    std::string maGoiCuoc;
    Date ngayDangKy;
    Date ngayHetHan;
    std::string loaiHopDong;
    std::string trangThai;
    double giaTriGoi;

public:
    HopDong();
    HopDong(const std::string& maHD,
            const std::string& maKH,
            const std::string& sdt,
            const std::string& maGC,
            const Date& ngayDK,
            const Date& ngayHH,
            const std::string& loaiHD,
            const std::string& tThai,
            double gia);

    std::string getMaHopDong() const { return id; }
    std::string getMaKhachHang() const { return maKhachHang; }
    std::string getSoDienThoai() const { return soDienThoai; }
    std::string getMaGoiCuoc() const { return maGoiCuoc; }
    Date getNgayDangKy() const { return ngayDangKy; }
    Date getNgayHetHan() const { return ngayHetHan; }
    std::string getLoaiHopDong() const { return loaiHopDong; }
    std::string getTrangThai() const { return trangThai; }
    double getGiaTriGoi() const { return giaTriGoi; }

    void setMaKhachHang(const std::string& maKH) { maKhachHang = maKH; }
    void setSoDienThoai(const std::string& sdt);
    void setMaGoiCuoc(const std::string& maGC) { maGoiCuoc = maGC; }
    void setNgayDangKy(const Date& d) { ngayDangKy = d; }
    void setNgayHetHan(const Date& d);
    void setLoaiHopDong(const std::string& loai);
    void setTrangThai(const std::string& tThai);
    void setGiaTriGoi(double gia);

    bool isExpired(const Date& currentDate) const;
    void giaHan(const Date& ngayHetHanMoi);
    void chamDut();
    void tamDung();
    void kichHoatLai();

    void displayHeader() const override;
    void displayRow() const override;
    void displayDetail() const override;

    std::string toFileString() const override;
    bool fromFileString(const std::string& line) override;
};

#endif // HOPDONG_H
