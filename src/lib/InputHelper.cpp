#include "InputHelper.h"
#include "Exceptions.h"
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
        if (!allowEmpty && val.empty()) {
            std::cout << "  [!] Gia tri khong duoc de trong! Vui long nhap lai.\n";
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

bool InputHelper::getConfirm(const std::string& prompt) {
    while (true) {
        std::string s = getString(prompt + " (y/n): ", false);
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        std::cout << "  [!] Vui long chi nhap 'y' (Dong y) hoac 'n' (Huy thao tac).\n";
    }
}

void InputHelper::pause(const std::string& message) {
    std::cout << message;
    std::string dummy;
    std::getline(std::cin, dummy);
}
