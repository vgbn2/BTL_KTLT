#ifndef KHIEUNAI_H
#define KHIEUNAI_H

#include <string>
#include "Entity.h"

namespace KhieuNaiConstants {
    const std::string TYPE_BILLING = "CuocPhatSinh";
    const std::string TYPE_NETWORK = "ChatLuongMang";
    const std::string TYPE_CARD = "TheCao";
    const std::string TYPE_SERVICE = "DichVuGoi";

    const std::string STATUS_RECEIVED = "TiepNhan";
    const std::string STATUS_PROCESSING = "DangXuLy";
    const std::string STATUS_RESOLVED = "DaGiaiQuyet";
    const std::string STATUS_REJECTED = "TuChoi";

    const int COL_WIDTH_ID = 12;
    const int COL_WIDTH_PHONE = 15;
    const int COL_WIDTH_TYPE = 16;
    const int COL_WIDTH_DATE = 14;
    const int COL_WIDTH_STATUS = 14;
    const int COL_WIDTH_CONTENT = 30;
}

class KhieuNai : public Entity {
private:
    std::string soDienThoai;
    std::string loaiKhieuNai;
    std::string ngayTiepNhan;
    std::string noiDung;
    std::string trangThai;

public:
    //tao phieu khieu nai mac dinh
    KhieuNai();
    //tao phieu khieu nai day du thong tin
    KhieuNai(
        const std::string& maKN,
        const std::string& sdt,
        const std::string& loai,
        const std::string& ngay,
        const std::string& content,
        const std::string& status = KhieuNaiConstants::STATUS_RECEIVED
    );

    //lay ma phieu khieu nai
    std::string getMaKhieuNai() const { return id; }
    //lay so dien thoai phan anh
    std::string getSoDienThoai() const { return soDienThoai; }
    //lay loai khieu nai
    std::string getLoaiKhieuNai() const { return loaiKhieuNai; }
    //lay ngay tiep nhan
    std::string getNgayTiepNhan() const { return ngayTiepNhan; }
    //lay noi dung phan anh
    std::string getNoiDung() const { return noiDung; }
    //lay trang thai xu ly
    std::string getTrangThai() const { return trangThai; }

    //cap nhat trang thai phieu khieu nai
    void setTrangThai(const std::string& status);

    //in tieu de danh sach khieu nai
    void displayHeader(std::ostream& os = std::cout) const override;
    //in mot dong khieu nai
    void displayRow(std::ostream& os = std::cout) const override;
    //in chi tiet khieu nai
    void displayDetail(std::ostream& os = std::cout) const override;

    //chuyen khieu nai thanh dong tep
    std::string toFileString() const override;
    //nap khieu nai tu dong tep
    bool fromFileString(const std::string& line) override;
};

#endif // KHIEUNAI_H
