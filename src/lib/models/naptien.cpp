#include "naptien.h"
#include "Date.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace NapTienConstants;

//tao phieu nap tien mac dinh
NapTien::NapTien()
    : Entity(""),
      soDienThoai(""),
      ngayNap(""),
      soTien(0.0),
      hinhThucNap(METHOD_BANKING),
      trangThai(STATUS_SUCCESS) {}

//tao phieu nap tien va kiem tra thong tin
NapTien::NapTien(
    const std::string& maGD,
    const std::string& sdt,
    const std::string& ngay,
    double tien,
    const std::string& hinhThuc,
    const std::string& tThai
) : Entity(maGD),
    soDienThoai(sdt),
    ngayNap(ngay),
    soTien(tien),
    hinhThucNap(hinhThuc),
    trangThai(tThai) {
    if (id.empty() || soDienThoai.empty() || ngayNap.empty()) {
        throw AppException("Loi: Thong tin bat buoc cua phieu nap tien khong hop le!");
    }
    Date::parse(ngayNap);
    if (!Normalized::isValidPhoneNumber(sdt)) {
        throw InvalidPhoneNumberException(sdt);
    }
    if (soTien <= 0.0) {
        throw AppException("Loi: So tien nap phai lon hon 0!");
    }
    if (hinhThucNap != METHOD_SCRATCH_CARD && hinhThucNap != METHOD_BANKING && hinhThucNap != METHOD_E_WALLET) {
        throw AppException("Loi: Hinh thuc nap tien khong hop le!");
    }
    if (trangThai != STATUS_SUCCESS && trangThai != STATUS_FAILED) {
        throw AppException("Loi: Trang thai nap tien khong hop le!");
    }
}

//cap nhat trang thai phieu nap
void NapTien::setTrangThai(const std::string& tThai) {
    trangThai = tThai;
}

//in tieu de danh sach phieu nap
void NapTien::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma GD", "So Dien Thoai", "Ngay Nap", "So Tien (VND)", "Hinh Thuc", "Trang Thai"},
        {COL_WIDTH_ID, COL_WIDTH_PHONE, COL_WIDTH_DATE, COL_WIDTH_AMOUNT, COL_WIDTH_METHOD, COL_WIDTH_STATUS},
        {false, false, false, true, false, false}, os
    );
}

//in mot dong phieu nap
void NapTien::displayRow(std::ostream& os) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << soTien;

    DisplayHelper::printRow(
        {id, soDienThoai, ngayNap, oss.str(), hinhThucNap, trangThai},
        {COL_WIDTH_ID, COL_WIDTH_PHONE, COL_WIDTH_DATE, COL_WIDTH_AMOUNT, COL_WIDTH_METHOD, COL_WIDTH_STATUS},
        {false, false, false, true, false, false}, os
    );
}

//in chi tiet phieu nap
void NapTien::displayDetail(std::ostream& os) const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << soTien << " VND";

    DisplayHelper::printCard({
        {"Ma giao dich", id},
        {"So dien thoai", soDienThoai},
        {"Ngay nap tien", ngayNap},
        {"So tien nap", oss.str()},
        {"Hinh thuc nap", hinhThucNap},
        {"Trang thai", trangThai}
    }, 16, os);
}

//chuyen phieu nap thanh dong tep
std::string NapTien::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << soDienThoai << "|"
        << ngayNap << "|"
        << std::fixed << std::setprecision(0) << soTien << "|"
        << hinhThucNap << "|"
        << trangThai;
    return oss.str();
}

//nap phieu nap tu dong tep
bool NapTien::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    if (tokens.size() != 6) {
        return false;
    }

    try {
        //doc va kiem tra giao dich nap tien,chan du thua
        size_t idxTien = 0;
        double tien = std::stod(tokens[3], &idxTien);
        if (idxTien != tokens[3].length()) return false;
        NapTien parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tien,
            tokens[4],
            tokens[5]
        );
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
