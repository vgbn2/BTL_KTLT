#include "hopdong.h"
#include "Exceptions.h"
#include "fileio.h"
#include "InputHelper.h"
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
    if (!soDienThoai.empty() && !InputHelper::isValidPhoneNumber(soDienThoai)) {
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
    if (!sdt.empty() && !InputHelper::isValidPhoneNumber(sdt)) {
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
    std::cout << "+"
              << std::string(COL_WIDTH_ID, '-') << "+"
              << std::string(COL_WIDTH_CUST, '-') << "+"
              << std::string(COL_WIDTH_PHONE, '-') << "+"
              << std::string(COL_WIDTH_PACKAGE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_TYPE, '-') << "+"
              << std::string(COL_WIDTH_STATUS, '-') << "+"
              << std::string(COL_WIDTH_PRICE, '-') << "+\n";

    std::cout << "|"
              << std::left << std::setw(COL_WIDTH_ID) << " Ma HD" << "|"
              << std::left << std::setw(COL_WIDTH_CUST) << " Ma KH" << "|"
              << std::left << std::setw(COL_WIDTH_PHONE) << " So Dien Thoai" << "|"
              << std::left << std::setw(COL_WIDTH_PACKAGE) << " Ma Goi" << "|"
              << std::left << std::setw(COL_WIDTH_DATE) << " Ngay DK" << "|"
              << std::left << std::setw(COL_WIDTH_DATE) << " Ngay HH" << "|"
              << std::left << std::setw(COL_WIDTH_TYPE) << " Loai HD" << "|"
              << std::left << std::setw(COL_WIDTH_STATUS) << " Trang Thai" << "|"
              << std::right << std::setw(COL_WIDTH_PRICE) << "Gia Cuoc (VND) " << "|\n";

    std::cout << "+"
              << std::string(COL_WIDTH_ID, '-') << "+"
              << std::string(COL_WIDTH_CUST, '-') << "+"
              << std::string(COL_WIDTH_PHONE, '-') << "+"
              << std::string(COL_WIDTH_PACKAGE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_TYPE, '-') << "+"
              << std::string(COL_WIDTH_STATUS, '-') << "+"
              << std::string(COL_WIDTH_PRICE, '-') << "+\n";
}

void HopDong::displayRow() const {
    std::cout << "|"
              << " " << std::left << std::setw(COL_WIDTH_ID - 1) << id << "|"
              << " " << std::left << std::setw(COL_WIDTH_CUST - 1) << maKhachHang << "|"
              << " " << std::left << std::setw(COL_WIDTH_PHONE - 1) << soDienThoai << "|"
              << " " << std::left << std::setw(COL_WIDTH_PACKAGE - 1) << maGoiCuoc << "|"
              << " " << std::left << std::setw(COL_WIDTH_DATE - 1) << ngayDangKy.toString() << "|"
              << " " << std::left << std::setw(COL_WIDTH_DATE - 1) << ngayHetHan.toString() << "|"
              << " " << std::left << std::setw(COL_WIDTH_TYPE - 1) << loaiHopDong << "|"
              << " " << std::left << std::setw(COL_WIDTH_STATUS - 1) << trangThai << "|"
              << std::right << std::setw(COL_WIDTH_PRICE - 1) << std::fixed << std::setprecision(0) << giaTriGoi << " |\n";
}

void HopDong::displayDetail() const {
    std::cout << "  --------------------------------------------------\n"
              << "  Ma hop dong    : " << id << "\n"
              << "  Ma khach hang  : " << maKhachHang << "\n"
              << "  So dien thoai  : " << soDienThoai << "\n"
              << "  Ma goi cuoc    : " << maGoiCuoc << "\n"
              << "  Ngay dang ky   : " << ngayDangKy.toString() << "\n"
              << "  Ngay het han   : " << ngayHetHan.toString() << "\n"
              << "  Loai hop dong  : " << loaiHopDong << "\n"
              << "  Trang thai     : " << trangThai << "\n"
              << "  Gia tri goi    : " << std::fixed << std::setprecision(0) << giaTriGoi << " VND\n"
              << "  --------------------------------------------------\n";
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
