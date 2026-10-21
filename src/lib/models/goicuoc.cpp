#include "goicuoc.h"
#include "Exceptions.h"
#include "fileio.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace GoiCuocConstants;

//tao goi cuoc mac dinh
GoiCuoc::GoiCuoc()
    : Entity(""),
      tenGoi(""),
      giaCuoc(0.0),
      dungLuongData(0),
      soPhutGoi(0),
      chuKyNgay(30) {}

//tao goi cuoc va kiem tra thong tin
GoiCuoc::GoiCuoc(
    const std::string& maGoi,
    const std::string& ten,
    double gia,
    int data,
    int goi,
    int chuky
) : Entity(maGoi),
    tenGoi(ten),
    giaCuoc(gia),
    dungLuongData(data),
    soPhutGoi(goi),
    chuKyNgay(chuky) {
    if (id.empty() || tenGoi.empty()) {
        throw AppException("Loi: Ma goi va ten goi khong duoc de trong!");
    }
    if (giaCuoc < 0.0) {
        throw AppException("Loi: Gia cuoc khong the am!");
    }
    if (dungLuongData < 0 || soPhutGoi < 0 || chuKyNgay <= 0) {
        throw AppException("Loi: Thong so data/thoai/chu ky khong hop le!");
    }
}

//cap nhat ten goi cuoc
void GoiCuoc::setTenGoi(const std::string& ten) {
    tenGoi = ten;
}

//cap nhat gia goi cuoc
void GoiCuoc::setGiaCuoc(double gia) {
    if (gia < 0.0) {
        throw AppException("Loi: Gia cuoc khong the am!");
    }
    giaCuoc = gia;
}

//cap nhat dung luong data
void GoiCuoc::setDungLuongData(int data) {
    if (data < 0) {
        throw AppException("Loi: Dung luong data khong hop le!");
    }
    dungLuongData = data;
}

//cap nhat so phut goi
void GoiCuoc::setSoPhutGoi(int phut) {
    if (phut < 0) {
        throw AppException("Loi: So phut goi khong hop le!");
    }
    soPhutGoi = phut;
}

//cap nhat chu ky su dung
void GoiCuoc::setChuKyNgay(int chuky) {
    if (chuky <= 0) {
        throw AppException("Loi: Chu ky ngay phai lon hon 0!");
    }
    chuKyNgay = chuky;
}

//in tieu de danh sach goi cuoc
void GoiCuoc::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma Goi", "Ten Goi Cuoc", "Gia Cuoc (VND)", "Data (GB)", "Thoai (Phut)", "Chu Ky (Ngay)"},
        {COL_WIDTH_ID, COL_WIDTH_NAME, COL_WIDTH_PRICE, COL_WIDTH_DATA, COL_WIDTH_VOICE, COL_WIDTH_CYCLE},
        {false, false, true, true, true, true}, os
    );
}

//in mot dong goi cuoc
void GoiCuoc::displayRow(std::ostream& os) const {
    std::ostringstream ossPrice;
    ossPrice << std::fixed << std::setprecision(0) << giaCuoc;
    DisplayHelper::printRow(
        {id, tenGoi, ossPrice.str(), std::to_string(dungLuongData), std::to_string(soPhutGoi), std::to_string(chuKyNgay)},
        {COL_WIDTH_ID, COL_WIDTH_NAME, COL_WIDTH_PRICE, COL_WIDTH_DATA, COL_WIDTH_VOICE, COL_WIDTH_CYCLE},
        {false, false, true, true, true, true}, os
    );
}

//in chi tiet goi cuoc
void GoiCuoc::displayDetail(std::ostream& os) const {
    std::ostringstream ossPrice;
    ossPrice << std::fixed << std::setprecision(0) << giaCuoc << " VND";
    DisplayHelper::printCard({
        {"Ma goi cuoc", id},
        {"Ten goi cuoc", tenGoi},
        {"Gia cuoc", ossPrice.str()},
        {"Dung luong data", std::to_string(dungLuongData) + " GB"},
        {"So phut goi", std::to_string(soPhutGoi) + " phut"},
        {"Chu ky su dung", std::to_string(chuKyNgay) + " ngay"}
    }, 16, os);
}

//chuyen goi cuoc thanh dong tep
std::string GoiCuoc::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << tenGoi << "|"
        << std::fixed << std::setprecision(0) << giaCuoc << "|"
        << dungLuongData << "|"
        << soPhutGoi << "|"
        << chuKyNgay;
    return oss.str();
}

//nap goi cuoc tu dong tep
bool GoiCuoc::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    if (tokens.size() != 6) {
        return false;
    }

    try {
        //doc va kiem tra gia tri so,chong chuoi rac
        size_t idxGia = 0, idxData = 0, idxGoi = 0, idxCK = 0;
        double gia = std::stod(tokens[2], &idxGia);
        int data = std::stoi(tokens[3], &idxData);
        int goi = std::stoi(tokens[4], &idxGoi);
        int chuky = std::stoi(tokens[5], &idxCK);
        if (idxGia != tokens[2].length() || idxData != tokens[3].length() ||
            idxGoi != tokens[4].length() || idxCK != tokens[5].length()) {
            return false;
        }
        GoiCuoc parsed(
            tokens[0],
            tokens[1],
            gia,
            data,
            goi,
            chuky
        );
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
