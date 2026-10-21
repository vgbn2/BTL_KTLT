#include "ghicuocgoi.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace CDRConstants;

//tao ban ghi CDR mac dinh
CDR::CDR()
    : Entity(""),
      soGoi(""),
      soNhan(""),
      loaiDichVu(TYPE_VOICE),
      thoiGianBatDau(""),
      thoiLuong(0),
      cuocPhatSinh(0.0) {}

//tao ban ghi CDR va kiem tra thong tin
CDR::CDR(
    const std::string& maCDR,
    const std::string& caller,
    const std::string& receiver,
    const std::string& type,
    const std::string& startTime,
    int duration,
    double cost
) : Entity(maCDR),
    soGoi(caller),
    soNhan(receiver),
    loaiDichVu(type),
    thoiGianBatDau(startTime),
    thoiLuong(duration),
    cuocPhatSinh(cost) {
    if (id.empty() || soGoi.empty() || soNhan.empty() || loaiDichVu.empty() || thoiGianBatDau.empty()) {
        throw AppException("Loi: Thong tin ban ghi CDR khong duoc de trong cac truong bat buoc!");
    }
    if (thoiLuong < 0) {
        throw AppException("Loi: Thoi luong cuoc goi / data khong duoc am!");
    }
    if (cuocPhatSinh < 0.0) {
        throw AppException("Loi: Cuoc phat sinh khong duoc am!");
    }
}

//in tieu de danh sach CDR
void CDR::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma CDR", "So Goi", "So Nhan", "Loai DV", "Thoi Gian", "Thoi Luong", "Cuoc (VND)"},
        {COL_WIDTH_ID, COL_WIDTH_CALLER, COL_WIDTH_RECEIVER, COL_WIDTH_TYPE,
         COL_WIDTH_TIME, COL_WIDTH_DURATION, COL_WIDTH_COST},
        {false, false, false, false, false, true, true}, os
    );
}

//in mot dong CDR
void CDR::displayRow(std::ostream& os) const {
    std::string durationStr = (loaiDichVu == TYPE_DATA) ? (std::to_string(thoiLuong) + " MB")
                                                        : (std::to_string(thoiLuong) + " s");
    DisplayHelper::printRow(
        {id, soGoi, soNhan, loaiDichVu, thoiGianBatDau, durationStr, DisplayHelper::formatCurrency(cuocPhatSinh)},
        {COL_WIDTH_ID, COL_WIDTH_CALLER, COL_WIDTH_RECEIVER, COL_WIDTH_TYPE,
         COL_WIDTH_TIME, COL_WIDTH_DURATION, COL_WIDTH_COST},
        {false, false, false, false, false, true, true}, os
    );
}

//in chi tiet CDR
void CDR::displayDetail(std::ostream& os) const {
    std::string durationStr = (loaiDichVu == TYPE_DATA) ? (std::to_string(thoiLuong) + " MB")
                                                        : (std::to_string(thoiLuong) + " giay");
    DisplayHelper::printCard({
        {"Ma ban ghi CDR", id},
        {"So thue bao goi", soGoi},
        {"So thue bao nhan", soNhan},
        {"Loai dich vu", loaiDichVu},
        {"Thoi gian bat dau", thoiGianBatDau},
        {"Thoi luong / Dung luong", durationStr},
        {"Cuoc phat sinh", DisplayHelper::formatCurrency(cuocPhatSinh, "VND")}
    }, 16, os);
}

//chuyen CDR thanh dong tep
std::string CDR::toFileString() const {
    return id + "|" + soGoi + "|" + soNhan + "|" + loaiDichVu + "|" +
           thoiGianBatDau + "|" + std::to_string(thoiLuong) + "|" +
           DisplayHelper::formatCurrency(cuocPhatSinh);
}

//nap CDR tu dong tep
bool CDR::fromFileString(const std::string& line) {
    auto tokens = FileIO::split(line, '|');
    if (tokens.size() < 7) return false;

    try {
        id = tokens[0];
        soGoi = tokens[1];
        soNhan = tokens[2];
        loaiDichVu = tokens[3];
        thoiGianBatDau = tokens[4];

        size_t idx = 0;
        thoiLuong = std::stoi(tokens[5], &idx);
        if (idx != tokens[5].length() || thoiLuong < 0) return false;

        idx = 0;
        cuocPhatSinh = std::stod(tokens[6], &idx);
        if (idx != tokens[6].length() || cuocPhatSinh < 0.0) return false;

        return !id.empty() && !soGoi.empty() && !soNhan.empty();
    } catch (...) {
        return false;
    }
}
