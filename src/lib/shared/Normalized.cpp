#include "Normalized.h"
#include <sstream>
#include <cctype>
#include <iomanip>

namespace Normalized{
    std::string trim(const std::string& str){
        size_t first=str.find_first_not_of(" \t\r\n");
        if(first == std::string::npos)return"";
        size_t last=str.find_last_not_of("\t\r\n");
        if(last == std:: string::npos)return"";
        return str.substr(first, last - first + 1);
    }
    std::string CollapseSpace(const std::string& str){
        std::string trimmed = trim(str);
        std::string result;
        bool inSpace=false;
        for(char c : trimmed){
                if(std::isspace(static_cast<unsigned char>(c))){
                    if(!inSpace){
                        result+=' ';
                        inSpace=true;
                    }
                }
                else{
                    result+=c;
                    inSpace=false;
                }
        }
        return result;
    }

    std::string removeSymbols(const std::string& str) {
        const std::string FORBIDDEN = "|{}><~`#@!%^&():;";
        std::string result = str;
        for (char& c : result) {
            if (FORBIDDEN.find(c) != std::string::npos) {
                c = ' ';
            }
        }
        return CollapseSpace(result);
    }

    std::string toLower(const std::string& str){
         std::string result = str;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;

    }
    std::string toUpper(const std::string& str){
        std::string result=str;
    for(char& c: result){
        c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return result;
    }
    std::string NormalizedName(const std::string& rawName){
        std::string cleaned = removeSymbols(rawName);
        if (cleaned.empty()) return "";
        std::istringstream iss(cleaned);
        std::string word;
        std::string result;
        while (iss >> word) {
            if (!result.empty()) result += ' ';
            result += word;
        }
        return result;
    }
    std::string NormalizedId(const std::string& RawId){
         std::string cleaned = toUpper(trim(RawId));
    if (cleaned.empty()) return "";

    std::string digits;
    for (char c : cleaned) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digits += c;
        }
    }
    if (digits.empty()) return cleaned;

  
    std::string NormalizedGoiCuoc(const std::string& str){
        // chuan hoa goi cuoc
    }
    std::string NormalizedProvinceCode(const std::string& str){
        //Ha Noi->HNI
        //TuyenQuang->TQG

    }


}
}