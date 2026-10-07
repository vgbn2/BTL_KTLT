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
    ThietBiIMEI();
    ThietBiIMEI(const std::string& imei,
                const std::string& tenTB,
                const std::string& hangSX,
                const std::string& sdt,
                const Date& ngayKH,
                const std::string& tThai,
                const std::string& bts);

    static bool validateLuhn(const std::string& imeiStr);

    std::string getMaIMEI() const { return id; }
    std::string getTenThietBi() const { return tenThietBi; }
    std::string getHangSanXuat() const { return hangSanXuat; }
    std::string getSoDienThoai() const { return soDienThoai; }
    Date getNgayKichHoat() const { return ngayKichHoat; }
    std::string getTrangThai() const { return trangThai; }
    std::string getTramBTSGanNhat() const { return tramBTSGanNhat; }

    void setMaIMEI(const std::string& imei);
    void setTenThietBi(const std::string& ten) { tenThietBi = ten; }
    void setHangSanXuat(const std::string& hang) { hangSanXuat = hang; }
    void setSoDienThoai(const std::string& sdt) { ganSIM(sdt); }
    void setNgayKichHoat(const Date& d) { ngayKichHoat = d; }
    void setTrangThai(const std::string& tThai);
    void setTramBTSGanNhat(const std::string& bts) { tramBTSGanNhat = bts; }

    bool isBlacklisted() const;
    void setBlacklist(bool lock);
    void ganSIM(const std::string& sdt);
    void goSIM();
    void capNhatBTS(const std::string& bts);

    void displayHeader() const override;
    void displayRow() const override;
    void displayDetail() const override;

    std::string toFileString() const override;
    bool fromFileString(const std::string& line) override;
};

#endif // IMEI_H
