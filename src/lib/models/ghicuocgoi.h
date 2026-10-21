#ifndef CDR_H
#define CDR_H

#include <string>
#include "Entity.h"

namespace CDRConstants {
    const std::string TYPE_VOICE = "Thoai";
    const std::string TYPE_SMS = "SMS";
    const std::string TYPE_DATA = "Data";

    const int COL_WIDTH_ID = 12;
    const int COL_WIDTH_CALLER = 14;
    const int COL_WIDTH_RECEIVER = 14;
    const int COL_WIDTH_TYPE = 10;
    const int COL_WIDTH_TIME = 22;
    const int COL_WIDTH_DURATION = 14;
    const int COL_WIDTH_COST = 14;
}

class CDR : public Entity {
private:
    std::string soGoi;
    std::string soNhan;
    std::string loaiDichVu;
    std::string thoiGianBatDau;
    int thoiLuong; // giay (hoac MB voi Data)
    double cuocPhatSinh; // VND

public:
    //tao ban ghi CDR mac dinh
    CDR();
    //tao ban ghi CDR day du thong tin
    CDR(
        const std::string& maCDR,
        const std::string& caller,
        const std::string& receiver,
        const std::string& type,
        const std::string& startTime,
        int duration,
        double cost
    );

    //lay ma CDR
    std::string getMaCDR() const { return id; }
    //lay so thue bao goi
    std::string getSoGoi() const { return soGoi; }
    //lay so thue bao nhan
    std::string getSoNhan() const { return soNhan; }
    //lay loai dich vu
    std::string getLoaiDichVu() const { return loaiDichVu; }
    //lay thoi gian bat dau
    std::string getThoiGianBatDau() const { return thoiGianBatDau; }
    //lay thoi luong hoac dung luong
    int getThoiLuong() const { return thoiLuong; }
    //lay cuoc phat sinh
    double getCuocPhatSinh() const { return cuocPhatSinh; }

    //in tieu de danh sach CDR
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong CDR
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet CDR
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen CDR thanh dong tep
    std::string toFileString() const override;
    //nap CDR tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // CDR_H
