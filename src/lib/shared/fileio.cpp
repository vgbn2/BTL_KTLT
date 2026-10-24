#include "fileio.h"
#include "Normalized.h"
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cerrno>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

//tach chuoi thanh cac truong du lieu
std::vector<std::string> FileIO::split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    //cat khoang trang tung truong tai ranh gioi phan cach truoc khi dua vao model.
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(Normalized::trim(token));
    }
    //giu truong rong cuoi dong de buoc kiem tra so luong truong tu choi ro rang.
    if (!s.empty() && s.back() == delimiter) {
        tokens.push_back("");
    }
    return tokens;
}

//tao thu muc cha neu chua ton tai
bool FileIO::ensureDirectoryExists(const std::string& filepath) {
    size_t pos = filepath.find_last_of("/\\");
    if (pos == std::string::npos) return true;
    std::string dir = filepath.substr(0, pos);
#if defined(_WIN32)
    _mkdir(dir.c_str());
#else
    mkdir(dir.c_str(), 0755);
#endif
    return true;
}

//khoi tao thu muc tam tren bo nho phu hop
bool FileIO::initTmpDir() {
#if defined(_WIN32)
    _mkdir(FilePaths::TMP_DIR.c_str());
    return true;
#else
    struct stat st;
    if (lstat(FilePaths::TMP_DIR.c_str(), &st) == 0) {
        struct stat target;
        if (stat(FilePaths::TMP_DIR.c_str(), &target) == 0 && S_ISDIR(target.st_mode)) {
            return true;
        }
        if (!S_ISLNK(st.st_mode)) {
            return false;
        }

        //khoi phuc tmpfs truoc,loi moi dung o cung
        mkdir("/dev/shm/ktlt_tmps", 0777);
        if (stat(FilePaths::TMP_DIR.c_str(), &target) == 0 && S_ISDIR(target.st_mode)) {
            return true;
        }
        if (unlink(FilePaths::TMP_DIR.c_str()) != 0) {
            return false;
        }
        return mkdir(FilePaths::TMP_DIR.c_str(), 0755) == 0 || errno == EEXIST;
    }

    //tao symlink sang /dev/shm de luu tren ram
    struct stat shmStat;
    if (stat("/dev/shm", &shmStat) == 0 && S_ISDIR(shmStat.st_mode)) {
        mkdir("/dev/shm/ktlt_tmps", 0777);
        if (symlink("/dev/shm/ktlt_tmps", FilePaths::TMP_DIR.c_str()) == 0) {
            return true;
        }
    }

    return mkdir(FilePaths::TMP_DIR.c_str(), 0755) == 0;
#endif
}

//tao duong dan tep tam tu tep dich
std::string FileIO::getTempStagingPath(const std::string& targetFilePath) {
    initTmpDir();
    size_t lastSlash = targetFilePath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? targetFilePath : targetFilePath.substr(lastSlash + 1);
    return FilePaths::TMP_DIR + "/" + filename + ".tmp";
}

//commit tep tam vao tep dich
bool FileIO::commitFromTmp(const std::string& tmpPath, const std::string& targetPath) {
    ensureDirectoryExists(targetPath);

    //doi ten truc tiep neu tren cung mot phan vung o dia
#if defined(_WIN32)
    if (MoveFileExA(tmpPath.c_str(), targetPath.c_str(),
                    MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        return true;
    }
#else
    if (std::rename(tmpPath.c_str(), targetPath.c_str()) == 0) {
        return true;
    }
#endif

    //sao chep qua phan vung o dia (tu ram /dev/shm sang ssd)
    std::ifstream src(tmpPath, std::ios::binary);
    if (!src.is_open()) {
        return false;
    }

    std::string intermediateTarget = targetPath + ".ssd_swap";
    std::ofstream dst(intermediateTarget, std::ios::binary | std::ios::trunc);
    if (!dst.is_open()) {
        src.close();
        return false;
    }

    //tranh loi failbit khi file 0 byte,chi doc khi co data
    if (src.peek() != std::ifstream::traits_type::eof()) {
        dst << src.rdbuf();
    }
    dst.flush();
    bool copyOk = !src.bad() && dst.good();
    src.close();
    dst.close();
    copyOk = copyOk && !dst.fail();
    if (!copyOk) {
        std::remove(intermediateTarget.c_str());
        return false;
    }

#if defined(_WIN32)
    bool replaced = MoveFileExA(intermediateTarget.c_str(), targetPath.c_str(),
                                MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
#else
    bool replaced = std::rename(intermediateTarget.c_str(), targetPath.c_str()) == 0;
#endif
    if (!replaced) {
        std::remove(intermediateTarget.c_str());
        return false;
    }
    std::remove(tmpPath.c_str());
    return true;
}

//kiem tra thu muc tam co dung tmpfs
bool FileIO::isUsingTmpfs() {
#if defined(__linux__)
    struct stat st;
    if (lstat(FilePaths::TMP_DIR.c_str(), &st) == 0 && S_ISLNK(st.st_mode)) {
        return true;
    }
#endif
    return false;
}

//noi thu muc va ten tep thanh duong dan
std::string FileIO::joinPath(const std::string& directory, const std::string& filename) {
    if (directory.empty()) return filename;
    if (directory.back() == '/' || directory.back() == '\\') return directory + filename;
    return directory + "/" + filename;
}

//tim thu muc data day du theo vi tri executable
std::string FileIO::resolveDataDirectory(const std::string& executablePath) {
    const std::vector<std::string> candidates = {
        "data"
    };

    std::vector<std::string> searchPaths = candidates;
    size_t slash = executablePath.find_last_of("/\\");
    if (slash != std::string::npos) {
        std::string executableDirectory = executablePath.substr(0, slash);
        searchPaths.push_back(joinPath(executableDirectory, "data"));
        searchPaths.push_back(joinPath(joinPath(executableDirectory, ".."), "data"));
        searchPaths.push_back(joinPath(joinPath(joinPath(executableDirectory, ".."), ".."), "data"));
    }

    const std::vector<std::string> requiredFiles = {
        "hopdong.txt",
        "imei.txt", 
        "CacGoiCuoc.txt", 
        "thuebao.txt",
        "khachhang.txt", 
        "hoadon.txt", 
        "naptien.txt", 
        "ghicuocgoi.txt", 
        "khieunai.txt"
    };
for (const auto& directory : searchPaths) {
        bool complete = true;
        for (const auto& filename : requiredFiles) {
            std::ifstream file(joinPath(directory, filename));
            if (!file.good()) {
                complete = false;
                break;
            }
        }
        if (complete) return directory;
    }
    return "";
}
