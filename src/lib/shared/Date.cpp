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

//tao ngay mac dinh
Date::Date() : day(1), month(1), year(2000) {}

//tao ngay va kiem tra tinh hop le
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

//tao ngay tu chuoi
Date::Date(const std::string& dateStr) : Date(parse(dateStr)) {}

//cap nhat ngay va kiem tra tinh hop le
void Date::setDay(int d) {
    if (!isValid(d, month, year)) {
        throw InvalidDateException("Ngay khong hop le!");
    }
    day = d;
}

//cap nhat thang va kiem tra tinh hop le
void Date::setMonth(int m) {
    if (!isValid(day, m, year)) {
        throw InvalidDateException("Thang khong hop le voi ngay/nam hien tai!");
    }
    month = m;
}

//cap nhat nam va kiem tra tinh hop le
void Date::setYear(int y) {
    if (!isValid(day, month, y)) {
        throw InvalidDateException("Nam khong hop le voi ngay/thang hien tai!");
    }
    year = y;
}

//cap nhat ca ngay,thang,nam cung luc
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

//kiem tra nam co phai nam nhuan
bool Date::isLeapYear(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

//lay so ngay toi da cua thang
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

//kiem tra ngay,thang,nam hop le
bool Date::isValid(int d, int m, int y) {
    if (y < MIN_VALID_YEAR || y > MAX_VALID_YEAR) return false;
    if (m < 1 || m > MONTHS_PER_YEAR) return false;
    return d >= 1 && d <= daysInMonth(m, y);
}

//phan tich chuoi thanh ngay (ho tro ca "now" de lay thoi gian hien tai)
Date Date::parse(const std::string& str) {
    if (str == "now" || str == "NOW" || str == "Now") {
        return Date::now();
    }
    int d = 0, m = 0, y = 0;
    char sep1 = 0, sep2 = 0;
    std::istringstream iss(str);
    //khong bo qua khoang trang: chi chap nhan dung dinh dang
    iss >> std::noskipws;
    //kiem tra dau gach va chan ky tu thua phia sau
    if (!(iss >> d >> sep1 >> m >> sep2 >> y)
        || (sep1 != '/' && sep1 != '-')
        || sep1 != sep2
        || iss.peek() != std::char_traits<char>::eof()) {
        throw InvalidDateException("Dinh dang ngay phai la DD/MM/YYYY (Chuoi nhap: '" + str + "')");
    }
    return Date(d, m, y);
}

//lay ngay hien tai cua he thong (thread-safe)
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

//tinh tuoi tai ngay tham chieu
int Date::calculateAge(const Date& relativeTo) const {
    int age = relativeTo.year - year;
    if (relativeTo.month < month || (relativeTo.month == month && relativeTo.day < day)) {
        age--;
    }
    return age;
}

//kiem tra ngay co nam trong tuong lai
bool Date::isFuture(const Date& relativeTo) const {
    return *this > relativeTo;
}

//kiem tra ngay co nam trong qua khu
bool Date::isPast(const Date& relativeTo) const {
    return *this < relativeTo;
}

//kiem tra ngay sinh theo khoang tuoi
bool Date::isValidBirthDate(const Date& d, int minAge, int maxAge, const Date& relativeTo) {
    if (d.isFuture(relativeTo)) {
        return false;
    }
    int age = d.calculateAge(relativeTo);
    return age >= minAge && age <= maxAge;
}

//dinh dang ngay thanh chuoi
std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << std::setfill('0') << std::setw(4) << year;
    return oss.str();
}

//so sanh ngay nho hon
bool Date::operator<(const Date& o) const {
    return std::tie(year, month, day) < std::tie(o.year, o.month, o.day);
}

//so sanh ngay nho hon hoac bang
bool Date::operator<=(const Date& o) const { return !(o < *this); }

//so sanh ngay lon hon
bool Date::operator>(const Date& o) const { return o < *this; }

//so sanh ngay lon hon hoac bang
bool Date::operator>=(const Date& o) const { return !(*this < o); }

//so sanh hai ngay bang nhau
bool Date::operator==(const Date& o) const {
    return year == o.year && month == o.month && day == o.day;
}

//so sanh hai ngay khac nhau
bool Date::operator!=(const Date& o) const { return !(*this == o); }

//ghi ngay ra luong
std::ostream& operator<<(std::ostream& os, const Date& d) {
    return os << d.toString();
}

//doc ngay tu luong
std::istream& operator>>(std::istream& is, Date& d) {
    std::string s;
    if (is >> s) {
        try {
            d = Date::parse(s);
        } catch (const InvalidDateException&) {
            is.setstate(std::ios::failbit);
        }
    }
    return is;
}
