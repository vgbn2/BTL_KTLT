/**
 * @file Date.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 */

#ifndef DATE_H
#define DATE_H

#include <string>
#include <iosfwd>

namespace CalendarConstants {
    const int MIN_VALID_YEAR = 1900;
    const int MAX_VALID_YEAR = 2100;
    const int DEFAULT_MIN_SUBSCRIBER_AGE = 14;
    const int DEFAULT_MAX_SUBSCRIBER_AGE = 120;

    const int MONTHS_PER_YEAR = 12;
    const int MONTH_FEBRUARY = 2;

    const int DAYS_FEB_LEAP = 29;
    const int DAYS_FEB_NORMAL = 28;
    const int DAYS_LONG_MONTH = 31;
    const int DAYS_SHORT_MONTH = 30;
}

class Date {
private:
    int day;
    int month;
    int year;

public:
    //tao ngay mac dinh
    Date();
    //tao ngay tu ngay,thang,nam
    Date(int d, int m, int y);
    //tao ngay tu chuoi
    explicit Date(const std::string& dateStr);

    //lay ngay
    int getDay() const { return day; }
    //lay thang
    int getMonth() const { return month; }
    //lay nam
    int getYear() const { return year; }

    //cap nhat ngay
    void setDay(int d);
    //cap nhat thang
    void setMonth(int m);
    //cap nhat nam
    void setYear(int y);
    //cap nhat ca ngay,thang,nam cung luc
    void setDate(int d, int m, int y);

    //chuyen ngay thanh chuoi
    std::string toString() const;

    //kiem tra nam nhuan
    static bool isLeapYear(int y);
    //lay so ngay trong thang
    static int daysInMonth(int m, int y);
    //kiem tra ngay hop le
    static bool isValid(int d, int m, int y);
    //kiem tra ngay sinh theo do tuoi
    static bool isValidBirthDate(const Date& d,
                                 int minAge = CalendarConstants::DEFAULT_MIN_SUBSCRIBER_AGE,
                                 int maxAge = CalendarConstants::DEFAULT_MAX_SUBSCRIBER_AGE,
                                 const Date& relativeTo = Date::now());
    //phan tich ngay tu chuoi
    static Date parse(const std::string& str);
    //lay ngay hien tai
    static Date now();

    //tinh tuoi tai moc thoi gian
    int calculateAge(const Date& relativeTo = Date::now()) const;
    //kiem tra ngay nam trong tuong lai
    bool isFuture(const Date& relativeTo = Date::now()) const;
    //kiem tra ngay nam trong qua khu
    bool isPast(const Date& relativeTo = Date::now()) const;

    //so sanh ngay nho hon
    bool operator<(const Date& other) const;
    //so sanh ngay nho hon hoac bang
    bool operator<=(const Date& other) const;
    //so sanh ngay lon hon
    bool operator>(const Date& other) const;
    //so sanh ngay lon hon hoac bang
    bool operator>=(const Date& other) const;
    //so sanh hai ngay bang nhau
    bool operator==(const Date& other) const;
    //so sanh hai ngay khac nhau
    bool operator!=(const Date& other) const;

    //ghi ngay ra luong
    friend std::ostream& operator<<(std::ostream& os, const Date& d);
    //doc ngay tu luong
    friend std::istream& operator>>(std::istream& is, Date& d);
};

#endif // DATE_H
