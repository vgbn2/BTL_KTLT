#ifndef IMEI_H
#define IMEI_H

#include <string>
#include "Entity.h"
#include "Date.h"

namespace IMEIConstants {
    const size_t REQUIRED_IMEI_LENGTH = 15;
    const int MODULO_BASE = 10;
    const int LUHN_MULTIPLIER = 2;
    const int LUHN_DIGIT_OVERFLOW_SUBTRACT = 9;
    const int TAC_LENGTH = 8;

    const std::string STATUS_ACTIVE = "HoatDong";
    const std::string STATUS_LOCKED = "KhoaMang";
    const std::string STATUS_SUSPENDED = "TamKhoa";
    const std::string UNASSIGNED_PHONE = "ChuaGan";

    const int COL_WIDTH_IMEI = 18;
    const int COL_WIDTH_NAME = 20;
    const int COL_WIDTH_BRAND = 12;
    const int COL_WIDTH_PHONE = 14;
    const int COL_WIDTH_DATE = 15;
    const int COL_WIDTH_STATUS = 12;
    const int COL_WIDTH_BTS = 16;
}

class ThietBiIMEI : public Entity {
private:
    std::string tenThietBi;
    std::string hangSanXuat;
    std::string soDienThoai;
    Date ngayKichHoat;
    std::string trangThai;
    std::string tramBTSGanNhat;

public:
    //tao thiet bi IMEI mac dinh
    ThietBiIMEI();
    //tao thiet bi IMEI day du thong tin
    ThietBiIMEI(const std::string& imei,
                const std::string& tenTB,
                const std::string& hangSX,
                const std::string& sdt,
                const Date& ngayKH,
                const std::string& tThai,
                const std::string& bts);

    //kiem tra IMEI theo thuat toan luhn
    static bool validateLuhn(const std::string& imeiStr);

    //lay ma IMEI
    std::string getMaIMEI() const { return id; }
    //lay ten thiet bi
    std::string getTenThietBi() const { return tenThietBi; }
    //lay hang san xuat
    std::string getHangSanXuat() const { return hangSanXuat; }
    //lay so dien thoai gan
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay ngay kich hoat
    Date getNgayKichHoat() const { return ngayKichHoat; }
    //lay trang thai thiet bi
    std::string getTrangThai() const { return trangThai; }
    //lay tram BTS gan nhat
    std::string getTramBTSGanNhat() const { return tramBTSGanNhat; }

    //cap nhat ma IMEI
    void setMaIMEI(const std::string& imei);
    //cap nhat ten thiet bi
    void setTenThietBi(const std::string& ten) { tenThietBi = ten; }
    //cap nhat hang san xuat
    void setHangSanXuat(const std::string& hang) { hangSanXuat = hang; }
    //gan so dien thoai cho thiet bi
    void setSoDienThoai(const std::string& sdt) { ganSIM(sdt); }
    //cap nhat ngay kich hoat
    void setNgayKichHoat(const Date& d) { ngayKichHoat = d; }
    void setTrangThai(const std::string& tThai);
    //cap nhat tram BTS gan nhat
    void setTramBTSGanNhat(const std::string& bts) { tramBTSGanNhat = bts; }

    //kiem tra thiet bi co bi khoa mang
    bool isBlacklisted() const;
    //dat trang thai khoa hoac mo khoa
    void setBlacklist(bool lock);
    //gan so dien thoai cho thiet bi
    void ganSIM(const std::string& sdt);
    //go so dien thoai khoi thiet bi
    void goSIM();
    //cap nhat tram BTS gan nhat
    void capNhatBTS(const std::string& bts);

    //in tieu de danh sach thiet bi
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong thiet bi
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet thiet bi
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen thiet bi thanh dong tep
    std::string toFileString() const override;
    //nap thiet bi tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // IMEI_H
