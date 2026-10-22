#ifndef NORMALIZED_H
#define NORMALIZED_H

#include <string>

namespace Normalized {
    //cat khoang trang dau cuoi
    std::string trim(const std::string& str);
    //gom nhieu khoang trang thanh mot
    std::string CollapseSpace(const std::string& str);
    //loai bo ky tu dac biet
    std::string removeSymbols(const std::string& str);
    //loai bo chu so
    std::string removeDigits(const std::string& str);

    //chuyen chuoi thanh chu thuong
    std::string toLower(const std::string& str);
    //chuyen chuoi thanh chu hoa
    std::string toUpper(const std::string& str);

    std::string NormalizedName(const std::string& str);
    std::string NormalizedId(const std::string& str);
    std::string NormalizedCustomerId(const std::string& str);
    std::string NormalizedProvinceCode(const std::string& str);
    bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned = false);
    bool isValidCustomerId(const std::string& id);
    bool isValidCredential(const std::string& cccd);
    bool isValidLuhn(const std::string& imeiStr);
}

#endif // NORMALIZED_H
