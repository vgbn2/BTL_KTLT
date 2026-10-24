#include "Normalized.h"
#include <sstream>
#include <cctype>

namespace Normalized {

    
    //cat khoang trang dau cuoi chuoi
    std::string trim(const std::string& str) {
        //cat khoang trang o bien dong nhap/tep;CR/LF cung loai bo ky tu ket dong Windows.
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }
    
    //gom cac khoang trang lien tiep
    std::string CollapseSpace(const std::string& str) {
        std::string trimmed = trim(str);
    std::string result;
    bool inSpace = false;
    for (char c : trimmed) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!inSpace) { result += ' '; inSpace = true; }
        } else {
            result += c;
            inSpace = false;
        }
    }
    return result;
}

//thay ky tu cam bang khoang trang
std::string removeSymbols(const std::string& str) {
    const std::string FORBIDDEN = "|{}><~`#@!%^&*():;";
    std::string result = str;
    for (char& c : result)
    if (FORBIDDEN.find(c) != std::string::npos) c = ' ';
    return CollapseSpace(result);
}

//loai bo chu so khoi chuoi
std::string removeDigits(const std::string& str) {
    const std::string DIGITS = "0123456789";
    std::string result = str;
    for (char& c : result)
    if (DIGITS.find(c) != std::string::npos) c = ' ';
    return CollapseSpace(result);
}

//chuyen chuoi thanh chu thuong
std::string toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
return result;
}

//chuyen chuoi thanh chu hoa
std::string toUpper(const std::string& str) {
    std::string result = str;
    for (char& c : result)
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
return result;
}

//chuan hoa ho ten theo tung tu
std::string NormalizedName(const std::string& rawName) {
    //luong chuoi cuc bo tach tu ma khong doc hoac thay doi std::cin.
    std::istringstream iss(removeSymbols(rawName));
    std::string word, result;
    while (iss >> word) {
        word = toLower(word);
        word[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(word[0])));
        if (!result.empty()) result += ' ';
        result += word;
    }
    return result;
}

//chuan hoa ma dinh danh
std::string NormalizedId(const std::string& rawId) {
    std::string cleaned = toUpper(trim(rawId));
    std::string digits;
    for (char c : cleaned)
    if (std::isdigit(static_cast<unsigned char>(c))) digits += c;
    return digits.empty() ? cleaned : digits;
}

//chuan hoa ma khach hang giu nguyen tien to KH
std::string NormalizedCustomerId(const std::string& rawId) {
    return toUpper(trim(rawId));
}

//tao ma viet tat tu ten tinh
std::string NormalizedProvinceCode(const std::string& tinhLe) {
    std::string code;
    bool startOfWord = true;
    char last = 0;
    for (unsigned char c : tinhLe) {
        if (c == ' ') { startOfWord = true; continue; }
        if (startOfWord) code += static_cast<char>(std::toupper(c));
        startOfWord = false;
        last = static_cast<char>(std::toupper(c));
    }
    if (last) code += last;
    return code;
}

//kiem tra so dien thoai di dong viet nam
bool isValidPhoneNumber(const std::string& phone, bool allowUnassigned) {
    if (allowUnassigned && (phone.empty() || phone == "ChuaGan")) return true;
    if (phone.size() != 10 || phone[0] != '0') return false;
    if (std::string("35789").find(phone[1]) == std::string::npos) return false;
    for (char c : phone)
    if (c < '0' || c > '9') return false;
    return true;
}

//kiem tra ma khach hang theo tien to KH va day so
bool isValidCustomerId(const std::string& id) {
    if (id.size() != 6 && id.size() != 9) return false;
    if (id[0] != 'K' || id[1] != 'H') return false;
    for (size_t i = 2; i < id.size(); ++i)
    if (id[i] < '0' || id[i] > '9') return false;
    return true;
}

//kiem tra so can cuoc cong dan viet nam
bool isValidCredential(const std::string& cccd) {
    static const std::string PROVINCE_CODES =
    " 001 002 004 006 008 010 011 012 014 015 017 019 020 022 024 025 026 027 030 031 "
    "033 034 035 036 037 038 040 042 044 045 046 048 049 051 052 054 056 058 060 062 064 066 "
    "067 068 070 072 074 075 077 079 080 082 083 084 086 087 089 091 092 093 094 095 "
    "096 ";
    
    if (cccd.size() != 12) return false;
    for (char c : cccd)
    if (c < '0' || c > '9') return false;
    if (PROVINCE_CODES.find(" " + cccd.substr(0, 3) + " ") == std::string::npos) return false;
    return cccd[3] <= '5';
}
//kiem tra ma IMEI theo thuat toan luhn
//vi du 010101010101010 khong hop le vi:
//allZeros=true
//no chars
//nhung ma  sum=0+2+0+2+0+2+0+2+0+2+0+2+0+2=14->14%10 va 14%10!=0
bool isValidLuhn(const std::string& imeiStr) {
    if (imeiStr.length() != 15) return false;
    
    bool allZeros = true;
    for (char c : imeiStr) {
        if (c < '0' || c > '9') return false;
        if (c != '0') allZeros = false;
    }
    if (allZeros) return false;
    
    int sum = 0;
    for (int i = 14; i >= 0; --i) {
        int d = imeiStr[i] - '0';
        int posFromRight = 15 - i;
        if (posFromRight % 2 == 0) {
            d *= 2;
            if (d > 9) d -= 9;
        }
        sum += d;
    }
    return (sum % 10 == 0);
}

bool isValidName(const std::string& str) {
    if (trim(str).empty() || str.find('|') != std::string::npos) return false;
    size_t codePoints = 0;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        size_t width = 1;
        //UTF-8 dau 2/3/4 byte nam trong cac khoang byte nay;0x80 la dau byte tiep tuc.
        if (c >= 0xC2 && c <= 0xDF) width = 2;
        else if (c >= 0xE0 && c <= 0xEF) width = 3;
        else if (c >= 0xF0 && c <= 0xF4) width = 4;
        else if (c >= 0x80) return false;
        if (width > str.size() - i) return false;
        for (size_t j = 1; j < width; ++j) {
            unsigned char continuation = static_cast<unsigned char>(str[i + j]);
            //0xC0 la mat bit dung de nhan dien byte tiep tuc: 10xxxxxx.
            if ((continuation & 0xC0) != 0x80) return false;
        }
        //Chan ma UTF-8 vuot gioi han Unicode;0xA0/0x90 tranh overlong va 0x9F/0x8F tranh surrogate/out-of-range.
        if (width == 3 && ((c == 0xE0 && static_cast<unsigned char>(str[i + 1]) < 0xA0) ||
                           (c == 0xED && static_cast<unsigned char>(str[i + 1]) > 0x9F))) return false;
        if (width == 4 && ((c == 0xF0 && static_cast<unsigned char>(str[i + 1]) < 0x90) ||
                           (c == 0xF4 && static_cast<unsigned char>(str[i + 1]) > 0x8F))) return false;
        i += width;
        if (++codePoints > MAX_NAME_LENGTH) return false;
    }
    return true;
}
} // namespace Normalized
