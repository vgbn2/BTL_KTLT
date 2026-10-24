#include "common.h"
#include "Normalized.h"
#include "imei.h"
#include "hopdong.h"
#include "khachhang.h"
#include "goicuoc.h"
#include "thuebao.h"
#include "hoadon.h"
#include "naptien.h"
#include "ghicuocgoi.h"
#include "khieunai.h"
#include "fileio.h"
#include "Exceptions.h"

void testIMEILuhn() {
    std::cout << "[RUN] Kiem thu ThietBiIMEI va thuat toan Luhn Checksum 3GPP...\n";

    const std::string VALID_IMEI_1 = "860123456789014";
    const std::string VALID_IMEI_2 = "351234567890124";
    const std::string VALID_IMEI_3 = "861987654321096";

    ASSERT_TRUE(ThietBiIMEI::validateLuhn(VALID_IMEI_1), "VALID_IMEI_1 phai hop le");
    ASSERT_TRUE(ThietBiIMEI::validateLuhn(VALID_IMEI_2), "VALID_IMEI_2 phai hop le");
    ASSERT_TRUE(ThietBiIMEI::validateLuhn(VALID_IMEI_3), "VALID_IMEI_3 phai hop le");

    const std::string INVALID_CHECKSUM = "860123456789011";
    const std::string INVALID_LENGTH_SHORT = "86012345678901";
    const std::string INVALID_LENGTH_LONG = "8601234567890145";
    const std::string INVALID_ALPHA = "86012345678901A";
    const std::string INVALID_ZEROS = "000000000000000";

    ASSERT_TRUE(!ThietBiIMEI::validateLuhn(INVALID_CHECKSUM), "INVALID_CHECKSUM phai bi tu choi");
    ASSERT_TRUE(!ThietBiIMEI::validateLuhn(INVALID_LENGTH_SHORT), "Short length phai bi tu choi");
    ASSERT_TRUE(!ThietBiIMEI::validateLuhn(INVALID_LENGTH_LONG), "Long length phai bi tu choi");
    ASSERT_TRUE(!ThietBiIMEI::validateLuhn(INVALID_ALPHA), "Alpha char phai bi tu choi");
    ASSERT_TRUE(!ThietBiIMEI::validateLuhn(INVALID_ZEROS), "All zeros phai bi tu choi");

    ThietBiIMEI tb(VALID_IMEI_1, "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    ASSERT_TRUE(tb.getId() == VALID_IMEI_1, "IMEI getId phai khop");
    ASSERT_TRUE(!tb.isBlacklisted(), "Thiet bi moi khong duoc o trang thai Blacklist");

    tb.setBlacklist(true);
    ASSERT_TRUE(tb.isBlacklisted(), "setBlacklist true phai chuyen thanh KhoaMang");

    tb.setBlacklist(false);
    ASSERT_TRUE(!tb.isBlacklisted(), "setBlacklist false phai chuyen thanh HoatDong");

    bool threwBadDeviceStatus = false;
    try {
        tb.setTrangThai("FakeStatus");
    } catch (const AppException&) {
        threwBadDeviceStatus = true;
    }
    ASSERT_TRUE(threwBadDeviceStatus, "Trang thai thiet bi sai phai bi tu choi");

    tb.ganSIM("0912345678");
    ASSERT_TRUE(tb.getSoDienThoai() == "0912345678", "ganSIM phai cap nhat sdt hop le");

    //kiem tra sdt thiet bi imei khong hop le
    const std::vector<std::string> invalidTbPhones = {"012345", "0241234567", "09812345678", "098123456A"};
    for (const auto& bad : invalidTbPhones) {
        bool threwCtor = false;
        try {
            ThietBiIMEI badTb(VALID_IMEI_1, "iPhone 15", "Apple", bad, Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
        } catch (const InvalidPhoneNumberException&) {
            threwCtor = true;
        }
        ASSERT_TRUE(threwCtor, "ThietBiIMEI constructor phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwGanSIM = false;
        try {
            tb.ganSIM(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwGanSIM = true;
        }
        ASSERT_TRUE(threwGanSIM, "ThietBiIMEI ganSIM phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwSetPhone = false;
        try {
            tb.setSoDienThoai(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwSetPhone = true;
        }
        ASSERT_TRUE(threwSetPhone, "ThietBiIMEI setSoDienThoai phai nem InvalidPhoneNumberException voi sdt: " + bad);
    }

    tb.setSoDienThoai("0987654321");
    ASSERT_TRUE(tb.getSoDienThoai() == "0987654321", "ThietBiIMEI setSoDienThoai hop le thanh cong");

    ThietBiIMEI tbUnassigned(VALID_IMEI_1, "iPhone 15", "Apple", "ChuaGan", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    ASSERT_TRUE(tbUnassigned.getSoDienThoai() == "ChuaGan", "ThietBiIMEI constructor voi ChuaGan phai hop le");

    ThietBiIMEI tbEmptyPhone(VALID_IMEI_1, "iPhone 15", "Apple", "", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    ASSERT_TRUE(tbEmptyPhone.getSoDienThoai().empty(), "ThietBiIMEI constructor voi chuoi rong phai hop le");

    tb.goSIM();
    ASSERT_TRUE(tb.getSoDienThoai() == IMEIConstants::UNASSIGNED_PHONE, "goSIM phai thanh ChuaGan");

    std::string serialized = tb.toFileString();
    ThietBiIMEI deserialized;
    bool ok = deserialized.fromFileString(serialized);
    ASSERT_TRUE(ok, "Deserialization ThietBiIMEI phai thanh cong");
    ASSERT_TRUE(deserialized.getId() == VALID_IMEI_1 && deserialized.getHangSanXuat() == "Apple" && deserialized.getSoDienThoai() == IMEIConstants::UNASSIGNED_PHONE,
                "Deserialized id, hang sx va sdt goSIM phai khop");
    ASSERT_TRUE(deserialized.getTenThietBi() == "iPhone 15" && deserialized.getTrangThai() == tb.getTrangThai() && deserialized.getTramBTSGanNhat() == "BTS-HN-001",
                "Deserialized ten, trang thai va BTS ID phai khop toan ven");
}

void testHopDong() {
    std::cout << "[RUN] Kiem thu HopDong va logic thoi han...\n";

    Date start(1, 1, 2024);
    Date end(1, 1, 2025);
    HopDong hd("HD9999", "KH9999", "0981234567", "GC999", start, end, "TraSau", "HieuLuc", 100000.0);

    ASSERT_TRUE(hd.getId() == "HD9999", "HopDong getId phai khop");
    ASSERT_TRUE(!hd.isExpired(Date(1, 6, 2024)), "HopDong khong the expired vao thang 6/2024");
    ASSERT_TRUE(hd.isExpired(Date(2, 1, 2025)), "HopDong phai expired vao 02/01/2025");

    //kiem tra sdt hop dong khong hop le
    const std::vector<std::string> invalidHdPhones = {"12345", "0123456789", "0241234567", "09812345678", "098123456A"};
    for (const auto& bad : invalidHdPhones) {
        bool threwCtor = false;
        try {
            HopDong badPhone("HD8888", "KH8888", bad, "GC001", start, end, "TraSau", "HieuLuc", 100.0);
        } catch (const InvalidPhoneNumberException&) {
            threwCtor = true;
        }
        ASSERT_TRUE(threwCtor, "HopDong constructor phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwMutator = false;
        try {
            hd.setSoDienThoai(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwMutator = true;
        }
        ASSERT_TRUE(threwMutator, "HopDong setSoDienThoai phai nem InvalidPhoneNumberException voi sdt: " + bad);
    }

    hd.setSoDienThoai("0912345678");
    ASSERT_TRUE(hd.getSoDienThoai() == "0912345678", "HopDong setSoDienThoai hop le thanh cong");
    hd.setSoDienThoai("0981234567");

    Date newEnd(1, 1, 2026);
    hd.giaHan(newEnd);
    ASSERT_TRUE(hd.getNgayHetHan() == newEnd, "Gia han hop dong thanh cong");

    bool threwNegPrice = false;
    try {
        HopDong badPrice("HD8887", "KH8887", "0981234567", "GC001", start, end, "TraSau", "HieuLuc", -50000.0);
    } catch (const AppException&) {
        threwNegPrice = true;
    }
    ASSERT_TRUE(threwNegPrice, "Gia tri goi am phai bi tu choi");

    bool threwBadType = false;
    try {
        HopDong badType("HD8886", "KH8886", "0981234567", "GC001", start, end, "InvalidType", "HieuLuc", 1000.0);
    } catch (const AppException&) {
        threwBadType = true;
    }
    ASSERT_TRUE(threwBadType, "Loai hop dong sai phai bi tu choi");

    bool threwBadStatus = false;
    try {
        HopDong badStatus("HD8885", "KH8885", "0981234567", "GC001", start, end, "TraSau", "InvalidStatus", 1000.0);
    } catch (const AppException&) {
        threwBadStatus = true;
    }
    ASSERT_TRUE(threwBadStatus, "Trang thai hop dong sai phai bi tu choi");

    hd.chamDut();
    ASSERT_TRUE(hd.getTrangThai() == HopDongConstants::STATUS_TERMINATED, "chamDut phai thanh ThanhLy");

    std::string s = hd.toFileString();
    HopDong dHd;
    bool ok = dHd.fromFileString(s);
    ASSERT_TRUE(ok, "Deserialization HopDong phai thanh cong");
    ASSERT_TRUE(dHd.getId() == "HD9999", "Deserialized HopDong id phai khop");
    ASSERT_TRUE(dHd.getGiaTriGoi() == 100000.0, "Deserialized HopDong giaTri phai khop");
}

void testKhachHangModel() {
    std::cout << "[RUN] Kiem thu KhachHang model va validation...\n";

    KhachHang kh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    ASSERT_TRUE(kh.getMaKhachHang() == "KH0001", "Ma KH phai la KH0001");
    ASSERT_TRUE(kh.getHoTen() == "Nguyen Van An", "Ho ten phai khop");
    ASSERT_TRUE(kh.getLoaiGiayTo() == "CCCD", "Loai giay to phai la CCCD");
    ASSERT_TRUE(kh.getSoGiayTo() == "001201004567", "So giay to phai khop");
    ASSERT_TRUE(kh.getSoDienThoai() == "0981234567", "So dien thoai phai khop");

    bool caughtLongName = false;
    try {
        KhachHang tooLong("KH0002", std::string(51, 'A'), "CCCD", "001201004567",
                          "01/01/2021", "C06", "0981234567", "HN");
    } catch (const AppException&) {
        caughtLongName = true;
    }
    ASSERT_TRUE(caughtLongName, "KhachHang phai tu choi ho ten qua 50 ky tu");
    ASSERT_TRUE(Normalized::isValidName(std::string(50, 'A')), "Ten dung 50 ky tu phai hop le");
    ASSERT_TRUE(!Normalized::isValidName(std::string(51, 'A')), "Ten 51 ky tu phai bi tu choi");
    ASSERT_TRUE(Normalized::isValidName("Nguyen Van An"), "Ten hop le phai duoc chap nhan");
    ASSERT_TRUE(!Normalized::isValidName("   "), "Ten chi co khoang trang phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidName(std::string("A\xC3\x28")), "Ten UTF-8 loi phai bi tu choi");

    //kiem tra loi sdt
    bool caughtPhone = false;
    try {
        KhachHang khErr("KH0002", "Tran Binh", "CCCD", "001201004567", "01/01/2021", "C06", "012345", "HN");
    } catch (const InvalidPhoneNumberException& e) {
        caughtPhone = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtPhone, "KhachHang voi SDT khong hop le phai quang InvalidPhoneNumberException");

    //kiem tra serialize
    std::string fileStr = kh.toFileString();
    ASSERT_TRUE(fileStr == "KH0001|Nguyen Van An|CCCD|001201004567|10/01/2021|Cuc C06|0981234567|Ha Noi", "toFileString phai dung format pipe-delimited");

    KhachHang khParsed;
    ASSERT_TRUE(khParsed.fromFileString(fileStr), "fromFileString phai parse thanh cong");
    ASSERT_TRUE(khParsed.getMaKhachHang() == "KH0001", "Parsed Ma KH phai khop");
    ASSERT_TRUE(khParsed.getHoTen() == "Nguyen Van An", "Parsed Ho Ten phai khop");
    ASSERT_TRUE(!khParsed.fromFileString("INVALID|LINE") && !khParsed.fromFileString("KH0001|Name|CCCD|001201004567|01/01/2021|C06|0981234567") && !khParsed.fromFileString("KH0001|Name|CCCD|001201004567|01/01/2021|C06|0981234567|HN|Extra"),
                "fromFileString phai tu choi dong thieu truong (7), thua truong (9), hoac line sai (2)");

    //kiem tra display voi StreamRedirectGuard
    std::ostringstream oss;
    {
        StreamRedirectGuard guard;
        std::cout.rdbuf(oss.rdbuf());
        kh.displayHeader();
        kh.displayRow();
        kh.displayDetail();
    }
    std::string out = oss.str();
    ASSERT_TRUE(out.find("KH0001") != std::string::npos, "Display output phai chua KH0001");
}

void testGoiCuocModel() {
    std::cout << "[RUN] Kiem thu GoiCuoc model va nghiep vu goi...\n";

    GoiCuoc gc1("M50", "Mobi 50", 50000.0, 5, 100, 30);
    GoiCuoc gc2("VD140", "VinaData 140", 140000.0, 30, 300, 30);

    ASSERT_TRUE(gc1.getMaGoi() == "M50", "Ma goi phai la M50");
    ASSERT_TRUE(gc1.getTenGoi() == "Mobi 50", "Ten goi phai la Mobi 50");
    ASSERT_TRUE(gc1.getGiaCuoc() == 50000.0, "Gia cuoc phai la 50000");
    ASSERT_TRUE(gc1.getDungLuongData() == 5, "Data phai la 5 GB");
    ASSERT_TRUE(gc1.getSoPhutGoi() == 100, "Thoai phai la 100 phut");
    ASSERT_TRUE(gc1.getChuKyNgay() == 30, "Chu ky phai la 30 ngay");

    //kiem tra loi validate
    bool caughtPrice = false;
    try {
        GoiCuoc gcErr("ERR", "Loi", -1000.0, 5, 100, 30);
    } catch (const AppException& e) {
        caughtPrice = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtPrice, "Gia cuoc am phai quang AppException");

    //kiem tra serialize
    std::string fileStr = gc1.toFileString();
    ASSERT_TRUE(fileStr == "M50|Mobi 50|50000|5|100|30", "toFileString GoiCuoc phai dung format");

    GoiCuoc parsedGc;
    ASSERT_TRUE(parsedGc.fromFileString(fileStr), "fromFileString GoiCuoc phai parse thanh cong");
    ASSERT_TRUE(parsedGc.getMaGoi() == "M50", "Parsed ma goi phai khop");
    ASSERT_TRUE(parsedGc.getGiaCuoc() == 50000.0, "Parsed gia cuoc phai khop");
}

void testThueBaoModel() {
    std::cout << "[RUN] Kiem thu ThueBao model va giao dich tai khoan...\n";

    ThueBao tb("0981234567", ThueBaoConstants::TYPE_PREPAID, "KH0001", "M50", "15/01/2024", ThueBaoConstants::STATUS_ACTIVE, 100000.0);
    ASSERT_TRUE(tb.getSoDienThoai() == "0981234567", "SDT phai khop");
    ASSERT_TRUE(tb.getLoaiThueBao() == ThueBaoConstants::TYPE_PREPAID, "Loai TB phai la TraTruoc");
    ASSERT_TRUE(tb.getSoDuTaiKhoan() == 100000.0, "So du ban dau phai la 100000");

    //nap tien
    tb.congTien(50000.0);
    ASSERT_TRUE(tb.getSoDuTaiKhoan() == 150000.0, "So du sau nap 50000 phai la 150000");

    //tru tien
    tb.truTien(100000.0);
    ASSERT_TRUE(tb.getSoDuTaiKhoan() == 50000.0, "So du sau tru 100000 phai la 50000");

    //tru tien vuot so du
    bool caughtOverdraft = false;
    try {
        tb.truTien(60000.0);
    } catch (const AppException& e) {
        caughtOverdraft = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtOverdraft, "Tru tien qua so du thue bao tra truoc phai quang AppException");

    //kiem tra loi sdt
    bool caughtPhone = false;
    try {
        ThueBao tbErr("012345", "TraTruoc", "KH0001", "M50", "01/01/2024");
    } catch (const InvalidPhoneNumberException& e) {
        caughtPhone = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtPhone, "SDT sai quy chuan phai quang InvalidPhoneNumberException");

    //kiem tra serialize
    std::string tbStr = tb.toFileString();
    ASSERT_TRUE(tbStr.find("0981234567|TraTruoc|KH0001|M50|15/01/2024|HoatDong|50000") != std::string::npos, "toFileString ThueBao phai dung format");

    ThueBao parsedTb;
    ASSERT_TRUE(parsedTb.fromFileString(tbStr), "fromFileString ThueBao phai parse thanh cong");
    ASSERT_TRUE(parsedTb.getSoDienThoai() == "0981234567", "Parsed SDT phai khop");
    ASSERT_TRUE(parsedTb.getSoDuTaiKhoan() == 50000.0, "Parsed so du phai khop");
}

void testHoaDonAndNapTienModels() {
    std::cout << "[RUN] Kiem thu HoaDon va NapTien models...\n";

    HoaDon hd("HD01", "0981234567", "01/2024", 50000.0, 100000.0, 150000.0, HoaDonConstants::STATUS_UNPAID);
    ASSERT_TRUE(hd.quaHan(Date(1, 2, 2024)), "Hoa don thang truoc phai qua han");
    ASSERT_TRUE(!hd.quaHan(Date(1, 1, 2024)), "Hoa don cung thang khong qua han");
    ASSERT_TRUE(hd.getMaHoaDon() == "HD01", "Ma HD phai la HD01");
    ASSERT_TRUE(hd.getTongTien() == 150000.0, "Tong tien phai la 150000");
    ASSERT_TRUE(hd.getTrangThai() == HoaDonConstants::STATUS_UNPAID, "Trang thai phai la ChuaThanhToan");

    hd.thanhToan();
    ASSERT_TRUE(hd.getTrangThai() == HoaDonConstants::STATUS_PAID, "Sau thanh toan phai la DaThanhToan");

    std::string hdStr = hd.toFileString();
    ASSERT_TRUE(hdStr == "HD01|0981234567|01/2024|50000|100000|150000|DaThanhToan", "HoaDon toFileString phai dung");

    HoaDon parsedHd;
    ASSERT_TRUE(parsedHd.fromFileString(hdStr), "HoaDon fromFileString phai thanh cong");
    ASSERT_TRUE(parsedHd.getTongTien() == 150000.0, "Parsed tong tien phai khop");

    NapTien nt("GD0001", "0981234567", "15/01/2024", 200000.0, NapTienConstants::METHOD_BANKING, NapTienConstants::STATUS_SUCCESS);
    ASSERT_TRUE(nt.getMaGiaoDich() == "GD0001", "Ma GD phai la GD0001");
    ASSERT_TRUE(nt.getSoTien() == 200000.0, "So tien nap phai la 200000");

    bool caughtAmnt = false;
    try {
        NapTien ntErr("GD0002", "0981234567", "15/01/2024", -5000.0);
    } catch (const AppException& e) {
        caughtAmnt = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtAmnt, "Nap tien voi so tien am phai quang AppException");

    std::string ntStr = nt.toFileString();
    ASSERT_TRUE(ntStr == "GD0001|0981234567|15/01/2024|200000|NganHang|ThanhCong", "NapTien toFileString phai dung");

    NapTien parsedNt;
    ASSERT_TRUE(parsedNt.fromFileString(ntStr), "NapTien fromFileString phai thanh cong");
    ASSERT_TRUE(parsedNt.getSoTien() == 200000.0, "Parsed so tien nap phai khop");
}

void testCDRAndKhieuNaiModels() {
    std::cout << "[RUN] Kiem thu CDR va KhieuNai models...\n";
    ASSERT_TRUE(FilePaths::GHICUOCGOI_DATA == "data/ghicuocgoi.txt", "Duong dan ghicuocgoi phai dung ten file moi");
    std::ifstream ghicuocgoiFixture(FilePaths::GHICUOCGOI_DATA);
    ASSERT_TRUE(ghicuocgoiFixture.good() && ghicuocgoiFixture.peek() != std::ifstream::traits_type::eof(), "Tep du lieu ghicuocgoi phai ton tai va co du lieu ban ghi");

    // Test CDR
    CDR cdr("CDR0001", "0981234567", "0912345678", "Thoai", "15/01/2024 08:30:00", 120, 2000.0);
    ASSERT_TRUE(cdr.getMaCDR() == "CDR0001", "Ma CDR phai khop");
    ASSERT_TRUE(cdr.getSoGoi() == "0981234567", "So goi CDR phai khop");
    ASSERT_TRUE(cdr.getSoNhan() == "0912345678", "So nhan CDR phai khop");
    ASSERT_TRUE(cdr.getLoaiDichVu() == "Thoai", "Loai dich vu CDR phai khop");
    ASSERT_TRUE(cdr.getThoiLuong() == 120, "Thoi luong CDR phai khop");
    ASSERT_TRUE(cdr.getCuocPhatSinh() == 2000.0, "Cuoc phat sinh CDR phai khop");

    std::string cdrStr = cdr.toFileString();
    ASSERT_TRUE(cdrStr.find("CDR0001") != std::string::npos, "toFileString CDR phai chua Ma CDR");

    CDR parsedCdr;
    ASSERT_TRUE(parsedCdr.fromFileString(cdrStr), "fromFileString CDR phai thanh cong");
    ASSERT_TRUE(parsedCdr.getCuocPhatSinh() == 2000.0, "Parsed cuoc phat sinh phai khop");

    const std::vector<std::string> invalidCdrRows = {
        cdrStr + "|extra",
        "CDR0002|0981234567|0912345678|Khac|15/01/2024 08:30:00|120|2000",
        "CDR0003|012345|0912345678|Thoai|15/01/2024 08:30:00|120|2000",
        "CDR0004|0981234567|0912345678|Thoai|31/02/2024 08:30:00|120|2000",
        "CDR0005|0981234567|0912345678|Thoai|15/01/2024 08:30:00|120|nan",
        "CDR0006|0981234567|0912345678|Data|15/01/2024 08:30:00|120|2000"
    };
    for (const auto& invalidRow : invalidCdrRows) {
        CDR rejected;
        ASSERT_TRUE(!rejected.fromFileString(invalidRow),
                    "fromFileString CDR phai tu choi dong du lieu khong hop le");
    }

    bool threwCdrDuration = false;
    try {
        CDR badCdr("CDR0002", "0981234567", "0912345678", "Thoai", "15/01/2024", -10, 1000.0);
    } catch (const AppException&) {
        threwCdrDuration = true;
    }
    ASSERT_TRUE(threwCdrDuration, "Thoi luong am phai nem AppException");

    bool threwCdrCost = false;
    try {
        CDR badCdr("CDR0003", "0981234567", "0912345678", "Thoai", "15/01/2024", 60, -500.0);
    } catch (const AppException&) {
        threwCdrCost = true;
    }
    ASSERT_TRUE(threwCdrCost, "Cuoc am phai nem AppException");

    // Test KhieuNai
    KhieuNai kn("KN0001", "0912345678", "CuocPhatSinh", "02/03/2024", "Cuoc data tang cao", "TiepNhan");
    ASSERT_TRUE(kn.getMaKhieuNai() == "KN0001", "Ma khieu nai phai khop");
    ASSERT_TRUE(kn.getSoDienThoai() == "0912345678", "SĐT khieu nai phai khop");
    ASSERT_TRUE(kn.getLoaiKhieuNai() == "CuocPhatSinh", "Loai khieu nai phai khop");
    ASSERT_TRUE(kn.getTrangThai() == "TiepNhan", "Trang thai khieu nai ban dau phai la TiepNhan");

    kn.setTrangThai("DaGiaiQuyet");
    ASSERT_TRUE(kn.getTrangThai() == "DaGiaiQuyet", "setTrangThai phai cap nhat dung");

    std::string knStr = kn.toFileString();
    ASSERT_TRUE(knStr.find("KN0001") != std::string::npos, "toFileString KhieuNai phai chua Ma KN");

    KhieuNai parsedKn;
    ASSERT_TRUE(parsedKn.fromFileString(knStr), "fromFileString KhieuNai phai thanh cong");
    ASSERT_TRUE(parsedKn.getTrangThai() == "DaGiaiQuyet", "Parsed trang thai khieu nai phai khop");

    bool threwKnPhone = false;
    try {
        KhieuNai badKn("KN0002", "012345", "CuocPhatSinh", "02/03/2024", "Loi", "TiepNhan");
    } catch (const InvalidPhoneNumberException&) {
        threwKnPhone = true;
    }
    ASSERT_TRUE(threwKnPhone, "SĐT khong hop le phai nem InvalidPhoneNumberException");

    // VAL-01: fromFileString phai kiem tra chinh xac 6 truong (khong it hon, khong nhieu hon).
    KhieuNai kn2;
    ASSERT_TRUE(!kn2.fromFileString("KN0001|0912345678|CuocPhatSinh|02/03/2024|Noi dung"),
                "fromFileString 5 truong phai return false");
    ASSERT_TRUE(!kn2.fromFileString("KN0001|0912345678|CuocPhatSinh|02/03/2024|Noi dung|TiepNhan|TruongThua"),
                "fromFileString 7 truong phai return false");
    ASSERT_TRUE(!kn2.fromFileString("KN0001|012BAD|CuocPhatSinh|02/03/2024|Noi dung|TiepNhan"),
                "fromFileString so dien thoai sai phai return false");
    ASSERT_TRUE(!kn2.fromFileString("||CuocPhatSinh|02/03/2024|Noi dung|TiepNhan"),
                "fromFileString ma phieu rong phai return false");
    // tat ca 4 loai va 3 trang thai chinh thuc phai duoc chap nhan.
    KhieuNai kn3;
    ASSERT_TRUE(kn3.fromFileString("KN0011|0912345678|CuocPhatSinh|01/01/2024|nd|TiepNhan"),   "loai CuocPhatSinh phai hop le");
    ASSERT_TRUE(kn3.fromFileString("KN0012|0912345678|ChatLuongMang|01/01/2024|nd|DangXuLy"),  "loai ChatLuongMang phai hop le");
    ASSERT_TRUE(kn3.fromFileString("KN0013|0912345678|TheCao|01/01/2024|nd|DaGiaiQuyet"),      "loai TheCao phai hop le");
    ASSERT_TRUE(kn3.fromFileString("KN0014|0912345678|DichVuGoi|01/01/2024|nd|TuChoi"),        "loai DichVuGoi phai hop le");
}

