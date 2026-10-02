#ifndef DATE_H
#define DATE_H

#include <string>
#include <iostream>

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
    Date();
    Date(int d, int m, int y);
    explicit Date(const std::string& dateStr);

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    void setDay(int d);
    void setMonth(int m);
    void setYear(int y);

    std::string toString() const;

    static bool isLeapYear(int y);
    static int daysInMonth(int m, int y);
    static bool isValid(int d, int m, int y);
    static bool isValidBirthDate(const Date& d,
                                 int minAge = CalendarConstants::DEFAULT_MIN_SUBSCRIBER_AGE,
                                 int maxAge = CalendarConstants::DEFAULT_MAX_SUBSCRIBER_AGE,
                                 const Date& relativeTo = Date::now());
    static Date parse(const std::string& str);
    static Date now();

    int calculateAge(const Date& relativeTo = Date::now()) const;
    bool isFuture(const Date& relativeTo = Date::now()) const;
    bool isPast(const Date& relativeTo = Date::now()) const;

    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Date& d);
    friend std::istream& operator>>(std::istream& is, Date& d);
};

#endif // DATE_H
