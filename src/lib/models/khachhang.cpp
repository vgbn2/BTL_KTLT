#include "khachhang.h"
#include "Date.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <sstream>

using namespace KhachHangConstants;

//tao khach hang mac dinh
KhachHang::KhachHang()
    : Entity(""),
      hoTen(""),
      loaiGiayTo("CCCD"),
      soGiayTo(""),
      ngayCap(""),
      noiCap(""),
      soDienThoai(""),
      diaChi("") {}

//tao khach hang va kiem tra thong tin
KhachHang::KhachHang(
    const std::string& maKH,
    const std::string& ten,
    const std::string& loaiGT,
    const std::string& soGT,
    const std::string& nCap,
    const std::string& noiC,
    const std::string& sdt,
    const std::string& dc
) : Entity(maKH),
    hoTen(ten),
    loaiGiayTo(loaiGT),
    soGiayTo(soGT),
    ngayCap(nCap),
    noiCap(noiC),
    soDienThoai(sdt),
    diaChi(dc) {
    if (id.empty() || !Normalized::isValidName(hoTen) || loaiGiayTo.empty() || soGiayTo.empty() ||
        ngayCap.empty() || noiCap.empty() || soDienThoai.empty() || diaChi.empty()) {
        throw AppException("Loi: Thong tin bat buoc cua khach hang khong hop le!");
    }
    if (!Normalized::isValidCustomerId(id)) {
        throw AppException("Loi: Ma khach hang phai co dang KH0001 hoac KH0000001!");
    }
    if (loaiGiayTo != "CCCD" || !Normalized::isValidCredential(soGiayTo)) {
        throw InvalidCredentialException(soGiayTo);
    }
    Date::parse(ngayCap);
    if (!Normalized::isValidPhoneNumber(soDienThoai)) {
        throw InvalidPhoneNumberException(soDienThoai);
    }
}

//cap nhat ho ten khach hang
void KhachHang::setHoTen(const std::string& value) {
    if (!Normalized::isValidName(value)) {
        throw AppException("Loi: Ho ten khong duoc rong hoac qua 50 ky tu!");
    }
    hoTen = value;
}

//cap nhat loai giay to
void KhachHang::setLoaiGiayTo(const std::string& value) {
    loaiGiayTo = value;
}

//cap nhat so giay to
void KhachHang::setSoGiayTo(const std::string& value) {
    if (!Normalized::isValidCredential(value)) {
        throw InvalidCredentialException(value);
    }
    soGiayTo = value;
}

//cap nhat ngay cap giay to
void KhachHang::setNgayCap(const std::string& value) {
    ngayCap = value;
}

//cap nhat noi cap giay to
void KhachHang::setNoiCap(const std::string& value) {
    noiCap = value;
}

//cap nhat so dien thoai khach hang
void KhachHang::setSoDienThoai(const std::string& value) {
    if (!value.empty() && !Normalized::isValidPhoneNumber(value)) {
        throw InvalidPhoneNumberException(value);
    }
    soDienThoai = value;
}

//cap nhat dia chi khach hang
void KhachHang::setDiaChi(const std::string& value) {
    diaChi = value;
}

//in tieu de danh sach khach hang
void KhachHang::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma KH", "Ho Va Ten", "Loai GT", "So Giay To", "Ngay Cap", "Noi Cap", "So Dien Thoai", "Dia Chi"},
        {COL_WIDTH_ID, COL_WIDTH_NAME, COL_WIDTH_DOC_TYPE, COL_WIDTH_DOC_NUMBER, COL_WIDTH_DATE, COL_WIDTH_ISSUER, COL_WIDTH_PHONE, COL_WIDTH_ADDRESS},
        {false, false, false, false, false, false, false, false}, os
    );
}

//in mot dong khach hang
void KhachHang::displayRow(std::ostream& os) const {
    DisplayHelper::printRow(
        {id, hoTen, loaiGiayTo, soGiayTo, ngayCap, noiCap, soDienThoai, diaChi},
        {COL_WIDTH_ID, COL_WIDTH_NAME, COL_WIDTH_DOC_TYPE, COL_WIDTH_DOC_NUMBER, COL_WIDTH_DATE, COL_WIDTH_ISSUER, COL_WIDTH_PHONE, COL_WIDTH_ADDRESS},
        {false, false, false, false, false, false, false, false}, os
    );
}

//in chi tiet khach hang
void KhachHang::displayDetail(std::ostream& os) const {
    DisplayHelper::printCard({
        {"Ma khach hang", id},
        {"Ho va ten", hoTen},
        {"Loai giay to", loaiGiayTo},
        {"So giay to", soGiayTo},
        {"Ngay cap", ngayCap},
        {"Noi cap", noiCap},
        {"So dien thoai", soDienThoai},
        {"Dia chi", diaChi}
    }, 16, os);
}

//chuyen khach hang thanh dong tep
std::string KhachHang::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << hoTen << "|"
        << loaiGiayTo << "|"
        << soGiayTo << "|"
        << ngayCap << "|"
        << noiCap << "|"
        << soDienThoai << "|"
        << diaChi;
    return oss.str();
}

//nap khach hang tu dong tep
bool KhachHang::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    const size_t EXPECTED_FIELD_COUNT = 8;
    if (tokens.size() != EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        //tao ban ghi tam da kiem tra de dong sai khong ghi de mot phan doi tuong hien tai.
        //doc va kiem tra thong tin khach hang,chan du thua
        KhachHang parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tokens[3],
            tokens[4],
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
