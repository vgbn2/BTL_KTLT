/**
 * @file hopdong.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

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
    const int COL_WIDTH_PRICE = 16;
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
    //tao hop dong mac dinh
    HopDong();
    //tao hop dong day du thong tin
    HopDong(const std::string& maHD,
            const std::string& maKH,
            const std::string& sdt,
            const std::string& maGC,
            const Date& ngayDK,
            const Date& ngayHH,
            const std::string& loaiHD,
            const std::string& tThai,
            double gia);

    //lay ma hop dong
    std::string getMaHopDong() const { return id; }
    //lay ma khach hang
    std::string getMaKhachHang() const { return maKhachHang; }
    //lay so dien thoai
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay ma goi cuoc
    std::string getMaGoiCuoc() const { return maGoiCuoc; }
    //lay ngay dang ky
    Date getNgayDangKy() const { return ngayDangKy; }
    //lay ngay het han
    Date getNgayHetHan() const { return ngayHetHan; }
    //lay loai hop dong
    std::string getLoaiHopDong() const { return loaiHopDong; }
    //lay trang thai hop dong
    std::string getTrangThai() const { return trangThai; }
    //lay gia tri goi
    double getGiaTriGoi() const { return giaTriGoi; }

    //cap nhat ma khach hang
    void setMaKhachHang(const std::string& maKH) { maKhachHang = maKH; }
    //cap nhat so dien thoai
    //cap nhat so dien thoai
    void setSoDienThoai(const std::string& sdt);
    //cap nhat ma goi cuoc
    void setMaGoiCuoc(const std::string& maGC) { maGoiCuoc = maGC; }
    //cap nhat ngay dang ky
    void setNgayDangKy(const Date& d) { ngayDangKy = d; }
    //cap nhat ngay het han
    void setNgayHetHan(const Date& d);
    //cap nhat loai hop dong
    void setLoaiHopDong(const std::string& loai);
    //cap nhat trang thai hop dong
    void setTrangThai(const std::string& tThai);
    //cap nhat gia tri goi
    void setGiaTriGoi(double gia);

    //kiem tra hop dong da het han
    bool isExpired(const Date& currentDate) const;
    //gia han hop dong
    void giaHan(const Date& ngayHetHanMoi);
    //cham dut hop dong
    void chamDut();
    //tam dung hop dong
    void tamDung();
    //kich hoat lai hop dong
    void kichHoatLai();

    //in tieu de danh sach hop dong
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong hop dong
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet hop dong
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen hop dong thanh dong tep
    std::string toFileString() const override;
    //nap hop dong tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // HOPDONG_H
