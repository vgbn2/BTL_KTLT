#ifndef INPUTHELPER_H
#define INPUTHELPER_H

#include <string>
#include "Date.h"

namespace InputLimits {
    const int DEFAULT_MIN_INT = -2147483647;
    const int DEFAULT_MAX_INT = 2147483647;
    const double DEFAULT_MIN_DOUBLE = 0.0;
    const double DEFAULT_MAX_DOUBLE = 1e12;
}

class InputHelper {
public:
    static void clearBuffer();
    static std::string getString(const std::string& prompt, bool allowEmpty = false);
    static int getInt(const std::string& prompt, int minVal = InputLimits::DEFAULT_MIN_INT, int maxVal = InputLimits::DEFAULT_MAX_INT);
    static double getDouble(const std::string& prompt, double minVal = InputLimits::DEFAULT_MIN_DOUBLE, double maxVal = InputLimits::DEFAULT_MAX_DOUBLE);
    static Date getDate(const std::string& prompt);
    static bool getConfirm(const std::string& prompt);
    static void pause(const std::string& message = "Nhan Enter de tiep tuc...");
};

#endif // INPUTHELPER_H
