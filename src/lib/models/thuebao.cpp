/**
 * @file thuebao.cpp
 * Thanh vien phu trach: Tùng Dương (B24DCVT106), Trương Khánh (B24DCVT201)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#include "thuebao.h"
#include "Date.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace ThueBaoConstants;

//tao thue bao mac dinh
ThueBao::ThueBao()
    : Entity(""),
      loaiThueBao(TYPE_PREPAID),
      maKhachHang(""),
      maGoiCuoc(""),
      ngayKichHoat(""),
      trangThai(STATUS_ACTIVE),
      soDuTaiKhoan(0.0) {}

//tao thue bao va kiem tra thong tin
ThueBao::ThueBao(
    const std::string& sdt,     //vd 0866776934
    const std::string& loaiTB,  // tra truoc tra sau
    const std::string& maKH,    
    const std::string& maGC,    //vd: SODA155
    const std::string& ngayKH,  //cd: 19/10/2026
    const std::string& tThai,   //da thanh toan/chua thanh toan
    double soDu                 //muny
) : Entity(sdt),
    loaiThueBao(loaiTB),
    maKhachHang(maKH),
    maGoiCuoc(maGC),
    ngayKichHoat(ngayKH),
    trangThai(tThai),
    soDuTaiKhoan(soDu) {
    if (!Normalized::isValidPhoneNumber(id)) {
        throw InvalidPhoneNumberException(id);
    }
    if (maKhachHang.empty() || ngayKichHoat.empty()) {
        throw AppException("Loi: Thong tin bat buoc cua thue bao khong hop le!");
    }
    Date::parse(ngayKichHoat);
    if (loaiThueBao != TYPE_PREPAID && loaiThueBao != TYPE_POSTPAID) {
        throw AppException("Loi: Loai thue bao phai la 'TraTruoc' hoac 'TraSau'!");
    }
    if (trangThai != STATUS_ACTIVE && trangThai != STATUS_LOCKED_1WAY &&
        trangThai != STATUS_LOCKED_2WAY && trangThai != STATUS_LOCKED_DEBT &&
        trangThai != STATUS_CANCELLED) {
        throw AppException("Loi: Trang thai thue bao khong hop le!");
    }
    if (soDuTaiKhoan < 0.0 && loaiThueBao == TYPE_PREPAID) {
        throw AppException("Loi: So du tai khoan tra truoc khong the am!");
    }
}

//cap nhat loai thue bao
void ThueBao::setLoaiThueBao(const std::string& loai) {
    if (loai != TYPE_PREPAID && loai != TYPE_POSTPAID) {
        throw AppException("Loi: Loai thue bao phai la 'TraTruoc' hoac 'TraSau'!");
    }
    loaiThueBao = loai;
}

//cap nhat ma khach hang so huu
void ThueBao::setMaKhachHang(const std::string& maKH) {
    maKhachHang = maKH;
}

//cap nhat ma goi cuoc
void ThueBao::setMaGoiCuoc(const std::string& maGC) {
    maGoiCuoc = maGC;
}

//cap nhat ngay kich hoat
void ThueBao::setNgayKichHoat(const std::string& ngay) {
    ngayKichHoat = ngay;
}

//cap nhat trang thai thue bao
void ThueBao::setTrangThai(const std::string& tThai) {
    trangThai = tThai;
}

//cap nhat so du tai khoan
void ThueBao::setSoDuTaiKhoan(double soDu) {
    if (soDu < 0.0 && loaiThueBao == TYPE_PREPAID) {
        throw AppException("Loi: So du tai khoan tra truoc khong the am!");
    }
    soDuTaiKhoan = soDu;
}

//cong tien vao tai khoan thue bao
void ThueBao::congTien(double tien) {
    if (tien <= 0) {
        throw AppException("Loi: So tien nap phai lon hon 0!");
    }
    soDuTaiKhoan += tien;
}

//tru tien khoi tai khoan thue bao
void ThueBao::truTien(double tien) {
    if (tien <= 0) {
        throw AppException("Loi: So tien tru phai lon hon 0!");
    }
    //doi voi thue bao tra truoc
    if (soDuTaiKhoan < tien && loaiThueBao == TYPE_PREPAID) {
        throw AppException("Loi: So du tai khoan khong du de thuc hien tru tien!");
    }
    soDuTaiKhoan -= tien;
}

//in tieu de danh sach thue bao
void ThueBao::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"So Dien Thoai", "Loai TB", "Ma KH", "Ma Goi", "Ngay KH", "Trang Thai", "So Du (VND)"},
        {COL_WIDTH_PHONE, COL_WIDTH_TYPE, COL_WIDTH_CUST, COL_WIDTH_PACKAGE, COL_WIDTH_DATE, COL_WIDTH_STATUS, COL_WIDTH_BALANCE},
        {false, false, false, false, false, false, true}, os
    );
}

//in mot dong thue bao
void ThueBao::displayRow(std::ostream& os) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << soDuTaiKhoan;
    DisplayHelper::printRow(
        {id, loaiThueBao, 
        maKhachHang, maGoiCuoc, 
        ngayKichHoat, trangThai, 
        oss.str()},
        
        {COL_WIDTH_PHONE, COL_WIDTH_TYPE, 
        COL_WIDTH_CUST, COL_WIDTH_PACKAGE, 
        COL_WIDTH_DATE, COL_WIDTH_STATUS, 
        COL_WIDTH_BALANCE},
        
        {false, false, false, false, false, false, true}, os
    );
}

//in chi tiet thue bao
void ThueBao::displayDetail(std::ostream& os) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << soDuTaiKhoan << " VND";
    DisplayHelper::printCard({
        {"So dien thoai", id},
        {"Loai thue bao", loaiThueBao},
        {"Ma chu so huu", maKhachHang},
        {"Goi cuoc dang dung", maGoiCuoc.empty() ? "(Chua dang ky)" : maGoiCuoc},
        {"Ngay kich hoat", ngayKichHoat},
        {"Trang thai", trangThai},
        {"So du tai khoan", oss.str()}
    }, 16, os);
}

//chuyen thue bao thanh dong tep
std::string ThueBao::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << loaiThueBao << "|"
        << maKhachHang << "|"
        << maGoiCuoc << "|"
        << ngayKichHoat << "|"
        << trangThai << "|"
        << std::fixed << std::setprecision(0) << soDuTaiKhoan;
    return oss.str();
}

//nap thue bao tu dong tep
bool ThueBao::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    if (tokens.size() != 7) {
        return false;
    }

    try {
        //phan tich day du so du truoc khi constructor kiem tra thue bao.
        //doc va kiem tra dinh dang thue bao,chong du thua
        size_t idxSoDu = 0;
        double soDu = std::stod(tokens[6], &idxSoDu);
        if (idxSoDu != tokens[6].length()) return false;
        ThueBao parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tokens[3],
            tokens[4],
            tokens[5],
            soDu
        );
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
