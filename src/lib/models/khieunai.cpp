#include "khieunai.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>

using namespace KhieuNaiConstants;

//tao phieu khieu nai mac dinh
KhieuNai::KhieuNai()
    : Entity(""),
      soDienThoai(""),
      loaiKhieuNai(TYPE_BILLING),
      ngayTiepNhan(""),
      noiDung(""),
      trangThai(STATUS_RECEIVED) {}

//tao phieu khieu nai va kiem tra thong tin
KhieuNai::KhieuNai(
    const std::string& maKN,
    const std::string& sdt,
    const std::string& loai,
    const std::string& ngay,
    const std::string& content,
    const std::string& status
) : Entity(maKN),
    soDienThoai(sdt),
    loaiKhieuNai(loai),
    ngayTiepNhan(ngay),
    noiDung(content),
    trangThai(status) {
    if (id.empty() || soDienThoai.empty() || loaiKhieuNai.empty() ||
        ngayTiepNhan.empty() || noiDung.empty() || trangThai.empty()) {
        throw AppException("Loi: Thong tin phieu khieu nai khong duoc de trong!");
    }
    if (!Normalized::isValidPhoneNumber(soDienThoai)) {
        throw InvalidPhoneNumberException(soDienThoai);
    }
}

//cap nhat trang thai phieu khieu nai
void KhieuNai::setTrangThai(const std::string& status) {
    if (status.empty()) {
        throw AppException("Loi: Trang thai khieu nai khong duoc de trong!");
    }
    trangThai = status;
}

//in tieu de danh sach khieu nai
void KhieuNai::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma KN", "So Dien Thoai", "Loai Khieu Nai", "Ngay Gui", "Trang Thai", "Noi Dung"},
        {COL_WIDTH_ID, COL_WIDTH_PHONE, COL_WIDTH_TYPE, COL_WIDTH_DATE,
         COL_WIDTH_STATUS, COL_WIDTH_CONTENT},
        {false, false, false, false, false, false}, os
    );
}

//in mot dong khieu nai
void KhieuNai::displayRow(std::ostream& os) const {
    DisplayHelper::printRow(
        {id, soDienThoai, loaiKhieuNai, ngayTiepNhan, trangThai, noiDung},
        {COL_WIDTH_ID, COL_WIDTH_PHONE, COL_WIDTH_TYPE, COL_WIDTH_DATE,
         COL_WIDTH_STATUS, COL_WIDTH_CONTENT},
        {false, false, false, false, false, false}, os
    );
}

//in chi tiet khieu nai
void KhieuNai::displayDetail(std::ostream& os) const {
    DisplayHelper::printCard({
        {"Ma phieu khiieu nai", id},
        {"So dien thoai phan anh", soDienThoai},
        {"Loai khiieu nai", loaiKhieuNai},
        {"Ngay tiep nhan", ngayTiepNhan},
        {"Trang thai xu ly", trangThai},
        {"Noi dung phan anh", noiDung}
    }, 16, os);
}

//chuyen khieu nai thanh dong tep
std::string KhieuNai::toFileString() const {
    return id + "|" + soDienThoai + "|" + loaiKhieuNai + "|" +
           ngayTiepNhan + "|" + noiDung + "|" + trangThai;
}

//nap khieu nai tu dong tep
bool KhieuNai::fromFileString(const std::string& line) {
    auto tokens = FileIO::split(line, '|');
    if (tokens.size() < 6) return false;

    id = tokens[0];
    soDienThoai = tokens[1];
    loaiKhieuNai = tokens[2];
    ngayTiepNhan = tokens[3];
    noiDung = tokens[4];
    trangThai = tokens[5];

    return !id.empty() && !soDienThoai.empty();
}
