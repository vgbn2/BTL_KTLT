#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <vector>

namespace FilePaths {
    const std::string HOPDONG_DATA = "data/hopdong.txt";
    const std::string IMEI_DATA = "data/imei.txt";
}

namespace FileConstants {
    const char DEFAULT_DELIMITER = '|';
    const char COMMENT_PREFIX = '#';
}

class FileIO {
public:
    static std::vector<std::string> split(const std::string& s, char delimiter = FileConstants::DEFAULT_DELIMITER);
    static std::string trim(const std::string& s);
    static bool ensureDirectoryExists(const std::string& filepath);
};

#endif // FILEIO_H
