#include "ghicuocgoi.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include "Date.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>

using namespace CDRConstants;

namespace {

bool isKnownServiceType(const std::string& type) {
    return type == TYPE_VOICE || type == TYPE_SMS || type == TYPE_DATA;
}

bool isValidStartTime(const std::string& value) {
    std::istringstream input(value);
    std::string date;
    int hour = 0;
    int minute = 0;
    int second = 0;
    char firstSeparator = 0;
    char secondSeparator = 0;
    std::string trailing;

    if (!(input >> date >> hour >> firstSeparator >> minute >> secondSeparator >> second) ||
        firstSeparator != ':' || secondSeparator != ':' || (input >> trailing) ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {
        return false;
    }

    try {
        Date::parse(date);
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace

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
    if (!isKnownServiceType(loaiDichVu)) {
        throw AppException("Loi: Loai dich vu CDR khong hop le!");
    }
    if (!Normalized::isValidPhoneNumber(soGoi) ||
        (loaiDichVu == TYPE_DATA ? soNhan != "DATA_SESSION" : !Normalized::isValidPhoneNumber(soNhan))) {
        throw AppException("Loi: So dien thoai CDR khong hop le!");
    }
    if (!isValidStartTime(thoiGianBatDau)) {
        throw AppException("Loi: Thoi gian bat dau CDR khong hop le!");
    }
    if (thoiLuong < 0) {
        throw AppException("Loi: Thoi luong cuoc goi / data khong duoc am!");
    }
    if (!std::isfinite(cuocPhatSinh) || cuocPhatSinh < 0.0) {
        throw AppException("Loi: Cuoc phat sinh khong duoc am hoac khong huu han!");
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
    if (tokens.size() != 7) return false;

    try {
        //phan tich tung truong truoc khi gan ban ghi da kiem tra vao doi tuong.
        size_t durationIndex = 0;
        int duration = std::stoi(tokens[5], &durationIndex);
        if (durationIndex != tokens[5].length()) return false;

        size_t costIndex = 0;
        double cost = std::stod(tokens[6], &costIndex);
        if (costIndex != tokens[6].length()) return false;

        CDR parsed(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], duration, cost);
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
