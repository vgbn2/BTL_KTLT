/**
 * @file Date.cpp
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#include "Date.h"
#include "Exceptions.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <tuple>
#include <string>
#include <istream>
#include <ostream>

using namespace CalendarConstants;

// tao ngay mac dinh
Date::Date() : day(1), month(1), year(2000) {}

// tao ngay va kiem tra tinh hop le
Date::Date(int d, int m, int y) {
    if (!isValid(d, m, y)) {
        std::ostringstream oss;
        oss << "Ngay khong hop le: " << d << "/" << m << "/" << y;
        throw InvalidDateException(oss.str());
    }
    day = d;
    month = m;
    year = y;
}

// tao ngay tu chuoi
Date::Date(const std::string& dateStr) : Date(parse(dateStr)) {}

// cap nhat ngay va kiem tra tinh hop le
void Date::setDay(int d) {
    if (!isValid(d, month, year)) {
        throw InvalidDateException("Ngay khong hop le!");
    }
    day = d;
}

// cap nhat thang va kiem tra tinh hop le
void Date::setMonth(int m) {
    if (!isValid(day, m, year)) {
        throw InvalidDateException("Thang khong hop le voi ngay/nam hien tai!");
    }
    month = m;
}

// cap nhat nam va kiem tra tinh hop le
void Date::setYear(int y) {
    if (!isValid(day, month, y)) {
        throw InvalidDateException("Nam khong hop le voi ngay/thang hien tai!");
    }
    year = y;
}

// cap nhat ca ngay,thang,nam cung luc
void Date::setDate(int d, int m, int y) {
    if (!isValid(d, m, y)) {
        std::ostringstream oss;
        oss << "Ngay khong hop le: " << d << "/" << m << "/" << y;
        throw InvalidDateException(oss.str());
    }
    day = d;
    month = m;
    year = y;
}

// kiem tra nam nhuan theo lich Gregory: chia het cho 400 hoac (chia het cho 4 va khong chia het 100)
// vi du: 2000, 2024 -> nhuan; 1900, 2026 -> khong nhuan
bool Date::isLeapYear(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

// so ngay toi da trong thang: T2 co 28 (thuong) hoac 29 (nhuan); T4,6,9,11 co 30; con lai co 31
int Date::daysInMonth(int m, int y) {
    switch (m) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return DAYS_LONG_MONTH;
        case 4: case 6: case 9: case 11:
            return DAYS_SHORT_MONTH;
        case MONTH_FEBRUARY:
            return isLeapYear(y) ? DAYS_FEB_LEAP : DAYS_FEB_NORMAL;
        default:
            return 0;
    }
}

// kiem tra ngay hop le trong khoang nam [1900, 2100]
// vi du: 29/02/2024 -> hop le; 29/02/2026, 31/04/2026 -> khong hop le
bool Date::isValid(int d, int m, int y) {
    if (y < MIN_VALID_YEAR || y > MAX_VALID_YEAR) return false;
    if (m < 1 || m > MONTHS_PER_YEAR) return false;
    return d >= 1 && d <= daysInMonth(m, y);
}

// phan tich chuoi DD/MM/YYYY, DD-MM-YYYY hoac "now" (chan ky tu thua phia sau)
// vi du: "15/10/2026", "now" -> hop le; "15.10.2026", "15/10/2026abc" -> tu choi
Date Date::parse(const std::string& str) {
    if (str == "now" || str == "NOW" || str == "Now") {
        return Date::now();
    }
    int d = 0, m = 0, y = 0;
    char sep1 = 0, sep2 = 0;
    std::istringstream iss(str);
    iss >> std::noskipws;
    if (!(iss >> d >> sep1 >> m >> sep2 >> y)
        || (sep1 != '/' && sep1 != '-')
        || sep1 != sep2
        || iss.peek() != std::char_traits<char>::eof()) {
        throw InvalidDateException("Dinh dang ngay phai la DD/MM/YYYY (Chuoi nhap: '" + str + "')");
    }
    return Date(d, m, y);
}

// lay ngay hien tai cua he thong (thread-safe)
Date Date::now() {
    std::time_t t = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    return Date(tmv.tm_mday, tmv.tm_mon + 1, tmv.tm_year + 1900);
}

// tinh tuoi tai ngay tham chieu, chi tinh theo nam
// vi du,
// neu sinh 2/1/1999, thoi diem hien tai la 15/10/2026
// 2026-1999=27 co thang 1<thang 10, va ngay 2<15->27 tuoi
// neu sinh 11/11/1999, thoi diem hien tai la 15/10/2026
// 2026-1999=27, nhung thang 11>10 ->26 tuoi
int Date::calculateAge(const Date& relativeTo) const {
    int age = relativeTo.year - year;
    if (relativeTo.month < month || (relativeTo.month == month && relativeTo.day < day)) {
        age--;
    }
    return age;
}

// kiem tra ngay co nam trong tuong lai
bool Date::isFuture(const Date& relativeTo) const {
    return *this > relativeTo;
}

// kiem tra ngay co nam trong qua khu
bool Date::isPast(const Date& relativeTo) const {
    return *this < relativeTo;
}

// kiem tra ngay sinh theo do tuoi chu thue bao [14, 120] va khong o tuong lai
// vi du tai 15/10/2026: sinh 2015 (11 tuoi) -> tu choi; sinh 2005 (21 tuoi) -> hop le
bool Date::isValidBirthDate(const Date& d, int minAge, int maxAge, const Date& relativeTo) {
    if (d.isFuture(relativeTo)) {
        return false;
    }
    int age = d.calculateAge(relativeTo);
    return age >= minAge && age <= maxAge;
}

// dinh dang chuoi DD/MM/YYYY (vi du: 15/10/2026, 01/05/2026)
std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << std::setfill('0') << std::setw(4) << year;
    return oss.str();
}

// cong hoac tru so ngay, tu dong xu ly can bang thang/nam va nam nhuan
// vi du: 19/10/2026 + 30 -> 18/11/2026; 05/03/2026 - 10 -> 23/02/2026
Date Date::addDays(int days) const {
    int d = day;
    int m = month;
    int y = year;

    if (days >= 0) {
        d += days;
        while (d > daysInMonth(m, y)) {
            d -= daysInMonth(m, y);
            m++;
            if (m > CalendarConstants::MONTHS_PER_YEAR) {
                m = 1;
                y++;
            }
        }
    } else {
        d += days;
        while (d < 1) {
            m--;
            if (m < 1) {
                m = CalendarConstants::MONTHS_PER_YEAR;
                y--;
            }
            d += daysInMonth(m, y);
        }
    }
    return Date(d, m, y);
}

// so sanh thu tu tu dien: nam -> thang -> ngay
bool Date::operator<(const Date& o) const {
    return std::tie(year, month, day) < std::tie(o.year, o.month, o.day);
}

bool Date::operator<=(const Date& o) const { return !(o < *this); }
bool Date::operator>(const Date& o) const { return o < *this; }
bool Date::operator>=(const Date& o) const { return !(*this < o); }
bool Date::operator==(const Date& o) const {
    return year == o.year && month == o.month && day == o.day;
}
bool Date::operator!=(const Date& o) const { return !(*this == o); }

std::ostream& operator<<(std::ostream& os, const Date& d) {
    return os << d.toString();
}

std::istream& operator>>(std::istream& is, Date& d) {
    std::string s;
    if (is >> s) {
        try {
            d = Date::parse(s);
        } catch (...) {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}
