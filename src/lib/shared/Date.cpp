#include "Date.h"
#include "Exceptions.h"
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace CalendarConstants;

Date::Date() : day(1), month(1), year(2000) {}

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

Date::Date(const std::string& dateStr) {
    *this = parse(dateStr);
}

void Date::setDay(int d) {
    if (!isValid(d, month, year)) {
        throw InvalidDateException("Ngay khong hop le!");
    }
    day = d;
}

void Date::setMonth(int m) {
    if (!isValid(day, m, year)) {
        throw InvalidDateException("Thang khong hop le!");
    }
    month = m;
}

void Date::setYear(int y) {
    if (!isValid(day, month, y)) {
        throw InvalidDateException("Nam khong hop le!");
    }
    year = y;
}

bool Date::isLeapYear(int y) {
    return (y % 400 == 0) || (y % 4 == 0 && y % 100 != 0);
}

int Date::daysInMonth(int m, int y) {
    if (m < 1 || m > MONTHS_PER_YEAR) return 0;
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

bool Date::isValid(int d, int m, int y) {
    if (y < MIN_VALID_YEAR || y > MAX_VALID_YEAR) return false;
    if (m < 1 || m > MONTHS_PER_YEAR) return false;
    int maxDays = daysInMonth(m, y);
    return (d >= 1 && d <= maxDays);
}

Date Date::parse(const std::string& str) {
    int d = 0, m = 0, y = 0;
    char sep1 = 0, sep2 = 0;
    std::istringstream iss(str);
    if (!(iss >> d >> sep1 >> m >> sep2 >> y) || (sep1 != '/' && sep1 != '-') || (sep2 != '/' && sep2 != '-')) {
        throw InvalidDateException("Dinh dang ngay phai la DD/MM/YYYY (Chuoi nhap: '" + str + "')");
    }
    return Date(d, m, y);
}

Date Date::now() {
    std::time_t t = std::time(nullptr);
    std::tm* localTime = std::localtime(&t);
    return Date(localTime->tm_mday, localTime->tm_mon + 1, localTime->tm_year + 1900);
}

int Date::calculateAge(const Date& relativeTo) const {
    int age = relativeTo.year - year;
    if (relativeTo.month < month || (relativeTo.month == month && relativeTo.day < day)) {
        age--;
    }
    return age;
}

bool Date::isFuture(const Date& relativeTo) const {
    return *this > relativeTo;
}

bool Date::isPast(const Date& relativeTo) const {
    return *this < relativeTo;
}

bool Date::isValidBirthDate(const Date& d, int minAge, int maxAge, const Date& relativeTo) {
    if (d.isFuture(relativeTo)) {
        return false;
    }
    int age = d.calculateAge(relativeTo);
    return (age >= minAge && age <= maxAge);
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << std::setfill('0') << std::setw(4) << year;
    return oss.str();
}

bool Date::operator<(const Date& other) const {
    if (year != other.year) return year < other.year;
    if (month != other.month) return month < other.month;
    return day < other.day;
}

bool Date::operator<=(const Date& other) const {
    return (*this < other) || (*this == other);
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator>=(const Date& other) const {
    return !(*this < other);
}

bool Date::operator==(const Date& other) const {
    return (year == other.year && month == other.month && day == other.day);
}

bool Date::operator!=(const Date& other) const {
    return !(*this == other);
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
    os << d.toString();
    return os;
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
