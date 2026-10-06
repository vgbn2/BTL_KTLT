#include "InputHelper.h"
#include "Exceptions.h"
#include "fileio.h"
#include <iostream>
#include <limits>

void InputHelper::clearBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string InputHelper::getString(const std::string& prompt, bool allowEmpty) {
    std::string val;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, val)) {
            if (std::cin.eof()) return "";
            clearBuffer();
            continue;
        }
        val = FileIO::trim(val);
        if (!allowEmpty && val.empty()) {
            std::cout << "  [!] Gia tri khong duoc de trong! Vui long nhap lai.\n";
            continue;
        }
        if (val.find('|') != std::string::npos) {
            std::cout << "  [!] Gia tri khong duoc chua ky tu phan cach '|'! Vui long nhap lai.\n";
            continue;
        }
        return val;
    }
}

int InputHelper::getInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::string s = getString(prompt, false);
        if (s.empty() && std::cin.eof()) return minVal;
        try {
            size_t idx = 0;
            int val = std::stoi(s, &idx);
            if (idx == s.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {
        }
        std::cout << "  [!] Gia tri khong hop le (Yeu cau so nguyen tu " << minVal << " den " << maxVal << ")! Thu lai.\n";
    }
}

double InputHelper::getDouble(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        std::string s = getString(prompt, false);
        if (s.empty() && std::cin.eof()) return minVal;
        try {
            size_t idx = 0;
            double val = std::stod(s, &idx);
            if (idx == s.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {
        }
        std::cout << "  [!] Gia tri khong hop le (Yeu cau so thuc tu " << minVal << " den " << maxVal << ")! Thu lai.\n";
    }
}

Date InputHelper::getDate(const std::string& prompt) {
    while (true) {
        std::string s = getString(prompt, false);
        if (s.empty() && std::cin.eof()) return Date();
        try {
            return Date::parse(s);
        } catch (const InvalidDateException& e) {
            std::cout << "  [!] " << e.what() << " Vui long nhap lai.\n";
        }
    }
}

Date InputHelper::getBirthDate(const std::string& prompt, int minAge, int maxAge) {
    while (true) {
        Date d = getDate(prompt);
        if (Date::isValidBirthDate(d, minAge, maxAge)) {
            return d;
        }
        if (d.isFuture()) {
            std::cout << "  [!] Ngay sinh khong the la ngay trong tuong lai! Vui long nhap lai.\n";
        } else {
            std::cout << "  [!] Do tuoi chu thue bao khong hop le (Yeu cau tu " << minAge << " den " << maxAge << " tuoi)! Thu lai.\n";
        }
    }
}

bool InputHelper::isValidPhoneNumber(const std::string& phone, bool allowUnassigned) {
    if (allowUnassigned && (phone.empty() || phone == InputLimits::UNASSIGNED_PHONE_TAG)) {
        return true;
    }
    if (phone.length() != InputLimits::REQUIRED_PHONE_LENGTH) {
        return false;
    }
    if (phone[0] != InputLimits::PHONE_PREFIX) {
        return false;
    }
    char secondDigit = phone[1];
    if (secondDigit != '3' && secondDigit != '5' && secondDigit != '7' && secondDigit != '8' && secondDigit != '9') {
        return false;
    }
    for (char c : phone) {
        if (c < '0' || c > '9') {
            return false;
        }
    }
    return true;
}

std::string InputHelper::getPhoneNumber(const std::string& prompt, bool allowUnassigned) {
    while (true) {
        std::string s = getString(prompt, allowUnassigned);
        if (allowUnassigned && s.empty()) {
            return InputLimits::UNASSIGNED_PHONE_TAG;
        }
        if (isValidPhoneNumber(s, allowUnassigned)) {
            return s;
        }
        std::cout << "  [!] So dien thoai khong hop le! Yeu cau dung 10 chu so Viet Nam (bat dau bang 03, 05, 07, 08, 09).\n";
    }
}

bool InputHelper::getConfirm(const std::string& prompt) {
    while (true) {
        std::string s = getString(prompt + " (y/n): ", false);
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        std::cout << "  [!] Vui long chi nhap 'y' hoac 'n'.\n";
    }
}

void InputHelper::pause(const std::string& message) {
    std::cout << message;
    std::string dummy;
    std::getline(std::cin, dummy);
}
