/**
 * @file fileio.h
 * Thanh vien phu trach: Trần Đức Anh (B24DCVT021)
 * Hoc phan: Ky thuat lap trinh (KTLT) - PTIT
 *
 * file nay chiu trach nhiem vao ra du lieu cho he thong:
 * - split: tach chuoi theo dau '|', giu cot rong de bat loi thieu du lieu
 * - luu file an toan (atomic): ghi vao tmps truoc roi moi doi ten thay file cu
 * - khac biet he dieu hanh (Linux vs Windows):
 *   + Linux: tao symlink tmps sang /dev/shm (RAM tmpfs) de chay test khong ton SSD;
 *     dung getpid(), rename() va copy qua file trung gian neu lech phan vung.
 *   + Windows: khong co /dev/shm nen tao folder tmps truc tiep bang _mkdir();
 *     dung GetCurrentProcessId(), dung MoveFileExA voi co ghi de (REPLACE_EXISTING)
 *     va ghi thang xuong dia (WRITE_THROUGH).
 * - tu tim thu muc data: chay o thu muc nao cung doc duoc 9 file data
 */

#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <vector>

//dia chi cua data, tmps la file tmps trong ram
namespace FilePaths {
    const std::string HOPDONG_DATA = "data/hopdong.txt";
    const std::string IMEI_DATA = "data/imei.txt";
    const std::string KHACHHANG_DATA = "data/khachhang.txt";
    const std::string GOICUOC_DATA = "data/CacGoiCuoc.txt";
    const std::string THUEBAO_DATA = "data/thuebao.txt";
    const std::string HOADON_DATA = "data/hoadon.txt";
    const std::string NAPTIEN_DATA = "data/naptien.txt";
    const std::string GHICUOCGOI_DATA = "data/ghicuocgoi.txt";
    const std::string KHIEUNAI_DATA = "data/khieunai.txt";
    const std::string TMP_DIR = "tmps";
}

namespace FileConstants {
    const char DEFAULT_DELIMITER = '|';
    const char COMMENT_PREFIX = '#';
}

class FileIO {
	public:
    //tach dong du lieu theo ky tu phan cach
    static std::vector<std::string> split(const std::string& s, char delimiter = FileConstants::DEFAULT_DELIMITER);
    //dam bao thu muc cua tep ton tai
    static bool ensureDirectoryExists(const std::string& filepath);
    //khoi tao thu muc luu tep tam thoi
    static bool initTmpDir();
    //tao duong dan tep tam thoi de ghi
    static std::string getTempStagingPath(const std::string& targetFilePath);
    //chuyen tep tam thanh tep dich
    static bool commitFromTmp(const std::string& tmpPath, const std::string& targetPath);
    //kiem tra tep tam co nam tren tmpfs hya ko
    static bool isUsingTmpfs();
    //tim thu muc data day du cua chuong trinh
    static std::string resolveDataDirectory(const std::string& executablePath);
    //noi thu muc va ten tep thanh duong dan
    static std::string joinPath(const std::string& directory, const std::string& filename);
};

#endif // FILEIO_H
