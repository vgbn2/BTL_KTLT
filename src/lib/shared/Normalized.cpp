/**
 * @file Normalized.cpp
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#include "Normalized.h"
#include <sstream>
#include <cctype>

// nhan doi chu so trong thuat toan Luhn: digit * 2, tru 9 neu > 9 (vi du: 3 -> 6, 8 -> 7)
static int doubleLuhnDigit(int digit) {
    const int doubled = digit * 2;
    return (doubled > 9) ? (doubled - 9) : doubled;
}

// cat khoang trang o dau va cuoi chuoi
std::string Normalized::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

// gom cac khoang trang lien tiep thanh mot khoang trang don
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

// thay ky tu cam bang khoang trang roi gom lai
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

// loai bo chu so khoi chuoi
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

// chuyen chuoi thanh chu thuong
std::string Normalized::toLower(const std::string& str) {
    std::string result = str;
    for (char& character : result) {
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    }
    return result;
}

// chuyen chuoi thanh chu hoa
std::string Normalized::toUpper(const std::string& str) {
    std::string result = str;
    for (char& character : result) {
        character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
    }
    return result;
}

// chuan hoa ho va ten: loai bo ky tu cam, viet hoa chu cai dau moi tu, cac chu sau viet thuong
// vi du: "   nguyen   van   a  " -> "Nguyen Van A"
//        "TRAN#@#DUC$$$ANH" -> "Tran Duc Anh"
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

// chuan hoa ma dinh danh: trich xuat chu so neu co, nguoc lai giu nguyen chu hoa
// vi du: " KH-0012 " -> "0012", " BTS_HN " -> "BTS_HN"
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

// chuan hoa ma khach hang: trim va viet hoa (vi du: "  kh0001  " -> "KH0001")
std::string Normalized::NormalizedCustomerId(const std::string& rawId) {
    return toUpper(trim(rawId));
}

// tao ma viet tat tinh/thanh pho tu chu cai dau moi tu va chu cai cuoi cung
// vi du: "Ha Noi" -> cac chu cai dau: 'H', 'N'; chu cai cuoi: 'I' -> "HNI"
//        "Bac Ninh" -> chu cai dau: 'B', 'N'; chu cai cuoi: 'H' -> "BNH"
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

// kiem tra so di dong VN 10 chu so bat dau bang 0[3|5|7|8|9] (hoac ChuaGan neu allowUnassigned)
// vi du: "0912345678" -> hop le; "0241234567" (co dinh), "09123" (ngan) -> khong hop le
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

// kiem tra ma khach hang: bat dau bang "KH" theo sau boi 4 hoac 7 chu so (vi du: KH0001, KH0000001)
// vi du: "KH0001" -> hop le; "KH1" (ngan), "AB0001" (sai tien to) -> khong hop le
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

// kiem tra CCCD 12 chu so: 3 so tinh (001-096), the ky/gioi tinh (<=5), 2 so nam sinh, 6 so ngau nhien
// vi du: "001099123456" (Ha Noi, Nam, 1999) -> hop le; "999099123456" -> sai ma tinh
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

// kiem tra ma IMEI 15 chu so theo thuat toan Luhn (Mod-10) chuan 3GPP:
// nhan doi chu so o vi tri chan tu phai sang trai (tru 9 neu > 9), tong tat ca phai chia het cho 10
// vi du: "358912345678902" -> sum = 70, 70 % 10 == 0 -> hop le
//        "358912345678903" -> sum = 71, 71 % 10 != 0 -> khong hop le
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

// kiem tra ten: khong rong sau khi trim, khong chua '|', toi da 50 ky tu ASCII in duoc [32, 126]
// vi du: "Nguyen Van A" -> hop le; "Nguyen|Van A", "  " -> khong hop le
// ponytail: clean ASCII name validation with length limit; upgrade to ICU if localization is required
bool Normalized::isValidName(const std::string& str) {
    if (trim(str).empty() || str.find('|') != std::string::npos || str.size() > MAX_NAME_LENGTH) {
        return false;
    }
    for (char ch : str) {
        unsigned char c = static_cast<unsigned char>(ch);
        if (c < 32 || c > 126) return false;
    }
    return true;
}
