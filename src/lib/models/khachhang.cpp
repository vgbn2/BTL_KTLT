
#include "KhachHang.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace KhachHangConstants;

// Ham khoi tao mac dinh
KhachHang::KhachHang()
    : Entity(""),
      hoTen(""),
      loaiGiayTo(""),
      soGiayTo(""),
      ngayCap(Date()),
      noiCap(""),
      soDienThoai(""),
      diaChi("") {}

// Ham khoi tao co tham so
KhachHang::KhachHang(
    const std::string& maKH,
    const std::string& ten,
    const std::string& loaiGT,
    const std::string& soGT,
    const Date& ngay,
    const std::string& noi,
    const std::string& sdt,
    const std::string& dc
)
    : Entity(maKH),
      hoTen(ten),
      loaiGiayTo(loaiGT),
      soGiayTo(soGT),
      ngayCap(ngay),
      noiCap(noi),
      soDienThoai(sdt),
      diaChi(dc) {

    if (!soDienThoai.empty() &&
        !Normalized::isValidPhoneNumber(soDienThoai)) {
        throw InvalidPhoneNumberException(soDienThoai);
    }
}

// Cac ham cap nhat thong tin
void KhachHang::setHoTen(const std::string& value) {
    hoTen = value;
}

void KhachHang::setLoaiGiayTo(const std::string& value) {
    loaiGiayTo = value;
}

void KhachHang::setSoGiayTo(const std::string& value) {
    soGiayTo = value;
}

void KhachHang::setNgayCap(const Date& value) {
    ngayCap = value;
}

void KhachHang::setNoiCap(const std::string& value) {
    noiCap = value;
}

void KhachHang::setSoDienThoai(const std::string& value) {
    if (!value.empty() &&
        !Normalized::isValidPhoneNumber(value)) {
        throw InvalidPhoneNumberException(value);
    }

    soDienThoai = value;
}

void KhachHang::setDiaChi(const std::string& value) {
    diaChi = value;
}

// Hien thi tieu de bang
void KhachHang::displayHeader() const {
    DisplayHelper::printHeader(
        {
            "Ma KH",
            "Ho Ten",
            "Loai Giay To",
            "So Giay To",
            "Ngay Cap",
            "Noi Cap",
            "So Dien Thoai",
            "Dia Chi"
        },
        {
            COL_WIDTH_ID,
            COL_WIDTH_NAME,
            COL_WIDTH_DOC_TYPE,
            COL_WIDTH_DOC_NUMBER,
            COL_WIDTH_DATE,
            COL_WIDTH_ISSUER,
            COL_WIDTH_PHONE,
            COL_WIDTH_ADDRESS
        },
        {false, false, false, false, false, false, false, false}
    );
}

// Hien thi mot dong khach hang
void KhachHang::displayRow() const {
    DisplayHelper::printRow(
        {
            id,
            hoTen,
            loaiGiayTo,
            soGiayTo,
            ngayCap.toString(),
            noiCap,
            soDienThoai,
            diaChi
        },
        {
            COL_WIDTH_ID,
            COL_WIDTH_NAME,
            COL_WIDTH_DOC_TYPE,
            COL_WIDTH_DOC_NUMBER,
            COL_WIDTH_DATE,
            COL_WIDTH_ISSUER,
            COL_WIDTH_PHONE,
            COL_WIDTH_ADDRESS
        },
        {false, false, false, false, false, false, false, false}
    );
}

// Hien thi chi tiet khach hang
void KhachHang::displayDetail() const {
    DisplayHelper::printCard({
        {"Ma khach hang", id},
        {"Ho ten", hoTen},
        {"Loai giay to", loaiGiayTo},
        {"So giay to", soGiayTo},
        {"Ngay cap", ngayCap.toString()},
        {"Noi cap", noiCap},
        {"So dien thoai", soDienThoai},
        {"Dia chi", diaChi}
    });
}

// Chuyen thong tin khach hang thanh chuoi de luu file
std::string KhachHang::toFileString() const {
    std::ostringstream oss;

    oss << id << "|"
        << hoTen << "|"
        << loaiGiayTo << "|"
        << soGiayTo << "|"
        << ngayCap.toString() << "|"
        << noiCap << "|"
        << soDienThoai << "|"
        << diaChi;

    return oss.str();
}

// Doc thong tin khach hang tu mot dong trong file
bool KhachHang::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');

    const size_t EXPECTED_FIELD_COUNT = 8;

    if (tokens.size() != EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        KhachHang parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tokens[3],
            Date::parse(tokens[4]),
            tokens[5],
            tokens[6],
            tokens[7]
        );

        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
