/**
 * @file goicuoc.h
 * Thanh vien phu trach: Tùng Dương (B24DCVT106)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef GOICUOC_H
#define GOICUOC_H

#include <string>
#include "Entity.h"

namespace GoiCuocConstants {
    const int COL_WIDTH_ID = 10;
    const int COL_WIDTH_NAME = 20;
    const int COL_WIDTH_PRICE = 14;
    const int COL_WIDTH_DATA = 12;
    const int COL_WIDTH_VOICE = 14;
    const int COL_WIDTH_CYCLE = 12;
}

class GoiCuoc : public Entity {
private:
    std::string tenGoi;
    double giaCuoc;
    int dungLuongData;//dung luong gb
    int soPhutGoi;//so phut thoai
    int chuKyNgay;//chu ky ngay

public:
    //tao goi cuoc mac dinh
    GoiCuoc();
    //tao goi cuoc day du thong tin
    GoiCuoc(
        const std::string& maGoi,
        const std::string& tenGoi,
        double giaCuoc,
        int dungLuongData,
        int soPhutGoi,
        int chuKyNgay = 30
    );

    //lay ma goi cuoc
    std::string getMaGoi() const { return id; }
    //lay ten goi cuoc
    std::string getTenGoi() const { return tenGoi; }
    //lay gia cuoc
    double getGiaCuoc() const { return giaCuoc; }
    //lay dung luong data
    int getDungLuongData() const { return dungLuongData; }
    //lay so phut goi
    int getSoPhutGoi() const { return soPhutGoi; }
    //lay chu ky ngay
    int getChuKyNgay() const { return chuKyNgay; }

    //cap nhat ten goi cuoc
    void setTenGoi(const std::string& ten);
    //cap nhat gia goi cuoc
    void setGiaCuoc(double gia);
    //cap nhat dung luong data
    void setDungLuongData(int data);
    //cap nhat so phut goi
    void setSoPhutGoi(int phut);
    //cap nhat chu ky su dung
    void setChuKyNgay(int chuky);

    //in tieu de danh sach goi cuoc
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong goi cuoc
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet goi cuoc
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen goi cuoc thanh dong tep
    std::string toFileString() const override;
    //nap goi cuoc tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // GOICUOC_H
