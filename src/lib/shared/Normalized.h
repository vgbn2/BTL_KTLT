/**
 * @file Normalized.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef NORMALIZED_H
#define NORMALIZED_H

#include <string>

namespace Normalized {
    const size_t MAX_NAME_LENGTH = 50;

    //kiem tra ten hop le (khong rong, khong co '|', <= 50 ky tu ASCII in duoc)
    bool isValidName(const std::string& str);

    //kiem tra so dien thoai di dong VN 10 chu so (03x, 05x, 07x, 08x, 09x)
    bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false);

    //kiem tra ma khach hang (KHxxxx hoac KHxxxxxxx)
    bool isValidCustomerId(const std::string& id);

    //kiem tra CCCD 12 chu so theo quy chuan (ma tinh 001-096, the ky/gioi tinh <= 5)
    bool isValidCredential(const std::string& cccd);

    //kiem tra ma IMEI 15 chu so theo thuat toan Luhn Checksum (Mod-10)
    bool isValidLuhn(const std::string& imeiStr);

    //cat khoang trang dau va cuoi chuoi
    std::string trim(const std::string& str);

    //gom cac khoang trang lien tiep thanh mot khoang trang don
    std::string CollapseSpace(const std::string& str);

    //loai bo ky tu cam dac biet bang khoang trang
    std::string removeSymbols(const std::string& str);

    //loai bo chu so khoi chuoi
    std::string removeDigits(const std::string& str);

    //chuyen chuoi thanh chu thuong
    std::string toLower(const std::string& str);

    //chuyen chuoi thanh chu hoa
    std::string toUpper(const std::string& str);

    //chuan hoa ho ten: viet hoa chu cai dau moi tu, cac chu sau viet thuong
    std::string NormalizedName(const std::string& str);

    //chuan hoa ma dinh danh: trich chu so neu co, nguoc lai giu chu hoa da trim
    std::string NormalizedId(const std::string& str);

    //chuan hoa ma khach hang: trim va viet hoa
    std::string NormalizedCustomerId(const std::string& str);

    //tao ma viet tat tinh/thanh pho tu chu cai dau va chu cai cuoi
    std::string NormalizedProvinceCode(const std::string& str);
}

#endif // NORMALIZED_H
