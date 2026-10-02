## Comprehensive explanation of custom libraries ##

# Date.h #
Purpose of the library: Time checking, invalid/valid time, counting subscribers age,subcription times, expiration date.

# Hardcoded list of variables:
```cpp```
namespace CalendarConstants{
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
# Boolean functions
```cpp```
bool Date::isLeapYear(y){
    leap year condition;
}
bool Date::isValid(int d,int m,int y){
    check year condition->return true/false
    check month condition->return true/false
}
bool Date::isValidBirthDate(const Date& d,int minAge,int maxAge,const Date& relative to){
    if (d.isFuture(relative to))    //check current date,compare it to birthdate->caculate age
    int age=d.caculateAge(relativeTo);
    return age inside a condtioned range
}

will update this later

# fileio.h #
purpose of the library: trim strings,get string, normalized string(soon to be implmented)


# IMEI.h #
