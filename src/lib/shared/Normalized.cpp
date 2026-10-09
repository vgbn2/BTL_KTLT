#include "Normalized.h"
#include <sstream>
#include <cctype>
#include <iomanip>

std::string Normalized::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    if (last == std::string::npos) return "";
    return str.substr(first, last - first + 1);
}

std::string Normalized::CollapseSpace(const std::string& str) {
    std::string trimmed = trim(str);
    std::string result;
    bool inSpace = false;
    for (char ch : trimmed) {
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            result += ch;
            inSpace = false;
            continue;
        }
        if (!inSpace) {
            result += ' ';
            inSpace = true;
        }
    }
    return result;
}

std::string Normalized::removeSymbols(const std::string& str) {
    const std::string FORBIDDEN = "|{}><~`#@!%^&():;";
    std::string result = str;
    for (char& ch : result) {
        if (FORBIDDEN.find(ch) != std::string::npos) {
            ch = ' ';
        }
    }
    return CollapseSpace(result);
}

std::string Normalized::toLower(const std::string& str) {
    std::string result = str;
    for (char& ch : result) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

std::string Normalized::toUpper(const std::string& str) {
    std::string result = str;
    for (char& ch : result) {
        ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
    }
    return result;
}

std::string Normalized::NormalizedName(const std::string& rawName) {
    std::string cleaned = removeSymbols(rawName);
    if (cleaned.empty()) return "";
    std::istringstream iss(cleaned);
    std::string word;
    std::string result;
    while (iss >> word) {
        if (!result.empty()) result += ' ';
        result += word;
    }
    return result;
}

bool Normalized::isValidPhoneNumber(const std::string& phone, bool allowUnassigned) {
    if (allowUnassigned && (phone.empty() || phone == "ChuaGan")) {
        return true;
    }
    if (phone.length() != 10 || phone[0] != '0') {
        return false;
    }
    char secondDigit = phone[1];
    if (secondDigit != '3' && secondDigit != '5' && secondDigit != '7' && secondDigit != '8' && secondDigit != '9') {
        return false;
    }
    for (char digitChar : phone) {
        if (!std::isdigit(static_cast<unsigned char>(digitChar))) {
            return false;
        }
    }
    return true;
}
