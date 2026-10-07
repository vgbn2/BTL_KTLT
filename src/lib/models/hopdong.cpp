#include "hopdong.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace HopDongConstants;

HopDong::HopDong()
    : Entity(""),
      maKhachHang(""),
      soDienThoai(""),
      maGoiCuoc(""),
      ngayDangKy(Date()),
      ngayHetHan(Date()),
      loaiHopDong(TYPE_PREPAID),
      trangThai(STATUS_ACTIVE),
      giaTriGoi(0.0) {}

HopDong::HopDong(const std::string& maHD,
                 const std::string& maKH,
                 const std::string& sdt,
                 const std::string& maGC,
                 const Date& ngayDK,
                 const Date& ngayHH,
                 const std::string& loaiHD,
                 const std::string& tThai,
                 double gia)
    : Entity(maHD),
      maKhachHang(maKH),
      soDienThoai(sdt),
      maGoiCuoc(maGC),
      ngayDangKy(ngayDK),
      ngayHetHan(ngayHH),
      loaiHopDong(loaiHD),
      trangThai(tThai),
      giaTriGoi(gia) {
    if (ngayHetHan < ngayDangKy) {
        throw InvalidDateException("Ngay het han khong duoc nho hon ngay dang ky!");
    }
    if (!soDienThoai.empty() && !Normalized::isValidPhoneNumber(soDienThoai)) {
        throw InvalidPhoneNumberException(soDienThoai);
    }
    if (giaTriGoi < 0.0) {
        throw AppException("Loi: Gia tri goi cuoc khong the am!");
    }
    if (!loaiHopDong.empty() && loaiHopDong != TYPE_PREPAID && loaiHopDong != TYPE_POSTPAID) {
        throw AppException("Loi: Loai hop dong phai la 'TraTruoc' hoac 'TraSau'!");
    }
    if (!trangThai.empty() && trangThai != STATUS_ACTIVE && trangThai != STATUS_SUSPENDED && trangThai != STATUS_TERMINATED) {
        throw AppException("Loi: Trang thai hop dong khong hop le!");
    }
}

void HopDong::setSoDienThoai(const std::string& sdt) {
    if (!sdt.empty() && !Normalized::isValidPhoneNumber(sdt)) {
        throw InvalidPhoneNumberException(sdt);
    }
    soDienThoai = sdt;
}

void HopDong::setNgayHetHan(const Date& d) {
    if (d < ngayDangKy) {
        throw InvalidDateException("Ngay het han khong duoc nho hon ngay dang ky!");
    }
    ngayHetHan = d;
}

void HopDong::setLoaiHopDong(const std::string& loai) {
    if (loai != TYPE_PREPAID && loai != TYPE_POSTPAID) {
        throw AppException("Loi: Loai hop dong phai la 'TraTruoc' hoac 'TraSau'!");
    }
    loaiHopDong = loai;
}

void HopDong::setTrangThai(const std::string& tThai) {
    if (tThai != STATUS_ACTIVE && tThai != STATUS_SUSPENDED && tThai != STATUS_TERMINATED) {
        throw AppException("Loi: Trang thai hop dong khong hop le!");
    }
    trangThai = tThai;
}

void HopDong::setGiaTriGoi(double gia) {
    if (gia < 0.0) {
        throw AppException("Loi: Gia tri goi cuoc khong the am!");
    }
    giaTriGoi = gia;
}

bool HopDong::isExpired(const Date& currentDate) const {
    return currentDate > ngayHetHan;
}

void HopDong::giaHan(const Date& ngayHetHanMoi) {
    if (ngayHetHanMoi <= ngayHetHan) {
        throw InvalidDateException("Ngay gia han moi phai lon hon ngay het han hien tai!");
    }
    ngayHetHan = ngayHetHanMoi;
    trangThai = STATUS_ACTIVE;
}

void HopDong::chamDut() {
    trangThai = STATUS_TERMINATED;
}

void HopDong::tamDung() {
    trangThai = STATUS_SUSPENDED;
}

void HopDong::kichHoatLai() {
    trangThai = STATUS_ACTIVE;
}

void HopDong::displayHeader() const {
    DisplayHelper::printHeader(
        {"Ma HD", "Ma KH", "So Dien Thoai", "Ma Goi", "Ngay DK", "Ngay HH", "Loai HD", "Trang Thai", "Gia Cuoc (VND)"},
        {COL_WIDTH_ID, COL_WIDTH_CUST, COL_WIDTH_PHONE, COL_WIDTH_PACKAGE, COL_WIDTH_DATE, COL_WIDTH_DATE, COL_WIDTH_TYPE, COL_WIDTH_STATUS, COL_WIDTH_PRICE},
        {false, false, false, false, false, false, false, false, true}
    );
}

void HopDong::displayRow() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << giaTriGoi;
    DisplayHelper::printRow(
        {id, maKhachHang, soDienThoai, maGoiCuoc, ngayDangKy.toString(), ngayHetHan.toString(), loaiHopDong, trangThai, oss.str()},
        {COL_WIDTH_ID, COL_WIDTH_CUST, COL_WIDTH_PHONE, COL_WIDTH_PACKAGE, COL_WIDTH_DATE, COL_WIDTH_DATE, COL_WIDTH_TYPE, COL_WIDTH_STATUS, COL_WIDTH_PRICE},
        {false, false, false, false, false, false, false, false, true}
    );
}

void HopDong::displayDetail() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << giaTriGoi << " VND";
    DisplayHelper::printCard({
        {"Ma hop dong", id},
        {"Ma khach hang", maKhachHang},
        {"So dien thoai", soDienThoai},
        {"Ma goi cuoc", maGoiCuoc},
        {"Ngay dang ky", ngayDangKy.toString()},
        {"Ngay het han", ngayHetHan.toString()},
        {"Loai hop dong", loaiHopDong},
        {"Trang thai", trangThai},
        {"Gia tri goi", oss.str()}
    });
}

std::string HopDong::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << maKhachHang << "|"
        << soDienThoai << "|"
        << maGoiCuoc << "|"
        << ngayDangKy.toString() << "|"
        << ngayHetHan.toString() << "|"
        << loaiHopDong << "|"
        << trangThai << "|"
        << std::fixed << std::setprecision(0) << giaTriGoi;
    return oss.str();
}

bool HopDong::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    const size_t EXPECTED_FIELD_COUNT = 9;
    if (tokens.size() < EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        id = tokens[0];
        maKhachHang = tokens[1];
        soDienThoai = tokens[2];
        maGoiCuoc = tokens[3];
        ngayDangKy = Date::parse(tokens[4]);
        ngayHetHan = Date::parse(tokens[5]);
        loaiHopDong = tokens[6];
        trangThai = tokens[7];
        giaTriGoi = std::stod(tokens[8]);
        return true;
    } catch (...) {
        return false;
    }
}
