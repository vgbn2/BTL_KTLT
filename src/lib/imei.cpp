#include "imei.h"
#include "Exceptions.h"
#include "fileio.h"
#include <iostream>
#include <iomanip>
#include <sstream>

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

void ThietBiIMEI::ganSIM(const std::string& sdt) {
    soDienThoai = sdt.empty() ? UNASSIGNED_PHONE : sdt;
}

void ThietBiIMEI::goSIM() {
    soDienThoai = UNASSIGNED_PHONE;
}

void ThietBiIMEI::capNhatBTS(const std::string& bts) {
    tramBTSGanNhat = bts;
}

void ThietBiIMEI::displayHeader() const {
    std::cout << "+"
              << std::string(COL_WIDTH_IMEI, '-') << "+"
              << std::string(COL_WIDTH_NAME, '-') << "+"
              << std::string(COL_WIDTH_BRAND, '-') << "+"
              << std::string(COL_WIDTH_PHONE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_STATUS, '-') << "+"
              << std::string(COL_WIDTH_BTS, '-') << "+\n";

    std::cout << "|"
              << std::left << std::setw(COL_WIDTH_IMEI) << " Ma IMEI" << "|"
              << std::left << std::setw(COL_WIDTH_NAME) << " Ten Thiet Bi" << "|"
              << std::left << std::setw(COL_WIDTH_BRAND) << " Hang SX" << "|"
              << std::left << std::setw(COL_WIDTH_PHONE) << " So Dien Thoai" << "|"
              << std::left << std::setw(COL_WIDTH_DATE) << " Ngay Kich Hoat" << "|"
              << std::left << std::setw(COL_WIDTH_STATUS) << " Trang Thai" << "|"
              << std::left << std::setw(COL_WIDTH_BTS) << " Tram BTS Gan" << "|\n";

    std::cout << "+"
              << std::string(COL_WIDTH_IMEI, '-') << "+"
              << std::string(COL_WIDTH_NAME, '-') << "+"
              << std::string(COL_WIDTH_BRAND, '-') << "+"
              << std::string(COL_WIDTH_PHONE, '-') << "+"
              << std::string(COL_WIDTH_DATE, '-') << "+"
              << std::string(COL_WIDTH_STATUS, '-') << "+"
              << std::string(COL_WIDTH_BTS, '-') << "+\n";
}

void ThietBiIMEI::displayRow() const {
    std::cout << "|"
              << " " << std::left << std::setw(COL_WIDTH_IMEI - 1) << id << "|"
              << " " << std::left << std::setw(COL_WIDTH_NAME - 1) << tenThietBi << "|"
              << " " << std::left << std::setw(COL_WIDTH_BRAND - 1) << hangSanXuat << "|"
              << " " << std::left << std::setw(COL_WIDTH_PHONE - 1) << soDienThoai << "|"
              << " " << std::left << std::setw(COL_WIDTH_DATE - 1) << ngayKichHoat.toString() << "|"
              << " " << std::left << std::setw(COL_WIDTH_STATUS - 1) << trangThai << "|"
              << " " << std::left << std::setw(COL_WIDTH_BTS - 1) << tramBTSGanNhat << "|\n";
}

void ThietBiIMEI::displayDetail() const {
    std::cout << "  --------------------------------------------------\n"
              << "  Ma IMEI        : " << id << "\n"
              << "  Ten thiet bi   : " << tenThietBi << "\n"
              << "  Hang san xuat  : " << hangSanXuat << "\n"
              << "  So dien thoai  : " << soDienThoai << "\n"
              << "  Ngay kich hoat : " << ngayKichHoat.toString() << "\n"
              << "  Trang thai     : " << trangThai << "\n"
              << "  Tram BTS gan   : " << tramBTSGanNhat << "\n"
              << "  --------------------------------------------------\n";
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
