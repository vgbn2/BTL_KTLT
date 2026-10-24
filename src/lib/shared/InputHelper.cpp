#include "InputHelper.h"
#include "Exceptions.h"
#include "Normalized.h"
#include <iostream>
#include <limits>

//xoa trang thai loi va bo dem nhap
void InputHelper::clearBuffer() {
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//doc ten va ap dung cung mot cong validation voi model
std::string InputHelper::getName(const std::string& prompt, bool allowEmpty) {
    while (true) {
        std::string value = getString(prompt, allowEmpty);
        if ((allowEmpty && value.empty()) || Normalized::isValidName(value)) return value;
        if (std::cin.eof()) return "";
        std::cout << "  [!] Ten khong duoc rong, chua '|' hoac qua 50 ky tu! Vui long nhap lai.\n";
    }
}

//doc chuoi va kiem tra ky tu phan cach
std::string InputHelper::getString(const std::string& prompt, bool allowEmpty) {
    std::string val;
    while (true) {
        std::cout << prompt;
        //doc tron dong roi chuan hoa truoc khi thuc hien bat ky kiem tra nao.
        if (!std::getline(std::cin, val)) {
            if (std::cin.eof()) return "";
            clearBuffer();
            continue;
        }
        val = Normalized::trim(val);
        if (!allowEmpty && val.empty()) {
            std::cout << "  [!] Gia tri khong duoc de trong! Vui long nhap lai.\n";
            continue;
        }
        //ky tu '|' la dau phan cach khi luu tep,cho phep no se lam thay doi schema.
        if (val.find('|') != std::string::npos) {
            std::cout << "  [!] Gia tri khong duoc chua ky tu phan cach '|'! Vui long nhap lai.\n";
            continue;
        }
        return val;
    }
}

//doc so nguyen trong khoang cho phep
int InputHelper::getInt(const std::string& prompt, int minVal, int maxVal) {
    while (true) {
        std::string s = getString(prompt, false);
        if (s.empty() && std::cin.eof()) return minVal;
        try {
            size_t idx = 0;
            int val = std::stoi(s, &idx);
            //yeu cau phep chuyen doi doc het moi ky tu;tu choi tien to so kem ky tu rac.
            if (idx == s.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {
        }
        std::cout << "  [!] Gia tri khong hop le (Yeu cau so nguyen tu " << minVal << " den " << maxVal << ")! Thu lai.\n";
    }
}

//doc so thuc trong khoang cho phep
double InputHelper::getDouble(const std::string& prompt, double minVal, double maxVal) {
    while (true) {
        std::string s = getString(prompt, false);
        if (s.empty() && std::cin.eof()) return minVal;
        try {
            size_t idx = 0;
            double val = std::stod(s, &idx);
            //kiem tra vi tri doc ngan gia tri nhu "1000xyz" bi chap nhan thanh 1000.
            if (idx == s.length() && val >= minVal && val <= maxVal) {
                return val;
            }
        } catch (...) {
        }
        std::cout << "  [!] Gia tri khong hop le (Yeu cau so thuc tu " << minVal << " den " << maxVal << ")! Thu lai.\n";
    }
}

//doc ngay hop le tu ban phim
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

//doc ngay sinh trong khoang tuoi cho phep
Date InputHelper::getBirthDate(const std::string& prompt, int minAge, int maxAge) {
    while (true) {
        Date d = getDate(prompt);
        if (std::cin.eof()) return d;
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


//doc va kiem tra so dien thoai
std::string InputHelper::getPhoneNumber(const std::string& prompt, bool allowUnassigned) {
    while (true) {
        std::string s = getString(prompt, allowUnassigned);
        if (allowUnassigned && s.empty()) {
            return InputLimits::UNASSIGNED_PHONE_TAG;
        }
        if (Normalized::isValidPhoneNumber(s, allowUnassigned)) {
            return s;
        }
        if (std::cin.eof()) {
            return "";
        }
        std::cout << "  [!] So dien thoai khong hop le! Yeu cau dung 10 chu so Viet Nam (bat dau bang 03, 05, 07, 08, 09).\n";
    }
}

//doc cau tra loi xac nhan
bool InputHelper::getConfirm(const std::string& prompt) {
    while (true) {
        std::string s = getString(prompt + " (y/n): ", false);
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        if (std::cin.eof()) return false;
        std::cout << "  [!] Vui long chi nhap 'y' hoac 'n'.\n";
    }
}

//tam dung cho nguoi dung xem ket qua
void InputHelper::pause(const std::string& message) {
    std::cout << message;
    std::string dummy;
    std::getline(std::cin, dummy);
}
