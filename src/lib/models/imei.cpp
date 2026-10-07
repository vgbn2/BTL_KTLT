#include "imei.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace IMEIConstants;

ThietBiIMEI::ThietBiIMEI()
    : Entity(""),
      tenThietBi(""),
      hangSanXuat(""),
      soDienThoai(UNASSIGNED_PHONE),
      ngayKichHoat(Date()),
      trangThai(STATUS_ACTIVE),
      tramBTSGanNhat("") {}

ThietBiIMEI::ThietBiIMEI(const std::string& imei,
                         const std::string& tenTB,
                         const std::string& hangSX,
                         const std::string& sdt,
                         const Date& ngayKH,
                         const std::string& tThai,
                         const std::string& bts)
    : Entity(imei),
      tenThietBi(tenTB),
      hangSanXuat(hangSX),
      soDienThoai(sdt),
      ngayKichHoat(ngayKH),
      trangThai(tThai),
      tramBTSGanNhat(bts) {
    if (!validateLuhn(imei)) {
        throw InvalidLuhnException(imei);
    }
    if (!soDienThoai.empty() && soDienThoai != UNASSIGNED_PHONE && !Normalized::isValidPhoneNumber(soDienThoai, true)) {
        throw InvalidPhoneNumberException(soDienThoai);
    }
    if (!trangThai.empty() && trangThai != STATUS_ACTIVE && trangThai != STATUS_LOCKED && trangThai != STATUS_SUSPENDED) {
        throw AppException("Loi: Trang thai thiet bi khong hop le!");
    }
}

bool ThietBiIMEI::validateLuhn(const std::string& imeiStr) {
    if (imeiStr.length() != REQUIRED_IMEI_LENGTH) {
        return false;
    }

    bool allZeros = true;
    for (char c : imeiStr) {
        if (c < '0' || c > '9') {
            return false;
        }
        if (c != '0') {
            allZeros = false;
        }
    }
    if (allZeros) {
        return false;
    }

    int sum = 0;
    for (int i = static_cast<int>(REQUIRED_IMEI_LENGTH) - 1; i >= 0; --i) {
        int d = imeiStr[i] - '0';
        int posFromRight = static_cast<int>(REQUIRED_IMEI_LENGTH) - i;
        if (posFromRight % 2 == 0) {
            d *= LUHN_MULTIPLIER;
            if (d > 9) {
                d -= LUHN_DIGIT_OVERFLOW_SUBTRACT;
            }
        }
        sum += d;
    }
    return (sum % MODULO_BASE == 0);
}

void ThietBiIMEI::setMaIMEI(const std::string& imei) {
    if (!validateLuhn(imei)) {
        throw InvalidLuhnException(imei);
    }
    id = imei;
}

bool ThietBiIMEI::isBlacklisted() const {
    return trangThai == STATUS_LOCKED;
}

void ThietBiIMEI::setBlacklist(bool lock) {
    trangThai = lock ? STATUS_LOCKED : STATUS_ACTIVE;
}

void ThietBiIMEI::setTrangThai(const std::string& tThai) {
    if (tThai != STATUS_ACTIVE && tThai != STATUS_LOCKED && tThai != STATUS_SUSPENDED) {
        throw AppException("Loi: Trang thai thiet bi khong hop le!");
    }
    trangThai = tThai;
}

void ThietBiIMEI::ganSIM(const std::string& sdt) {
    if (!sdt.empty() && sdt != UNASSIGNED_PHONE && !Normalized::isValidPhoneNumber(sdt, true)) {
        throw InvalidPhoneNumberException(sdt);
    }
    soDienThoai = sdt.empty() ? UNASSIGNED_PHONE : sdt;
}

void ThietBiIMEI::goSIM() {
    soDienThoai = UNASSIGNED_PHONE;
}

void ThietBiIMEI::capNhatBTS(const std::string& bts) {
    tramBTSGanNhat = bts;
}

void ThietBiIMEI::displayHeader() const {
    DisplayHelper::printHeader(
        {"Ma IMEI", "Ten Thiet Bi", "Hang SX", "So Dien Thoai", "Ngay Kich Hoat", "Trang Thai", "Tram BTS Gan"},
        {COL_WIDTH_IMEI, COL_WIDTH_NAME, COL_WIDTH_BRAND, COL_WIDTH_PHONE, COL_WIDTH_DATE, COL_WIDTH_STATUS, COL_WIDTH_BTS}
    );
}

void ThietBiIMEI::displayRow() const {
    DisplayHelper::printRow(
        {id, tenThietBi, hangSanXuat, soDienThoai, ngayKichHoat.toString(), trangThai, tramBTSGanNhat},
        {COL_WIDTH_IMEI, COL_WIDTH_NAME, COL_WIDTH_BRAND, COL_WIDTH_PHONE, COL_WIDTH_DATE, COL_WIDTH_STATUS, COL_WIDTH_BTS}
    );
}

void ThietBiIMEI::displayDetail() const {
    DisplayHelper::printCard({
        {"Ma IMEI", id},
        {"Ten thiet bi", tenThietBi},
        {"Hang san xuat", hangSanXuat},
        {"So dien thoai", soDienThoai},
        {"Ngay kich hoat", ngayKichHoat.toString()},
        {"Trang thai", trangThai},
        {"Tram BTS gan", tramBTSGanNhat}
    });
}

std::string ThietBiIMEI::toFileString() const {
    std::ostringstream oss;
    oss << id << "|"
        << tenThietBi << "|"
        << hangSanXuat << "|"
        << soDienThoai << "|"
        << ngayKichHoat.toString() << "|"
        << trangThai << "|"
        << tramBTSGanNhat;
    return oss.str();
}

bool ThietBiIMEI::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    const size_t EXPECTED_FIELD_COUNT = 7;
    if (tokens.size() < EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        id = tokens[0];
        tenThietBi = tokens[1];
        hangSanXuat = tokens[2];
        soDienThoai = tokens[3];
        ngayKichHoat = Date::parse(tokens[4]);
        trangThai = tokens[5];
        tramBTSGanNhat = tokens[6];
        return true;
    } catch (...) {
        return false;
    }
}
