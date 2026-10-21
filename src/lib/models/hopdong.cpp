#include "hopdong.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace HopDongConstants;

//tao hop dong mac dinh
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

//tao hop dong va kiem tra thong tin
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
    if (id.empty() || maKhachHang.empty() || soDienThoai.empty() || maGoiCuoc.empty() ||
        loaiHopDong.empty() || trangThai.empty()) {
        throw AppException("Loi: Cac truong bat buoc cua hop dong khong duoc de trong!");
    }
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

//cap nhat so dien thoai hop dong
void HopDong::setSoDienThoai(const std::string& sdt) {
    if (!sdt.empty() && !Normalized::isValidPhoneNumber(sdt)) {
        throw InvalidPhoneNumberException(sdt);
    }
    soDienThoai = sdt;
}

//cap nhat ngay het han
void HopDong::setNgayHetHan(const Date& d) {
    if (d < ngayDangKy) {
        throw InvalidDateException("Ngay het han khong duoc nho hon ngay dang ky!");
    }
    ngayHetHan = d;
}

//cap nhat loai hop dong
void HopDong::setLoaiHopDong(const std::string& loai) {
    if (loai != TYPE_PREPAID && loai != TYPE_POSTPAID) {
        throw AppException("Loi: Loai hop dong phai la 'TraTruoc' hoac 'TraSau'!");
    }
    loaiHopDong = loai;
}

//cap nhat trang thai hop dong
void HopDong::setTrangThai(const std::string& tThai) {
    if (tThai != STATUS_ACTIVE && tThai != STATUS_SUSPENDED && tThai != STATUS_TERMINATED) {
        throw AppException("Loi: Trang thai hop dong khong hop le!");
    }
    trangThai = tThai;
}

//cap nhat gia tri goi cuoc
void HopDong::setGiaTriGoi(double gia) {
    if (gia < 0.0) {
        throw AppException("Loi: Gia tri goi cuoc khong the am!");
    }
    giaTriGoi = gia;
}

//kiem tra hop dong da het han
bool HopDong::isExpired(const Date& currentDate) const {
    return currentDate > ngayHetHan;
}

//gia han hop dong va kich hoat lai
void HopDong::giaHan(const Date& ngayHetHanMoi) {
    if (ngayHetHanMoi <= ngayHetHan) {
        throw InvalidDateException("Ngay gia han moi phai lon hon ngay het han hien tai!");
    }
    ngayHetHan = ngayHetHanMoi;
    trangThai = STATUS_ACTIVE;
}

//cham dut hop dong
void HopDong::chamDut() {
    trangThai = STATUS_TERMINATED;
}

//tam dung hop dong
void HopDong::tamDung() {
    trangThai = STATUS_SUSPENDED;
}

//kich hoat lai hop dong
void HopDong::kichHoatLai() {
    trangThai = STATUS_ACTIVE;
}

//in tieu de danh sach hop dong
void HopDong::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma HD", "Ma KH", "So Dien Thoai", "Ma Goi", "Ngay DK", "Ngay HH", "Loai HD", "Trang Thai", "Gia Cuoc (VND)"},
        {COL_WIDTH_ID, COL_WIDTH_CUST, COL_WIDTH_PHONE, COL_WIDTH_PACKAGE, COL_WIDTH_DATE, COL_WIDTH_DATE, COL_WIDTH_TYPE, COL_WIDTH_STATUS, COL_WIDTH_PRICE},
        {false, false, false, false, false, false, false, false, true}, os
    );
}

//in mot dong hop dong
void HopDong::displayRow(std::ostream& os) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << giaTriGoi;
    DisplayHelper::printRow(
        {id, maKhachHang, soDienThoai, maGoiCuoc, ngayDangKy.toString(), ngayHetHan.toString(), loaiHopDong, trangThai, oss.str()},
        {COL_WIDTH_ID, COL_WIDTH_CUST, COL_WIDTH_PHONE, COL_WIDTH_PACKAGE, COL_WIDTH_DATE, COL_WIDTH_DATE, COL_WIDTH_TYPE, COL_WIDTH_STATUS, COL_WIDTH_PRICE},
        {false, false, false, false, false, false, false, false, true}, os
    );
}

//in chi tiet hop dong
void HopDong::displayDetail(std::ostream& os) const {
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
    }, 16, os);
}

//chuyen hop dong thanh dong tep
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

//nap hop dong tu dong tep
bool HopDong::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    const size_t EXPECTED_FIELD_COUNT = 9;
    if (tokens.size() != EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        //doc va kiem tra dinh dang hop dong,chong du thua
        size_t idxGia = 0;
        double gia = std::stod(tokens[8], &idxGia);
        if (idxGia != tokens[8].length()) return false;
        HopDong parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tokens[3],
            Date::parse(tokens[4]),
            Date::parse(tokens[5]),
            tokens[6],
            tokens[7],
            gia
        );
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
