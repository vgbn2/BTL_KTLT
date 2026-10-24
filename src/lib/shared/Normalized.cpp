/**
 * @file Normalized.cpp
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#include "Normalized.h"
#include <sstream>
#include <cctype>

static int doubleLuhnDigit(int digit) {
    const int doubled = digit * 2;
    return (doubled > 9) ? (doubled - 9) : doubled;
}

//cat khoang trang dau cuoi chuoi
std::string Normalized::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

//gom cac khoang trang lien tiep
std::string Normalized::CollapseSpace(const std::string& str) {
    std::string trimmed = trim(str);
    std::string result;
    bool inSpace = false;
    for (char ch : trimmed) {
        const bool isWhitespace = (std::isspace(static_cast<unsigned char>(ch)) != 0);
        if (isWhitespace && inSpace) {
            continue;
        }
        if (isWhitespace) {
            result += ' ';
            inSpace = true;
            continue;
        }
        result += ch;
        inSpace = false;
    }
    return result;
}

//thay ky tu cam bang khoang trang
std::string Normalized::removeSymbols(const std::string& str) {
    const std::string FORBIDDEN = "|{}><~`#@!%^&*():;";
    std::string result = str;
    for (char& ch : result) {
        if (FORBIDDEN.find(ch) != std::string::npos) {
            ch = ' ';
        }
    }
    return CollapseSpace(result);
}

//loai bo chu so khoi chuoi
std::string Normalized::removeDigits(const std::string& str) {
    const std::string DIGITS = "0123456789";
    std::string result = str;
    for (char& ch : result) {
        if (DIGITS.find(ch) != std::string::npos) {
            ch = ' ';
        }
    }
    return CollapseSpace(result);
}

//chuyen chuoi thanh chu thuong
std::string Normalized::toLower(const std::string& str) {
    std::string result = str;
    for (char& character : result) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return result;
}

//chuyen chuoi thanh chu hoa
std::string Normalized::toUpper(const std::string& str) {
    std::string result = str;
    for (char& character : result) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}

//chuan hoa ho ten theo tung tu
std::string Normalized::NormalizedName(const std::string& rawName) {
    std::istringstream iss(removeSymbols(rawName));
    std::string word, result;
    while (iss >> word) {
        word = toLower(word);
        word[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(word[0])));
        if (!result.empty()) {
            result += ' ';
        }
        result += word;
    }
    return result;
}

//chuan hoa ma dinh danh
std::string Normalized::NormalizedId(const std::string& rawId) {
    std::string cleaned = toUpper(trim(rawId));
    std::string digits;
    for (char ch : cleaned) {
        if (std::isdigit(static_cast<unsigned char>(ch))) {
            digits += ch;
        }
    }
    return digits.empty() ? cleaned : digits;
}

//chuan hoa ma khach hang giu nguyen tien to KH
std::string Normalized::NormalizedCustomerId(const std::string& rawId) {
    return toUpper(trim(rawId));
}

//tao ma viet tat tu ten tinh
std::string Normalized::NormalizedProvinceCode(const std::string& tinhLe) {
    std::string code;
    bool startOfWord = true;
    char lastChar = 0;
    for (unsigned char ch : tinhLe) {
        if (ch == ' ') {
            startOfWord = true;
            continue;
        }
        if (startOfWord) {
            code += static_cast<char>(std::toupper(ch));
        }
        startOfWord = false;
        lastChar = static_cast<char>(std::toupper(ch));
    }
    if (lastChar != 0) {
        code += lastChar;
    }
    return code;
}

//kiem tra so dien thoai di dong viet nam
bool Normalized::isValidPhoneNumber(const std::string& phone, bool allowUnassigned) {
    if (allowUnassigned && (phone.empty() || phone == "ChuaGan")) {
        return true;
    }
    if (phone.size() != 10 || phone[0] != '0') {
        return false;
    }
    if (std::string("35789").find(phone[1]) == std::string::npos) {
        return false;
    }
    for (char ch : phone) {
        if (ch < '0' || ch > '9') {
            return false;
        }
    }
    return true;
}

//kiem tra ma khach hang theo tien to KH va day so
bool Normalized::isValidCustomerId(const std::string& id) {
    if (id.size() != 6 && id.size() != 9) {
        return false;
    }
    if (id[0] != 'K' || id[1] != 'H') {
        return false;
    }
    for (size_t i = 2; i < id.size(); ++i) {
        if (id[i] < '0' || id[i] > '9') {
            return false;
        }
    }
    return true;
}

//kiem tra so can cuoc cong dan viet nam
bool Normalized::isValidCredential(const std::string& cccd) {
    static const std::string PROVINCE_CODES =
        " 001 002 004 006 008 010 011 012 014 015 017 019 020 022 024 025 026 027 030 031 "
        "033 034 035 036 037 038 040 042 044 045 046 048 049 051 052 054 056 058 060 062 064 066 "
        "067 068 070 072 074 075 077 079 080 082 083 084 086 087 089 091 092 093 094 095 "
        "096 ";

    if (cccd.size() != 12) {
        return false;
    }
    for (char ch : cccd) {
        if (ch < '0' || ch > '9') {
            return false;
        }
    }
    if (PROVINCE_CODES.find(" " + cccd.substr(0, 3) + " ") == std::string::npos) {
        return false;
    }
    return cccd[3] <= '5';
}

//kiem tra ma IMEI theo thuat toan luhn
bool Normalized::isValidLuhn(const std::string& imeiStr) {
    if (imeiStr.length() != 15) {
        return false;
    }

    bool allZeros = true;
    for (char ch : imeiStr) {
        if (ch < '0' || ch > '9') {
            return false;
        }
        if (ch != '0') {
            allZeros = false;
        }
    }
    if (allZeros) {
        return false;
    }

    int sum = 0;
    for (int i = 14; i >= 0; --i) {
        const int digit = imeiStr[i] - '0';
        const bool shouldDouble = ((15 - i) % 2 == 0);
        sum += shouldDouble ? doubleLuhnDigit(digit) : digit;
    }
    return (sum % 10 == 0);
}

//kiem tra ten khong rong, khong pha schema va khong qua 50 ky tu UTF-8
bool Normalized::isValidName(const std::string& str) {
    if (trim(str).empty() || str.find('|') != std::string::npos) {
        return false;
    }
    size_t codePoints = 0;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        if (c < 32 || c == 127) {
            return false;
        }
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
        //tranh overlong va surrogate/out-of-range.
        if (width == 3 && ((c == 0xE0 && static_cast<unsigned char>(str[i + 1]) < 0xA0) ||
                           (c == 0xED && static_cast<unsigned char>(str[i + 1]) > 0x9F))) return false;
        if (width == 4 && ((c == 0xF0 && static_cast<unsigned char>(str[i + 1]) < 0x90) ||
                           (c == 0xF4 && static_cast<unsigned char>(str[i + 1]) > 0x8F))) return false;
        i += width;
        if (++codePoints > MAX_NAME_LENGTH) return false;
    }
    return true;
}
