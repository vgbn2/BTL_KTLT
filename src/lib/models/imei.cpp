/**
 * @file imei.cpp
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

/**
 *trien khai model thiet bi dau cuoi khai bao trong imei.h, gom kiem tra Luhn,
 *chuyen trang thai SIM/BTS, hien thi console va tuan tu hoa tep phang bay truong.
 */
#include "imei.h"
#include "Exceptions.h"
#include "fileio.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>
#include <vector>

using namespace IMEIConstants;

//tao thiet bi IMEI mac dinh
ThietBiIMEI::ThietBiIMEI()
    : Entity(""),
      tenThietBi(""),
      hangSanXuat(""),
      soDienThoai(UNASSIGNED_PHONE),
      ngayKichHoat(Date()),
      trangThai(STATUS_ACTIVE),
      tramBTSGanNhat("") {}

//tao thiet bi IMEI va kiem tra thong tin
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
    if (id.empty() || !Normalized::isValidName(tenThietBi) ||
        !Normalized::isValidName(hangSanXuat) || trangThai.empty() || tramBTSGanNhat.empty()) {
        throw AppException("Loi: Thong tin thiet bi IMEI khong duoc de trong cac truong bat buoc!");
    }
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

//kiem tra IMEI theo thuat toan luhn
bool ThietBiIMEI::validateLuhn(const std::string& imeiStr) {
    return Normalized::isValidLuhn(imeiStr);
}

//sinh chuoi IMEI 15 chu so mo phong hop le theo thuat toan Luhn 3GPP
//static_cast<char>(X) tuong duong (char)X trong C
std::string ThietBiIMEI::generateSimulatedImei() {
    std::random_device seed;
    std::mt19937 generator(seed());
    std::uniform_int_distribution<int> digit(0, 9);
    while (true) {
        std::string prefix;
        prefix.reserve(14);
        prefix.push_back(static_cast<char>('3' + digit(generator) % 7));
        for (int i = 1; i < 14; ++i) {
            prefix.push_back(static_cast<char>('0' + digit(generator)));
        }
        for (int checkDigit = 0; checkDigit <= 9; ++checkDigit) {
            std::string candidate = prefix + std::to_string(checkDigit);
            if (validateLuhn(candidate)) {
                return candidate;
            }
        }
    }
}

//cap nhat ma IMEI va kiem tra luhn
void ThietBiIMEI::setMaIMEI(const std::string& imei) {
    if (!validateLuhn(imei)) {
        throw InvalidLuhnException(imei);
    }
    id = imei;
}
//danh sach den
//kiem tra thiet bi co bi khoa mang
bool ThietBiIMEI::isBlacklisted() const {
    return trangThai == STATUS_LOCKED;
}
//tra ve status
//if(trangThai==locked)
//return STATUS_LOCKED
//else
//tra ve trang thai hoat dong
//dat trang thai khoa hoac mo khoa
void ThietBiIMEI::setBlacklist(bool lock) {
    trangThai = lock ? STATUS_LOCKED : STATUS_ACTIVE;
}

//cap nhat ten thiet bi va hang san xuat
void ThietBiIMEI::setTenThietBi(const std::string& ten) {
    if (!Normalized::isValidName(ten)) {
        throw AppException("Loi: Ten thiet bi khong duoc rong hoac qua 50 ky tu!");
    }
    tenThietBi = ten;
}

void ThietBiIMEI::setHangSanXuat(const std::string& hang) {
    if (!Normalized::isValidName(hang)) {
        throw AppException("Loi: Hang san xuat khong duoc rong hoac qua 50 ky tu!");
    }
    hangSanXuat = hang;
}

//cap nhat trang thai thiet bi
void ThietBiIMEI::setTrangThai(const std::string& tThai) {
    if (tThai != STATUS_ACTIVE && tThai != STATUS_LOCKED && tThai != STATUS_SUSPENDED) {
        throw AppException("Loi: Trang thai thiet bi khong hop le!");
    }
    trangThai = tThai;
}

//gan so dien thoai cho thiet bi
void ThietBiIMEI::ganSIM(const std::string& sdt) {
    if (!sdt.empty() && sdt != UNASSIGNED_PHONE && !Normalized::isValidPhoneNumber(sdt, true)) {
        throw InvalidPhoneNumberException(sdt);
    }
    soDienThoai = sdt.empty() ? UNASSIGNED_PHONE : sdt;
}

//go so dien thoai khoi thiet bi
void ThietBiIMEI::goSIM() {
    soDienThoai = UNASSIGNED_PHONE;
}

//cap nhat tram BTS gan nhat
void ThietBiIMEI::capNhatBTS(const std::string& bts) {
    tramBTSGanNhat = bts;
}
//hienthi
//in tieu de danh sach thiet bi
void ThietBiIMEI::displayHeader(std::ostream& os) const {
    DisplayHelper::printHeader(
        {"Ma IMEI", "Ten Thiet Bi", "Hang SX", "So Dien Thoai", "Ngay Kich Hoat", "Trang Thai", "Tram BTS Gan"},
        {COL_WIDTH_IMEI, COL_WIDTH_NAME, COL_WIDTH_BRAND, COL_WIDTH_PHONE, COL_WIDTH_DATE, COL_WIDTH_STATUS, COL_WIDTH_BTS},
        {}, os
    );
}

//in mot dong thiet bi
void ThietBiIMEI::displayRow(std::ostream& os) const {
    DisplayHelper::printRow(
        {id, 
        tenThietBi, hangSanXuat,
        soDienThoai, ngayKichHoat.toString(),
        trangThai, tramBTSGanNhat},
        {COL_WIDTH_IMEI, 
        COL_WIDTH_NAME, COL_WIDTH_BRAND,
        COL_WIDTH_PHONE, COL_WIDTH_DATE, 
        COL_WIDTH_STATUS, COL_WIDTH_BTS},
        {}, os
    );
}

//in chi tiet thiet bi
void ThietBiIMEI::displayDetail(std::ostream& os) const {
    DisplayHelper::printCard({
        {"Ma IMEI", id},
        {"Ten thiet bi", tenThietBi},
        {"Hang san xuat", hangSanXuat},
        {"So dien thoai", soDienThoai},
        {"Ngay kich hoat", ngayKichHoat.toString()},
        {"Trang thai", trangThai},
        {"Tram BTS gan", tramBTSGanNhat}
    }, 16, os);
}

//chuyen thiet bi thanh dong tep
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

//nap thiet bi tu dong tep
bool ThietBiIMEI::fromFileString(const std::string& line) {
    std::vector<std::string> tokens = FileIO::split(line, '|');
    const size_t EXPECTED_FIELD_COUNT = 7;
    if (tokens.size() != EXPECTED_FIELD_COUNT) {
        return false;
    }

    try {
        //kiem tra ngay,ma IMEI,sentinel so dien thoai va trang thai truoc khi gan.
        ThietBiIMEI parsed(
            tokens[0],
            tokens[1],
            tokens[2],
            tokens[3],
            Date::parse(tokens[4]),
            tokens[5],
            tokens[6]
        );
        *this = parsed;
        return true;
    } catch (...) {
        return false;
    }
}
