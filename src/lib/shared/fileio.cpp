/**
 * @file fileio.cpp
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 *
 * file nay chiu trach nhiem cho luong vao ra cua repo:
 * 1. tach chuoi (split): tach theo '|', giu truong rong de kiem tra loi
 * 2. tao thu muc tam tmps & khac biet OS (Linux vs Windows):
 *    - Tren Linux: tao symlink tmps sang /dev/shm (RAM tmpfs) 
 *      de tranh hao mon o SSD;
 *      khi commit thi phai copy sang .ssd_swap roi rename 
 *      vi rename khong nhay qua
 *      duoc 2 phan vung khac nhau (RAM sang o dia bi loi EXDEV). 
 *      Dung getpid() tranh trung lap.
 *    - Tren Windows: khong co /dev/shm nen tao thu muc tmps binh thuong (_mkdir);
 *      dung GetCurrentProcessId(), dung MoveFileExA (MOVEFILE_REPLACE_EXISTING va
 *      MOVEFILE_WRITE_THROUGH) de ghi de an toan va ep flush du lieu xuong dia.
 * 3. ghi tam roi commit: ghi ra file tam .tmp truoc,
 * kiem tra xong moi doi ten sang file goc
 *    de neu dang ghi ma bi crash/tat nguon thi du lieu cu kho co the bi hong
 * 4. resolveDataDirectory: tu tim duong dan thu muc data chua 9 file
 *     dulieu du chay o dau
 */

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
// quyen thu muc POSIX (he bat phan / octal):
// 0755 = rwxr-xr-x (chu so huu full quyen doc/ghi/chay 7, nhom va nguoi khac chi doc/chay 5)
// 0777 = rwxrwxrwx (full quyen cho tat ca tien trinh doc/ghi tren vung nho dem ram /dev/shm)
#define POSIX_DIR_PERMISSIONS 0755
#define POSIX_SHM_PERMISSIONS 0777
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
    // 0755 la he bat phan (octal) quyen POSIX rwxr-xr-x:
    // chu so huu full quyen doc/ghi/chay (7), nhom va nguoi khac chi doc va truy cap (5)
    mkdir(dir.c_str(), POSIX_DIR_PERMISSIONS);
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
        mkdir("/dev/shm/ktlt_tmps", POSIX_SHM_PERMISSIONS);
        if (stat(FilePaths::TMP_DIR.c_str(), &target) == 0 && S_ISDIR(target.st_mode)) {
            return true;
        }
        if (unlink(FilePaths::TMP_DIR.c_str()) != 0) {
            return false;
        }
        return mkdir(FilePaths::TMP_DIR.c_str(), POSIX_DIR_PERMISSIONS) == 0 || errno == EEXIST;
    }

    //tao symlink sang /dev/shm de luu tren ram
    struct stat shmStat;
    if (stat("/dev/shm", &shmStat) == 0 && S_ISDIR(shmStat.st_mode)) {
        mkdir("/dev/shm/ktlt_tmps", POSIX_SHM_PERMISSIONS);
        if (symlink("/dev/shm/ktlt_tmps", FilePaths::TMP_DIR.c_str()) == 0) {
            return true;
        }
    }

    return mkdir(FilePaths::TMP_DIR.c_str(), POSIX_DIR_PERMISSIONS) == 0;
#endif
}

//tao duong dan tep tam tu tep dich
std::string FileIO::getTempStagingPath(const std::string& targetFilePath) {
    initTmpDir();
    size_t lastSlash = targetFilePath.find_last_of("/\\");
    std::string filename = (lastSlash == std::string::npos) ? targetFilePath : targetFilePath.substr(lastSlash + 1);
#if defined(_WIN32)
    return FilePaths::TMP_DIR + "/" + filename + "." + std::to_string(GetCurrentProcessId()) + ".tmp";
#else
    return FilePaths::TMP_DIR + "/" + filename + "." + std::to_string(getpid()) + ".tmp";
#endif
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

#if defined(_WIN32)
    std::string intermediateTarget = targetPath + "." + std::to_string(GetCurrentProcessId()) + ".ssd_swap";
#else
    std::string intermediateTarget = targetPath + "." + std::to_string(getpid()) + ".ssd_swap";
#endif
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
    //tim path bang cac luot qua tat ca 
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
