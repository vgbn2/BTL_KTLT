#ifndef INPUTHELPER_H
#define INPUTHELPER_H

#include <string>
#include "Date.h"
#include "Normalized.h"

namespace InputLimits {
    const int DEFAULT_MIN_INT = -2147483647;//32 bits
    const int DEFAULT_MAX_INT = 2147483647;
    const double DEFAULT_MIN_DOUBLE = 0.0;
    const double DEFAULT_MAX_DOUBLE = 1e12;
    const size_t REQUIRED_PHONE_LENGTH = 10;
    const char PHONE_PREFIX = '0';
    const std::string UNASSIGNED_PHONE_TAG = "ChuaGan";
}

class InputHelper {
public:
    //xoa trang thai loi va bo dem nhap
    static void clearBuffer();
    //doc ten tu ban phim
    static std::string getName(const std::string& prompt, bool allowEmpty = false);
    //doc chuoi tu ban phim
    static std::string getString(const std::string& prompt, bool allowEmpty = false);
    //doc so nguyen trong khoang cho phep
    static int getInt(const std::string& prompt, int minVal = InputLimits::DEFAULT_MIN_INT, int maxVal = InputLimits::DEFAULT_MAX_INT);
    //doc so thuc trong khoang cho phep
    static double getDouble(const std::string& prompt, double minVal = InputLimits::DEFAULT_MIN_DOUBLE, double maxVal = InputLimits::DEFAULT_MAX_DOUBLE);
    //doc ngay hop le tu ban phim
    static Date getDate(const std::string& prompt);
    //doc ngay sinh trong do tuoi cho phep
    static Date getBirthDate(const std::string& prompt,
                             int minAge = CalendarConstants::DEFAULT_MIN_SUBSCRIBER_AGE,
                             int maxAge = CalendarConstants::DEFAULT_MAX_SUBSCRIBER_AGE);
    //doc va kiem tra so dien thoai
    static std::string getPhoneNumber(const std::string& prompt, bool allowUnassigned = false);
    //doc cau tra loi xac nhan
    static bool getConfirm(const std::string& prompt);
    //tam dung cho nguoi dung xem ket qua
    static void pause(const std::string& message = "Nhan Enter de tiep tuc...");
};

#endif // INPUTHELPER_H
