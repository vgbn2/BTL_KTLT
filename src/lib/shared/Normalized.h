#ifndef NORMALIZED_H
#define NORMALIZED_H
#include <vector>
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>

namespace Normalized{
    std::string Trim(const std::string& str);
    std::string CollapseSpace(const std::string& str);
    std::string removeSymbols(const std::string& str);

    std::string toLower(const std::string& str);
    std::string toUpper(const std::string& str);
    
    std::string NormalizedName(const std::string& str);
    std::string NormalizedId(const std::string& str);
    std::string NormalizedGoiCuoc(const std::string& str);
    std::string NormalizedProvinceCode(const std::string& str);
 
}   


#endif // NORMALIZED_H