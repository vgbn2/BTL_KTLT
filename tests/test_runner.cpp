#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cstdio>
#include <fstream>
#include "Date.h"
#include "hopdong.h"
#include "imei.h"
#include "Repository.h"
#include "Exceptions.h"
#include "InputHelper.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include "HopDongMenu.h"
#include "ThietBiIMEIMenu.h"
#include "khachhang.h"
#include "goicuoc.h"
#include "thuebao.h"
#include "hoadon.h"
#include "naptien.h"
#include "GoiCuocMenu.h"
#include "ThueBaoMenu.h"
#include "KhachHangMenu.h"
#include "NapTienMenu.h"
#include "HoaDonMenu.h"
#include "ghicuocgoi.h"
#include "khieunai.h"
#include "BTSRegister.h"
#include <sstream>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace TestStats {
    int passed = 0;
    int failed = 0;
}

// ponytail: RAII guard ensures cin/cout buffers are restored even during test assertion failures or unexpected exceptions
struct StreamRedirectGuard {
    std::streambuf* origCin;
    std::streambuf* origCout;
    StreamRedirectGuard()
        : origCin(std::cin.rdbuf()), origCout(std::cout.rdbuf()) {}
    ~StreamRedirectGuard() {
        std::cin.rdbuf(origCin);
        std::cout.rdbuf(origCout);
        std::cin.clear();
    }
    StreamRedirectGuard(const StreamRedirectGuard&) = delete;
    StreamRedirectGuard& operator=(const StreamRedirectGuard&) = delete;
};

#define TEST_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            TestStats::passed++; \
        } else { \
            TestStats::failed++; \
            std::cerr << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        } \
    } while (0)

void testDate() {
    std::cout << "[RUN] Kiem thu Date va Leap Year...\n";

    TEST_ASSERT(Date::isLeapYear(2000), "Nam 2000 phai la nam nhuan");
    TEST_ASSERT(Date::isLeapYear(2024), "Nam 2024 phai la nam nhuan");
    TEST_ASSERT(!Date::isLeapYear(1900), "Nam 1900 khong phai la nam nhuan");
    TEST_ASSERT(!Date::isLeapYear(2023), "Nam 2023 khong phai la nam nhuan");

    TEST_ASSERT(Date::isValid(29, 2, 2024), "29/02/2024 phai hop le");
    TEST_ASSERT(!Date::isValid(29, 2, 2023), "29/02/2023 khong the hop le");
    TEST_ASSERT(!Date::isValid(31, 4, 2024), "31/04/2024 khong the hop le");
    TEST_ASSERT(Date::isValid(31, 12, 2024), "31/12/2024 phai hop le");

    Date d1(15, 1, 2024);
    Date d2(15, 1, 2025);
    Date d3(15, 1, 2024);

    TEST_ASSERT(d1 < d2, "d1 phai nho hon d2");
    TEST_ASSERT(d1 <= d3, "d1 phai <= d3");
    TEST_ASSERT(d1 == d3, "d1 phai == d3");
    TEST_ASSERT(d2 > d1, "d2 phai lon hon d1");

    Date parsed = Date::parse("05/04/2024");
    TEST_ASSERT(parsed.getDay() == 5 && parsed.getMonth() == 4 && parsed.getYear() == 2024, "Parse Date sai gia tri");
    TEST_ASSERT(parsed.toString() == "05/04/2024", "Date toString sai format");

    bool caughtTrailingDate = false;
    try {
        Date::parse("15/01/2024abc");
    } catch (const InvalidDateException& e) {
        caughtTrailingDate = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtTrailingDate, "Date::parse voi chuoi co ky tu thua phai quang InvalidDateException");

    bool caughtMixedSep = false;
    try {
        Date::parse("15/01-2024");
    } catch (const InvalidDateException& e) {
        caughtMixedSep = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtMixedSep, "Date::parse voi dau phan cach khong dong nhat phai quang InvalidDateException");
}

void testBirthDateAndAgeValidation() {
    std::cout << "[RUN] Kiem thu Ngay sinh (Birth Date) va Do tuoi chu thue bao...\n";

    Date referenceDate(15, 10, 2024);

    Date validBirth(20, 5, 2000);
    TEST_ASSERT(validBirth.calculateAge(referenceDate) == 24, "Nguoi sinh 20/05/2000 den 15/10/2024 phai 24 tuoi");
    TEST_ASSERT(Date::isValidBirthDate(validBirth, 14, 120, referenceDate), "Nguoi 24 tuoi phai hop le");

    Date youngValid(15, 10, 2010);
    TEST_ASSERT(youngValid.calculateAge(referenceDate) == 14, "Nguoi sinh 15/10/2010 den 15/10/2024 dung 14 tuoi");
    TEST_ASSERT(Date::isValidBirthDate(youngValid, 14, 120, referenceDate), "Dung 14 tuoi phai duoc dang ky thue bao");

    Date tooYoung(16, 10, 2010);
    TEST_ASSERT(tooYoung.calculateAge(referenceDate) == 13, "Nguoi sinh 16/10/2010 den 15/10/2024 la 13 tuoi (chua den sinh nhat)");
    TEST_ASSERT(!Date::isValidBirthDate(tooYoung, 14, 120, referenceDate), "13 tuoi phai bi tu choi");

    Date futureDate(1, 1, 2025);
    TEST_ASSERT(futureDate.isFuture(referenceDate), "Ngay 01/01/2025 phai la tuong lai so voi 15/10/2024");
    TEST_ASSERT(!Date::isValidBirthDate(futureDate, 14, 120, referenceDate), "Ngay sinh trong tuong lai phai bi tu choi");

    Date tooOld(1, 1, 1901);
    TEST_ASSERT(!Date::isValidBirthDate(tooOld, 14, 120, referenceDate), "Tuoi > 120 phai bi tu choi");

    bool threwInvalidYear = false;
    try {
        Date invalidYear(1, 1, 1850);
    } catch (const InvalidDateException&) {
        threwInvalidYear = true;
    }
    TEST_ASSERT(threwInvalidYear, "Nam < 1900 phai nem InvalidDateException");
}

void testPhoneNumberValidation() {
    std::cout << "[RUN] Kiem thu So dien thoai di dong Viet Nam (10 chu so)...\n";

    //kiem tra cac dau so nha mang viet nam
    TEST_ASSERT(Normalized::isValidPhoneNumber("0981234567"), "0981234567 (Viettel) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0912345678"), "0912345678 (VinaPhone) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0903456789"), "0903456789 (MobiFone) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0888123456"), "0888123456 (VinaPhone) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0778901234"), "0778901234 (MobiFone) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0381234567"), "0381234567 (Viettel) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0851234567"), "0851234567 (VinaPhone) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0581234567"), "0581234567 (Vietnamobile) phai hop le");
    TEST_ASSERT(Normalized::isValidPhoneNumber("0591234567"), "0591234567 (Gmobile) phai hop le");

    TEST_ASSERT(!Normalized::isValidPhoneNumber("098123456"), "So 9 chu so phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("09812345678"), "So 11 chu so phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("1981234567"), "Khong bat dau bang 0 phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("8498123456"), "Dau ma quoc gia khong dung chuan 0x phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("0181234567"), "Dau so 01x cu phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("0281234567"), "Dau so co dinh 02x phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("0481234567"), "Dau so 04x phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("0681234567"), "Dau so 06x phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("098123456A"), "Chua ky tu chu cai phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("098 123456"), "Chua khoang trang phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("098-123456"), "Chua dau gach noi phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidPhoneNumber(""), "Chuoi rong phai bi tu choi khi khong cho phep ChuaGan");

    TEST_ASSERT(Normalized::isValidPhoneNumber("ChuaGan", true), "ChuaGan phai hop le khi cho phep unassigned");
    TEST_ASSERT(Normalized::isValidPhoneNumber("", true), "Chuoi rong phai hop le khi cho phep unassigned");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("ChuaGan", false), "ChuaGan khong the hop le khi bat buoc nhap sdt");
    TEST_ASSERT(!Normalized::isValidPhoneNumber("", false), "Chuoi rong khong hop le khi khong cho phep unassigned");

}

void testCustomerIdValidation() {
    std::cout << "[RUN] Kiem thu ma khach hang legacy va mo rong...\n";

    TEST_ASSERT(Normalized::isValidCustomerId("KH0001"), "Ma KH legacy KH0001 phai hop le");
    TEST_ASSERT(Normalized::NormalizedCustomerId(" kh0001 ") == "KH0001",
                "NormalizedCustomerId phai chuan hoa khoang trang va chu thuong");
    TEST_ASSERT(Normalized::isValidCustomerId("KH0000001"), "Ma KH mo rong KH0000001 phai hop le");
    TEST_ASSERT(Normalized::isValidCustomerId("KH9999999"), "Ma KH mo rong KH9999999 phai hop le");
    TEST_ASSERT(!Normalized::isValidCustomerId("KH00001"), "Ma KH 5 chu so phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidCustomerId("KH10000000"), "Ma KH 8 chu so phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidCustomerId("KX0000001"), "Ma KH sai tien to phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidCustomerId("KH00000A1"), "Ma KH chua ky tu chu phai bi tu choi");

    KhachHang expanded("KH0000001", "Nguoi Mo Rong", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    TEST_ASSERT(expanded.getMaKhachHang() == "KH0000001", "KhachHang phai giu ma KH mo rong");

    bool caughtBadId = false;
    try {
        KhachHang invalid("KH1", "Nguoi Sai", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    } catch (const AppException&) {
        caughtBadId = true;
    }
    TEST_ASSERT(caughtBadId, "KhachHang phai tu choi ma KH sai format");
}

void testCredentialValidation() {
    std::cout << "[RUN] Kiem thu CCCD 12 chu so va ma tinh...\n";

    TEST_ASSERT(Normalized::isValidCredential("079085012345"), "CCCD 079085012345 phai hop le");
    TEST_ASSERT(Normalized::isValidCredential("079285012345"), "CCCD 079285012345 phai hop le");
    TEST_ASSERT(!Normalized::isValidCredential("099085012345"), "CCCD voi ma tinh 099 phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidCredential("07908501234"), "CCCD 11 chu so phai bi tu choi");
    TEST_ASSERT(!Normalized::isValidCredential("079A85012345"), "CCCD chua ky tu chu phai bi tu choi");

    bool caughtConstructor = false;
    try {
        KhachHang invalid("KH0000003", "Nguoi Sai", "CCCD", "099085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    } catch (const InvalidCredentialException&) {
        caughtConstructor = true;
    }
    TEST_ASSERT(caughtConstructor, "KhachHang constructor phai tu choi CCCD sai");

    KhachHang kh("KH0000004", "Nguoi Dung", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    bool caughtSetter = false;
    try {
        kh.setSoGiayTo("099085012345");
    } catch (const InvalidCredentialException&) {
        caughtSetter = true;
    }
    TEST_ASSERT(caughtSetter, "KhachHang setter phai tu choi CCCD sai");
    TEST_ASSERT(!kh.fromFileString("KH-BAD|Nguoi Sai|CCCD|099085012345|10/01/2021|Cuc C06|0981234567|Ha Noi"),
                "KhachHang fromFileString phai tu choi CCCD sai");
}

void testNormalizedUtils() {
    std::cout << "[RUN] Kiem thu Normalized string utils...\n";
    TEST_ASSERT(Normalized::trim("  hello world  \t\n") == "hello world", "Normalized::trim phai cat khoang trang hai dau");
    TEST_ASSERT(Normalized::CollapseSpace("  a   b   c  ") == "a b c", "Normalized::CollapseSpace phai thu gon khoang trang");
    TEST_ASSERT(Normalized::removeDigits("Tran 35267 Duc") == "Tran Duc", "removeDigits phai loai bo chu so");
    TEST_ASSERT(Normalized::removeSymbols("Tran* Duc") == "Tran Duc", "removeSymbols phai loai bo ky tu cam");
    TEST_ASSERT(Normalized::NormalizedName("  Nguyen   Van   A!  ") == "Nguyen Van A", "NormalizedName phai chuan hoa ten va loai ky tu cam");
    TEST_ASSERT(Normalized::toUpper("abcDef") == "ABCDEF", "Normalized::toUpper phai viet hoa toan bo");
    TEST_ASSERT(Normalized::toLower("ABCdef") == "abcdef", "Normalized::toLower phai viet thuong toan bo");
}

void testIMEILuhn() {
    std::cout << "[RUN] Kiem thu ThietBiIMEI va thuat toan Luhn Checksum 3GPP...\n";

    const std::string VALID_IMEI_1 = "860123456789014";
    const std::string VALID_IMEI_2 = "351234567890124";
    const std::string VALID_IMEI_3 = "861987654321096";

    TEST_ASSERT(ThietBiIMEI::validateLuhn(VALID_IMEI_1), "VALID_IMEI_1 phai hop le");
    TEST_ASSERT(ThietBiIMEI::validateLuhn(VALID_IMEI_2), "VALID_IMEI_2 phai hop le");
    TEST_ASSERT(ThietBiIMEI::validateLuhn(VALID_IMEI_3), "VALID_IMEI_3 phai hop le");

    const std::string INVALID_CHECKSUM = "860123456789011";
    const std::string INVALID_LENGTH_SHORT = "86012345678901";
    const std::string INVALID_LENGTH_LONG = "8601234567890145";
    const std::string INVALID_ALPHA = "86012345678901A";
    const std::string INVALID_ZEROS = "000000000000000";

    TEST_ASSERT(!ThietBiIMEI::validateLuhn(INVALID_CHECKSUM), "INVALID_CHECKSUM phai bi tu choi");
    TEST_ASSERT(!ThietBiIMEI::validateLuhn(INVALID_LENGTH_SHORT), "Short length phai bi tu choi");
    TEST_ASSERT(!ThietBiIMEI::validateLuhn(INVALID_LENGTH_LONG), "Long length phai bi tu choi");
    TEST_ASSERT(!ThietBiIMEI::validateLuhn(INVALID_ALPHA), "Alpha char phai bi tu choi");
    TEST_ASSERT(!ThietBiIMEI::validateLuhn(INVALID_ZEROS), "All zeros phai bi tu choi");

    ThietBiIMEI tb(VALID_IMEI_1, "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    TEST_ASSERT(tb.getId() == VALID_IMEI_1, "IMEI getId phai khop");
    TEST_ASSERT(!tb.isBlacklisted(), "Thiet bi moi khong duoc o trang thai Blacklist");

    tb.setBlacklist(true);
    TEST_ASSERT(tb.isBlacklisted(), "setBlacklist true phai chuyen thanh KhoaMang");

    tb.setBlacklist(false);
    TEST_ASSERT(!tb.isBlacklisted(), "setBlacklist false phai chuyen thanh HoatDong");

    bool threwBadDeviceStatus = false;
    try {
        tb.setTrangThai("FakeStatus");
    } catch (const AppException&) {
        threwBadDeviceStatus = true;
    }
    TEST_ASSERT(threwBadDeviceStatus, "Trang thai thiet bi sai phai bi tu choi");

    tb.ganSIM("0912345678");
    TEST_ASSERT(tb.getSoDienThoai() == "0912345678", "ganSIM phai cap nhat sdt hop le");

    //kiem tra sdt thiet bi imei khong hop le
    const std::vector<std::string> invalidTbPhones = {"012345", "0241234567", "09812345678", "098123456A"};
    for (const auto& bad : invalidTbPhones) {
        bool threwCtor = false;
        try {
            ThietBiIMEI badTb(VALID_IMEI_1, "iPhone 15", "Apple", bad, Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
        } catch (const InvalidPhoneNumberException&) {
            threwCtor = true;
        }
        TEST_ASSERT(threwCtor, "ThietBiIMEI constructor phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwGanSIM = false;
        try {
            tb.ganSIM(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwGanSIM = true;
        }
        TEST_ASSERT(threwGanSIM, "ThietBiIMEI ganSIM phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwSetPhone = false;
        try {
            tb.setSoDienThoai(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwSetPhone = true;
        }
        TEST_ASSERT(threwSetPhone, "ThietBiIMEI setSoDienThoai phai nem InvalidPhoneNumberException voi sdt: " + bad);
    }

    tb.setSoDienThoai("0987654321");
    TEST_ASSERT(tb.getSoDienThoai() == "0987654321", "ThietBiIMEI setSoDienThoai hop le thanh cong");

    ThietBiIMEI tbUnassigned(VALID_IMEI_1, "iPhone 15", "Apple", "ChuaGan", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    TEST_ASSERT(tbUnassigned.getSoDienThoai() == "ChuaGan", "ThietBiIMEI constructor voi ChuaGan phai hop le");

    ThietBiIMEI tbEmptyPhone(VALID_IMEI_1, "iPhone 15", "Apple", "", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    TEST_ASSERT(tbEmptyPhone.getSoDienThoai().empty(), "ThietBiIMEI constructor voi chuoi rong phai hop le");

    tb.goSIM();
    TEST_ASSERT(tb.getSoDienThoai() == IMEIConstants::UNASSIGNED_PHONE, "goSIM phai thanh ChuaGan");

    std::string serialized = tb.toFileString();
    ThietBiIMEI deserialized;
    bool ok = deserialized.fromFileString(serialized);
    TEST_ASSERT(ok, "Deserialization ThietBiIMEI phai thanh cong");
    TEST_ASSERT(deserialized.getId() == VALID_IMEI_1, "Deserialized id phai khop");
    TEST_ASSERT(deserialized.getTenThietBi() == "iPhone 15", "Deserialized ten phai khop");
}

void testHopDong() {
    std::cout << "[RUN] Kiem thu HopDong va logic thoi han...\n";

    Date start(1, 1, 2024);
    Date end(1, 1, 2025);
    HopDong hd("HD9999", "KH9999", "0981234567", "GC999", start, end, "TraSau", "HieuLuc", 100000.0);

    TEST_ASSERT(hd.getId() == "HD9999", "HopDong getId phai khop");
    TEST_ASSERT(!hd.isExpired(Date(1, 6, 2024)), "HopDong khong the expired vao thang 6/2024");
    TEST_ASSERT(hd.isExpired(Date(2, 1, 2025)), "HopDong phai expired vao 02/01/2025");

    //kiem tra sdt hop dong khong hop le
    const std::vector<std::string> invalidHdPhones = {"12345", "0123456789", "0241234567", "09812345678", "098123456A"};
    for (const auto& bad : invalidHdPhones) {
        bool threwCtor = false;
        try {
            HopDong badPhone("HD8888", "KH8888", bad, "GC001", start, end, "TraSau", "HieuLuc", 100.0);
        } catch (const InvalidPhoneNumberException&) {
            threwCtor = true;
        }
        TEST_ASSERT(threwCtor, "HopDong constructor phai nem InvalidPhoneNumberException voi sdt: " + bad);

        bool threwMutator = false;
        try {
            hd.setSoDienThoai(bad);
        } catch (const InvalidPhoneNumberException&) {
            threwMutator = true;
        }
        TEST_ASSERT(threwMutator, "HopDong setSoDienThoai phai nem InvalidPhoneNumberException voi sdt: " + bad);
    }

    hd.setSoDienThoai("0912345678");
    TEST_ASSERT(hd.getSoDienThoai() == "0912345678", "HopDong setSoDienThoai hop le thanh cong");
    hd.setSoDienThoai("0981234567");

    Date newEnd(1, 1, 2026);
    hd.giaHan(newEnd);
    TEST_ASSERT(hd.getNgayHetHan() == newEnd, "Gia han hop dong thanh cong");

    bool threwNegPrice = false;
    try {
        HopDong badPrice("HD8887", "KH8887", "0981234567", "GC001", start, end, "TraSau", "HieuLuc", -50000.0);
    } catch (const AppException&) {
        threwNegPrice = true;
    }
    TEST_ASSERT(threwNegPrice, "Gia tri goi am phai bi tu choi");

    bool threwBadType = false;
    try {
        HopDong badType("HD8886", "KH8886", "0981234567", "GC001", start, end, "InvalidType", "HieuLuc", 1000.0);
    } catch (const AppException&) {
        threwBadType = true;
    }
    TEST_ASSERT(threwBadType, "Loai hop dong sai phai bi tu choi");

    bool threwBadStatus = false;
    try {
        HopDong badStatus("HD8885", "KH8885", "0981234567", "GC001", start, end, "TraSau", "InvalidStatus", 1000.0);
    } catch (const AppException&) {
        threwBadStatus = true;
    }
    TEST_ASSERT(threwBadStatus, "Trang thai hop dong sai phai bi tu choi");

    hd.chamDut();
    TEST_ASSERT(hd.getTrangThai() == HopDongConstants::STATUS_TERMINATED, "chamDut phai thanh ThanhLy");

    std::string s = hd.toFileString();
    HopDong dHd;
    bool ok = dHd.fromFileString(s);
    TEST_ASSERT(ok, "Deserialization HopDong phai thanh cong");
    TEST_ASSERT(dHd.getId() == "HD9999", "Deserialized HopDong id phai khop");
    TEST_ASSERT(dHd.getGiaTriGoi() == 100000.0, "Deserialized HopDong giaTri phai khop");
}

void testRepository() {
    std::cout << "[RUN] Kiem thu Repository<T> CRUD va atomic persistence...\n";

    const std::string TEST_FILE = "data/test_repo.txt";
    Repository<HopDong> repo(TEST_FILE);
    repo.clear();

    HopDong hd1("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    HopDong hd2("HD0002", "KH0002", "0978999888", "GC002", Date(1, 2, 2024), Date(1, 2, 2025), "TraTruoc", "HieuLuc", 90000.0);

    TEST_ASSERT(repo.add(hd1), "Them hd1 phai thanh cong");
    TEST_ASSERT(repo.add(hd2), "Them hd2 phai thanh cong");
    TEST_ASSERT(repo.size() == 2, "Repository phai co 2 phan tu");

    bool threwDuplicate = false;
    try {
        repo.add(hd1);
    } catch (const DuplicateIdException&) {
        threwDuplicate = true;
    }
    TEST_ASSERT(threwDuplicate, "Them trung id phai nem DuplicateIdException");

    HopDong* found = repo.findById("HD0001");
    TEST_ASSERT(found != nullptr, "Phai tim thay HD0001");
    TEST_ASSERT(found->getMaKhachHang() == "KH0001", "Khach hang phai la KH0001");

    HopDong notFound = HopDong("HD9999", "KH9999", "0989999999", "GC999", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 100.0);
    bool threwNotFound = false;
    try {
        repo.update("HD9999", notFound);
    } catch (const NotFoundException&) {
        threwNotFound = true;
    }
    TEST_ASSERT(threwNotFound, "Update ma khong ton tai phai nem NotFoundException");

    repo.remove("HD0001");
    TEST_ASSERT(repo.size() == 1, "Sau khi xoa size phai la 1");
    TEST_ASSERT(repo.findById("HD0001") == nullptr, "HD0001 khong con ton tai");

    Repository<HopDong> reloadedRepo(TEST_FILE);
    reloadedRepo.loadFromFile();
    TEST_ASSERT(reloadedRepo.size() == 1, "Reloaded store phai co dung 1 ban ghi da luu");
    TEST_ASSERT(reloadedRepo.findById("HD0002") != nullptr, "Reloaded store phai chua HD0002");

    const std::string INVALID_IMEI_FILE = "data/test_invalid_imei.txt";
    {
        std::ofstream out(INVALID_IMEI_FILE);
        out << "not-an-imei|Phone|Brand|0241234567|15/01/2024|HoatDong|BTS-001\\n";
    }
    Repository<ThietBiIMEI> invalidImeiStore(INVALID_IMEI_FILE);
    TEST_ASSERT(invalidImeiStore.loadFromFile(), "File IMEI ton tai phai mo duoc");
    TEST_ASSERT(invalidImeiStore.empty(), "Ban ghi IMEI sai phai bi bo qua khi nap file");

    const std::string INVALID_HOPDONG_FILE = "data/test_invalid_hopdong.txt";
    {
        std::ofstream out(INVALID_HOPDONG_FILE);
        out << "HD-BAD|KH-1|0241234567|GC-1|01/01/2024|01/01/2025|TraSau|HieuLuc|1000\\n";
    }
    Repository<HopDong> invalidHopDongStore(INVALID_HOPDONG_FILE);
    TEST_ASSERT(invalidHopDongStore.loadFromFile(), "File hop dong ton tai phai mo duoc");
    TEST_ASSERT(invalidHopDongStore.empty(), "Ban ghi hop dong sai phai bi bo qua khi nap file");

    const std::string MISSING_FILE = "data/test_missing_file.txt";
    std::remove(MISSING_FILE.c_str());
    Repository<HopDong> missingStore(MISSING_FILE);
    TEST_ASSERT(!missingStore.loadFromFile(), "File khong ton tai phai bao that bai khi nap");

    const std::string LOAD_PRESERVE_FILE = "data/test_load_preserve.txt";
    Repository<HopDong> loadPreserveStore(LOAD_PRESERVE_FILE);
    loadPreserveStore.clear();
    loadPreserveStore.add(hd1);
    std::remove(LOAD_PRESERVE_FILE.c_str());
    TEST_ASSERT(!loadPreserveStore.loadFromFile(), "Nap file da bi xoa phai bao that bai");
    TEST_ASSERT(loadPreserveStore.size() == 1 && loadPreserveStore.findById("HD0001") != nullptr,
                "Nap file that bai phai giu nguyen du lieu dang co trong bo nho");

    Repository<HopDong> rollbackStore("/proc/ktlt_repository_rollback.txt");
    bool saveFailed = false;
    try {
        rollbackStore.add(hd1);
    } catch (const FileIOException& e) {
        saveFailed = (e.what() != nullptr);
    }
    TEST_ASSERT(saveFailed, "Duong dan khong ghi duoc phai nem FileIOException");
    TEST_ASSERT(rollbackStore.empty(), "Them that bai phai rollback ban ghi trong bo nho");

    Repository<HopDong> updateRollbackStore("/proc/ktlt_repository_update.txt");
    updateRollbackStore.getAll().push_back(hd1);
    HopDong changed("HD0001", "KH0002", "0978999888", "GC002", Date(1, 1, 2024), Date(1, 1, 2025), "TraTruoc", "HieuLuc", 90000.0);
    bool updateThrew = false;
    try {
        updateRollbackStore.update("HD0001", changed);
    } catch (const FileIOException& e) {
        updateThrew = (e.what() != nullptr);
    }
    TEST_ASSERT(updateThrew, "Cap nhat tren file loi phai nem FileIOException");
    TEST_ASSERT(updateRollbackStore.findById("HD0001")->getMaKhachHang() == "KH0001",
                "Cap nhat that bai phai khoi phuc ban ghi cu");

    Repository<HopDong> removeRollbackStore("/proc/ktlt_repository_remove.txt");
    removeRollbackStore.getAll().push_back(hd1);
    bool removeThrew = false;
    try {
        removeRollbackStore.remove("HD0001");
    } catch (const FileIOException& e) {
        removeThrew = (e.what() != nullptr);
    }
    TEST_ASSERT(removeThrew, "Xoa tren file loi phai nem FileIOException");
    TEST_ASSERT(removeRollbackStore.size() == 1 && removeRollbackStore.findById("HD0001") != nullptr,
                "Xoa that bai phai chen lai ban ghi dung vi tri");

    Repository<HopDong> clearRollbackStore("/proc/ktlt_repository_clear.txt");
    clearRollbackStore.getAll().push_back(hd1);
    clearRollbackStore.getAll().push_back(hd2);
    bool clearThrew = false;
    try {
        clearRollbackStore.clear();
    } catch (const FileIOException& e) {
        clearThrew = (e.what() != nullptr);
    }
    TEST_ASSERT(clearThrew, "Clear tren file loi phai nem FileIOException");
    TEST_ASSERT(clearRollbackStore.size() == 2 && clearRollbackStore.findById("HD0001") != nullptr && clearRollbackStore.findById("HD0002") != nullptr,
                "Clear that bai phai khoi phuc toan bo danh sach");

    std::remove(TEST_FILE.c_str());
    std::remove(INVALID_IMEI_FILE.c_str());
    std::remove(INVALID_HOPDONG_FILE.c_str());
}

void testDisplayHelper() {
    std::cout << "[RUN] Kiem thu DisplayHelper va format ASCII table/card...\n";

    std::ostringstream ossBorder;
    DisplayHelper::printBorder({6, 8}, ossBorder);
    std::string borderExpected = "+------+--------+\n";
    TEST_ASSERT(ossBorder.str() == borderExpected, "DisplayHelper printBorder phai dung dinh dang +------+--------+");

    std::ostringstream ossHeader;
    DisplayHelper::printHeader({"Col A", "Col B"}, {7, 8}, {false, true}, ossHeader);
    std::string headerStr = ossHeader.str();
    TEST_ASSERT(headerStr.find("+-------+--------+") != std::string::npos, "Header phai co duong vien tren va duoi");
    TEST_ASSERT(headerStr.find("| Col A |  Col B |") != std::string::npos, "Header phai can chinh dung le trai va le phai");

    std::ostringstream ossRow;
    DisplayHelper::printRow({"val1", "val2"}, {7, 8}, {false, true}, ossRow);
    std::string rowStr = ossRow.str();
    TEST_ASSERT(rowStr == "| val1  |   val2 |\n", "Row phai can chinh dung le va do rong");

    std::ostringstream ossCard;
    DisplayHelper::printCard({{"Key A", "Value A"}, {"Key B", "Value B"}}, 10, ossCard);
    std::string cardStr = ossCard.str();
    TEST_ASSERT(cardStr.find("Key A     : Value A") != std::string::npos, "Card phai in nhan va gia tri dung dinh dang");

    HopDong tableItem("HD-TABLE", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    std::ostringstream ossTable;
    DisplayHelper::printTable(std::vector<HopDong>{tableItem}, ossTable);
    TEST_ASSERT(ossTable.str().find("HD-TABLE") != std::string::npos,
                "printTable phai forward output stream vao display methods");

    //kiem thu tich hop model
    HopDong hd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    ThietBiIMEI tb("860123456789014", "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");

    //chuyen huong cout tam thoi
    std::ostringstream ossModel;
    std::streambuf* oldCout = std::cout.rdbuf(ossModel.rdbuf());
    hd.displayHeader();
    hd.displayRow();
    hd.displayDetail();
    tb.displayHeader();
    tb.displayRow();
    tb.displayDetail();
    std::cout.rdbuf(oldCout);

    std::string modelOut = ossModel.str();
    TEST_ASSERT(!modelOut.empty(), "Model display methods phai tao ra output qua DisplayHelper");
    TEST_ASSERT(modelOut.find("HD0001") != std::string::npos, "Output phai chua Ma HD0001");
    TEST_ASSERT(modelOut.find("860123456789014") != std::string::npos, "Output phai chua Ma IMEI");
}

void testMenuControllers() {
    std::cout << "[RUN] Kiem thu Menu Controllers (HopDongMenu & ThietBiIMEIMenu)...\n";

    const std::string HD_TEST_FILE = "data/test_hd_menu.txt";
    const std::string IMEI_TEST_FILE = "data/test_imei_menu.txt";
    const std::string KH_TEST_FILE = "data/test_kh_menu.txt";
    const std::string GC_TEST_FILE = "data/test_gc_menu.txt";
    const std::string TB_TEST_FILE = "data/test_tb_menu.txt";

    Repository<HopDong> hdRepo(HD_TEST_FILE);
    hdRepo.clear();
    HopDong sampleHd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    hdRepo.add(sampleHd);

    Repository<ThietBiIMEI> imeiRepo(IMEI_TEST_FILE);
    imeiRepo.clear();
    ThietBiIMEI sampleImei("860123456789014", "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    imeiRepo.add(sampleImei);

    Repository<KhachHang> khRepo(KH_TEST_FILE);
    khRepo.clear();
    KhachHang sampleKh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    khRepo.add(sampleKh);

    Repository<GoiCuoc> gcRepo(GC_TEST_FILE);
    gcRepo.clear();
    GoiCuoc sampleGc("GC001", "Goi Cuoc 60G", 150000.0, 60, 1000, 30);
    gcRepo.add(sampleGc);

    Repository<ThueBao> tbRepo(TB_TEST_FILE);
    tbRepo.clear();
    ThueBao sampleTb("0981234567", "TraSau", "KH0001", "GC001", "15/01/2024", "HoatDong", 100000.0);
    tbRepo.add(sampleTb);

    HopDongMenu hdMenu(hdRepo, khRepo, tbRepo, gcRepo);
    ThietBiIMEIMenu imeiMenu(imeiRepo, tbRepo);

    std::streambuf* origCin = std::cin.rdbuf();
    std::streambuf* origCout = std::cout.rdbuf();

    //test thoat hopdongmenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("QUAN LY HOP DONG DANG KY") != std::string::npos, "HopDongMenu phai hien thi tieu de menu");
        TEST_ASSERT(out.find("[0] Quay lai menu chinh") != std::string::npos, "HopDongMenu phai co option [0] Quay lai menu chinh");
    }

    //test thoat imeimenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("QUAN LY THIET BI DAU CUOI (IMEI)") != std::string::npos, "ThietBiIMEIMenu phai hien thi tieu de menu");
        TEST_ASSERT(out.find("[0] Quay lai menu chinh") != std::string::npos, "ThietBiIMEIMenu phai co option [0] Quay lai menu chinh");
    }

    //test submenu tim kiem
    {
        std::istringstream inputSim("3\n0\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("TIM KIEM HOP DONG") != std::string::npos, "HopDongMenu timKiemHopDong phai vao submenu");
    }

    //test huy cap nhat hd
    {
        std::istringstream inputSim("5\nHD0001\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "HopDongMenu cap nhat choice 0 phai thong bao huy bo");
    }

    //test huy cap nhat imei
    {
        std::istringstream inputSim("5\n860123456789014\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "ThietBiIMEIMenu cap nhat choice 0 phai thong bao huy bo");
    }

    //test eof inputhelper
    {
        std::istringstream emptyInput("");
        std::ostringstream outputCapture;
        std::cin.rdbuf(emptyInput.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        std::string phone = InputHelper::getPhoneNumber("Nhap sdt: ", false);
        TEST_ASSERT(phone.empty(), "getPhoneNumber tren EOF phai return chuoi rong ma khong loop vo han");

        bool confirm = InputHelper::getConfirm("Xac nhan?");
        TEST_ASSERT(!confirm, "getConfirm tren EOF phai return false ma khong loop vo han");
    }

    //test tu choi khoa ngoai
    {
        std::istringstream inputSim("1\nHD0002\nKH9999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("khong ton tai trong he thong") != std::string::npos, "HopDongMenu phai tu choi Ma KH khong ton tai");
        TEST_ASSERT(hdRepo.findById("HD0002") == nullptr, "HopDongMenu khong duoc luu ban ghi khi khoa ngoai sai");
    }

    //test tu choi sdt
    {
        std::istringstream inputSim("1\n1\n861987654321096\nXiaomi 14\nXiaomi\n0989999999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("chua duoc dang ky tren he thong") != std::string::npos, "ThietBiIMEIMenu phai tu choi SDT chua dang ky");
        TEST_ASSERT(imeiRepo.findById("861987654321096") == nullptr, "ThietBiIMEIMenu khong duoc luu ban ghi khi SDT chua dang ky");
    }

    //khoi phuc cout/cin
    std::cin.rdbuf(origCin);
    std::cout.rdbuf(origCout);

    std::remove(HD_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
    std::remove(KH_TEST_FILE.c_str());
    std::remove(GC_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
}

void testFileIOTmpfs() {
    std::cout << "[RUN] Kiem thu FileIO staging qua tmps/ va tmpfs zero-SSD...\n";

    TEST_ASSERT(FileIO::joinPath("data", "khachhang.txt") == "data/khachhang.txt",
                "joinPath phai tao duong dan data on dinh");
    TEST_ASSERT(FileIO::resolveDataDirectory("/tmp/quanlythuebao") == "data",
                "resolveDataDirectory phai tim thay bo data tu thu muc hien tai");
    TEST_ASSERT(FileIO::initTmpDir(), "initTmpDir phai return true");

    std::string testTarget = "data/test_tmpfs_target.txt";
    std::string stagingPath = FileIO::getTempStagingPath(testTarget);

    TEST_ASSERT(stagingPath.find("tmps/") != std::string::npos, "Staging path phai nam trong thu muc tmps/");

    //ghi file staging tmps
    {
        std::ofstream out(stagingPath);
        TEST_ASSERT(out.is_open(), "Mo file staging tren tmps/ phai thanh cong");
        out << "SAMPLE_DATA_LINE_1|VAL1\nSAMPLE_DATA_LINE_2|VAL2\n";
        out.close();
    }

    //commit tu tmps
    bool committed = FileIO::commitFromTmp(stagingPath, testTarget);
    TEST_ASSERT(committed, "commitFromTmp phai thanh cong");

    //kiem tra noi dung file
    {
        std::ifstream in(testTarget);
        TEST_ASSERT(in.is_open(), "Target file tren SSD phai ton tai sau khi commit tu tmps/");
        std::string line1, line2;
        std::getline(in, line1);
        std::getline(in, line2);
        TEST_ASSERT(line1 == "SAMPLE_DATA_LINE_1|VAL1", "Noi dung dong 1 phai khop");
        TEST_ASSERT(line2 == "SAMPLE_DATA_LINE_2|VAL2", "Noi dung dong 2 phai khop");
        in.close();
    }

    //kiem tra don dep tmps
    {
        std::ifstream tmpCheck(stagingPath);
        TEST_ASSERT(!tmpCheck.is_open(), "File tam tren tmps/ phai duoc xoa sau khi commit vao SSD");
    }

    std::remove(testTarget.c_str());

    const std::string preservedTmp = FileIO::getTempStagingPath("data/test_preserved_target");
    const std::string preservedTarget = "data/test_preserved_target";
    std::remove(preservedTarget.c_str());
#if defined(_WIN32)
    _rmdir(preservedTarget.c_str());
    _mkdir(preservedTarget.c_str());
#else
    rmdir(preservedTarget.c_str());
    mkdir(preservedTarget.c_str(), 0755);
#endif
    {
        std::ofstream out(preservedTmp);
        out << "NEW_DATA";
    }
    bool replaceDirectory = FileIO::commitFromTmp(preservedTmp, preservedTarget);
    std::ifstream stagedAfterFailure(preservedTmp);
    TEST_ASSERT(!replaceDirectory, "Commit vao dich la thu muc phai that bai");
    TEST_ASSERT(stagedAfterFailure.is_open(), "Commit that bai phai giu lai file staging");
    stagedAfterFailure.close();
    std::remove(preservedTmp.c_str());
#if defined(_WIN32)
    _rmdir(preservedTarget.c_str());
#else
    rmdir(preservedTarget.c_str());
#endif
}

void testKhachHangModel() {
    std::cout << "[RUN] Kiem thu KhachHang model va validation...\n";

    KhachHang kh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    TEST_ASSERT(kh.getMaKhachHang() == "KH0001", "Ma KH phai la KH0001");
    TEST_ASSERT(kh.getHoTen() == "Nguyen Van An", "Ho ten phai khop");
    TEST_ASSERT(kh.getLoaiGiayTo() == "CCCD", "Loai giay to phai la CCCD");
    TEST_ASSERT(kh.getSoGiayTo() == "001201004567", "So giay to phai khop");
    TEST_ASSERT(kh.getSoDienThoai() == "0981234567", "So dien thoai phai khop");

    //kiem tra loi sdt
    bool caughtPhone = false;
    try {
        KhachHang khErr("KH0002", "Tran Binh", "CCCD", "001201004567", "01/01/2021", "C06", "012345", "HN");
    } catch (const InvalidPhoneNumberException& e) {
        caughtPhone = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtPhone, "KhachHang voi SDT khong hop le phai quang InvalidPhoneNumberException");

    //kiem tra serialize
    std::string fileStr = kh.toFileString();
    TEST_ASSERT(fileStr == "KH0001|Nguyen Van An|CCCD|001201004567|10/01/2021|Cuc C06|0981234567|Ha Noi", "toFileString phai dung format pipe-delimited");

    KhachHang khParsed;
    TEST_ASSERT(khParsed.fromFileString(fileStr), "fromFileString phai parse thanh cong");
    TEST_ASSERT(khParsed.getMaKhachHang() == "KH0001", "Parsed Ma KH phai khop");
    TEST_ASSERT(khParsed.getHoTen() == "Nguyen Van An", "Parsed Ho Ten phai khop");
    TEST_ASSERT(!khParsed.fromFileString("INVALID|LINE"), "fromFileString voi line sai so truong phai return false");

    //kiem tra display
    std::ostringstream oss;
    std::streambuf* oldCout = std::cout.rdbuf(oss.rdbuf());
    kh.displayHeader();
    kh.displayRow();
    kh.displayDetail();
    std::cout.rdbuf(oldCout);
    std::string out = oss.str();
    TEST_ASSERT(out.find("KH0001") != std::string::npos, "Display output phai chua KH0001");
}

void testGoiCuocModel() {
    std::cout << "[RUN] Kiem thu GoiCuoc model va nghiep vu goi...\n";

    GoiCuoc gc1("M50", "Mobi 50", 50000.0, 5, 100, 30);
    GoiCuoc gc2("VD140", "VinaData 140", 140000.0, 30, 300, 30);

    TEST_ASSERT(gc1.getMaGoi() == "M50", "Ma goi phai la M50");
    TEST_ASSERT(gc1.getTenGoi() == "Mobi 50", "Ten goi phai la Mobi 50");
    TEST_ASSERT(gc1.getGiaCuoc() == 50000.0, "Gia cuoc phai la 50000");
    TEST_ASSERT(gc1.getDungLuongData() == 5, "Data phai la 5 GB");
    TEST_ASSERT(gc1.getSoPhutGoi() == 100, "Thoai phai la 100 phut");
    TEST_ASSERT(gc1.getChuKyNgay() == 30, "Chu ky phai la 30 ngay");

    //kiem tra loi validate
    bool caughtPrice = false;
    try {
        GoiCuoc gcErr("ERR", "Loi", -1000.0, 5, 100, 30);
    } catch (const AppException& e) {
        caughtPrice = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtPrice, "Gia cuoc am phai quang AppException");

    //kiem tra serialize
    std::string fileStr = gc1.toFileString();
    TEST_ASSERT(fileStr == "M50|Mobi 50|50000|5|100|30", "toFileString GoiCuoc phai dung format");

    GoiCuoc parsedGc;
    TEST_ASSERT(parsedGc.fromFileString(fileStr), "fromFileString GoiCuoc phai parse thanh cong");
    TEST_ASSERT(parsedGc.getMaGoi() == "M50", "Parsed ma goi phai khop");
    TEST_ASSERT(parsedGc.getGiaCuoc() == 50000.0, "Parsed gia cuoc phai khop");
}

void testThueBaoModel() {
    std::cout << "[RUN] Kiem thu ThueBao model va giao dich tai khoan...\n";

    ThueBao tb("0981234567", ThueBaoConstants::TYPE_PREPAID, "KH0001", "M50", "15/01/2024", ThueBaoConstants::STATUS_ACTIVE, 100000.0);
    TEST_ASSERT(tb.getSoDienThoai() == "0981234567", "SDT phai khop");
    TEST_ASSERT(tb.getLoaiThueBao() == ThueBaoConstants::TYPE_PREPAID, "Loai TB phai la TraTruoc");
    TEST_ASSERT(tb.getSoDuTaiKhoan() == 100000.0, "So du ban dau phai la 100000");

    //nap tien
    tb.congTien(50000.0);
    TEST_ASSERT(tb.getSoDuTaiKhoan() == 150000.0, "So du sau nap 50000 phai la 150000");

    //tru tien
    tb.truTien(100000.0);
    TEST_ASSERT(tb.getSoDuTaiKhoan() == 50000.0, "So du sau tru 100000 phai la 50000");

    //tru tien vuot so du
    bool caughtOverdraft = false;
    try {
        tb.truTien(60000.0);
    } catch (const AppException& e) {
        caughtOverdraft = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtOverdraft, "Tru tien qua so du thue bao tra truoc phai quang AppException");

    //kiem tra loi sdt
    bool caughtPhone = false;
    try {
        ThueBao tbErr("012345", "TraTruoc", "KH0001", "M50", "01/01/2024");
    } catch (const InvalidPhoneNumberException& e) {
        caughtPhone = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtPhone, "SDT sai quy chuan phai quang InvalidPhoneNumberException");

    //kiem tra serialize
    std::string tbStr = tb.toFileString();
    TEST_ASSERT(tbStr.find("0981234567|TraTruoc|KH0001|M50|15/01/2024|HoatDong|50000") != std::string::npos, "toFileString ThueBao phai dung format");

    ThueBao parsedTb;
    TEST_ASSERT(parsedTb.fromFileString(tbStr), "fromFileString ThueBao phai parse thanh cong");
    TEST_ASSERT(parsedTb.getSoDienThoai() == "0981234567", "Parsed SDT phai khop");
    TEST_ASSERT(parsedTb.getSoDuTaiKhoan() == 50000.0, "Parsed so du phai khop");
}

void testHoaDonAndNapTienModels() {
    std::cout << "[RUN] Kiem thu HoaDon va NapTien models...\n";

    HoaDon hd("HD01", "0981234567", "01/2024", 50000.0, 100000.0, 150000.0, HoaDonConstants::STATUS_UNPAID);
    TEST_ASSERT(hd.quaHan(Date(1, 2, 2024)), "Hoa don thang truoc phai qua han");
    TEST_ASSERT(!hd.quaHan(Date(1, 1, 2024)), "Hoa don cung thang khong qua han");
    TEST_ASSERT(hd.getMaHoaDon() == "HD01", "Ma HD phai la HD01");
    TEST_ASSERT(hd.getTongTien() == 150000.0, "Tong tien phai la 150000");
    TEST_ASSERT(hd.getTrangThai() == HoaDonConstants::STATUS_UNPAID, "Trang thai phai la ChuaThanhToan");

    hd.thanhToan();
    TEST_ASSERT(hd.getTrangThai() == HoaDonConstants::STATUS_PAID, "Sau thanh toan phai la DaThanhToan");

    std::string hdStr = hd.toFileString();
    TEST_ASSERT(hdStr == "HD01|0981234567|01/2024|50000|100000|150000|DaThanhToan", "HoaDon toFileString phai dung");

    HoaDon parsedHd;
    TEST_ASSERT(parsedHd.fromFileString(hdStr), "HoaDon fromFileString phai thanh cong");
    TEST_ASSERT(parsedHd.getTongTien() == 150000.0, "Parsed tong tien phai khop");

    NapTien nt("GD0001", "0981234567", "15/01/2024", 200000.0, NapTienConstants::METHOD_BANKING, NapTienConstants::STATUS_SUCCESS);
    TEST_ASSERT(nt.getMaGiaoDich() == "GD0001", "Ma GD phai la GD0001");
    TEST_ASSERT(nt.getSoTien() == 200000.0, "So tien nap phai la 200000");

    bool caughtAmnt = false;
    try {
        NapTien ntErr("GD0002", "0981234567", "15/01/2024", -5000.0);
    } catch (const AppException& e) {
        caughtAmnt = (e.what() != nullptr);
    }
    TEST_ASSERT(caughtAmnt, "Nap tien voi so tien am phai quang AppException");

    std::string ntStr = nt.toFileString();
    TEST_ASSERT(ntStr == "GD0001|0981234567|15/01/2024|200000|NganHang|ThanhCong", "NapTien toFileString phai dung");

    NapTien parsedNt;
    TEST_ASSERT(parsedNt.fromFileString(ntStr), "NapTien fromFileString phai thanh cong");
    TEST_ASSERT(parsedNt.getSoTien() == 200000.0, "Parsed so tien nap phai khop");
}

void testNewRepositoriesAndMenus() {
    std::cout << "[RUN] Kiem thu Repository CRUD va Menu Controllers moi (GoiCuocMenu & ThueBaoMenu)...\n";

    const std::string GC_TEST_FILE = "data/test_gc_repo.txt";
    const std::string TB_TEST_FILE = "data/test_tb_repo.txt";
    const std::string KH_TEST_FILE = "data/test_kh_repo.txt";

    Repository<GoiCuoc> gcRepo(GC_TEST_FILE);
    gcRepo.clear();
    GoiCuoc gc("M50", "Mobi 50", 50000.0, 5, 100, 30);
    gcRepo.add(gc);

    TEST_ASSERT(gcRepo.size() == 1, "gcRepo size phai bang 1 sau khi them");
    TEST_ASSERT(gcRepo.findById("M50") != nullptr, "findById M50 phai tim thay");

    Repository<KhachHang> khRepo(KH_TEST_FILE);
    khRepo.clear();
    KhachHang kh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    khRepo.add(kh);

    Repository<ThueBao> tbRepo(TB_TEST_FILE);
    tbRepo.clear();
    ThueBao tb("0981234567", "TraTruoc", "KH0001", "M50", "15/01/2024", "HoatDong", 100000.0);
    tbRepo.add(tb);

    TEST_ASSERT(tbRepo.size() == 1, "tbRepo size phai bang 1 sau khi them");
    TEST_ASSERT(tbRepo.findById("0981234567") != nullptr, "findById 0981234567 phai tim thay");

    GoiCuocMenu gcMenu(gcRepo);
    ThueBaoMenu tbMenu(tbRepo, gcRepo, khRepo);

    std::streambuf* origCin = std::cin.rdbuf();
    std::streambuf* origCout = std::cout.rdbuf();

    //test thoat goicuocmenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        gcMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("QUAN LY GOI CUOC DI DONG") != std::string::npos, "GoiCuocMenu phai hien thi tieu de menu");
        TEST_ASSERT(out.find("[0] Quay lai menu chinh") != std::string::npos, "GoiCuocMenu phai co option [0] Quay lai menu chinh");
    }

    //test thoat thuebaomenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("QUAN LY THUE BAO DI DONG") != std::string::npos, "ThueBaoMenu phai hien thi tieu de menu");
        TEST_ASSERT(out.find("[0] Quay lai menu chinh") != std::string::npos, "ThueBaoMenu phai co option [0] Quay lai menu chinh");
    }

    //test tu choi ma kh
    {
        std::istringstream inputSim("1\n0989999999\n1\nKH9999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("khong ton tai trong he thong") != std::string::npos, "ThueBaoMenu phai tu choi Ma KH khong ton tai");
        TEST_ASSERT(tbRepo.findById("0989999999") == nullptr, "ThueBaoMenu khong duoc luu thue bao khi Ma KH sai");
    }

    std::cin.rdbuf(origCin);
    std::cout.rdbuf(origCout);

    std::remove(GC_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(KH_TEST_FILE.c_str());
}

void testCDRAndKhieuNaiModels() {
    std::cout << "[RUN] Kiem thu CDR va KhieuNai models...\n";
    TEST_ASSERT(FilePaths::GHICUOCGOI_DATA == "data/ghicuocgoi.txt", "Duong dan ghicuocgoi phai dung ten file moi");
    std::ifstream ghicuocgoiFixture(FilePaths::GHICUOCGOI_DATA);
    TEST_ASSERT(ghicuocgoiFixture.good(), "Tep du lieu ghicuocgoi phai ton tai");

    // Test CDR
    CDR cdr("CDR0001", "0981234567", "0912345678", "Thoai", "15/01/2024 08:30:00", 120, 2000.0);
    TEST_ASSERT(cdr.getMaCDR() == "CDR0001", "Ma CDR phai khop");
    TEST_ASSERT(cdr.getSoGoi() == "0981234567", "So goi CDR phai khop");
    TEST_ASSERT(cdr.getSoNhan() == "0912345678", "So nhan CDR phai khop");
    TEST_ASSERT(cdr.getLoaiDichVu() == "Thoai", "Loai dich vu CDR phai khop");
    TEST_ASSERT(cdr.getThoiLuong() == 120, "Thoi luong CDR phai khop");
    TEST_ASSERT(cdr.getCuocPhatSinh() == 2000.0, "Cuoc phat sinh CDR phai khop");

    std::string cdrStr = cdr.toFileString();
    TEST_ASSERT(cdrStr.find("CDR0001") != std::string::npos, "toFileString CDR phai chua Ma CDR");

    CDR parsedCdr;
    TEST_ASSERT(parsedCdr.fromFileString(cdrStr), "fromFileString CDR phai thanh cong");
    TEST_ASSERT(parsedCdr.getCuocPhatSinh() == 2000.0, "Parsed cuoc phat sinh phai khop");

    bool threwCdrDuration = false;
    try {
        CDR badCdr("CDR0002", "0981234567", "0912345678", "Thoai", "15/01/2024", -10, 1000.0);
    } catch (const AppException&) {
        threwCdrDuration = true;
    }
    TEST_ASSERT(threwCdrDuration, "Thoi luong am phai nem AppException");

    bool threwCdrCost = false;
    try {
        CDR badCdr("CDR0003", "0981234567", "0912345678", "Thoai", "15/01/2024", 60, -500.0);
    } catch (const AppException&) {
        threwCdrCost = true;
    }
    TEST_ASSERT(threwCdrCost, "Cuoc am phai nem AppException");

    // Test KhieuNai
    KhieuNai kn("KN0001", "0912345678", "CuocPhatSinh", "02/03/2024", "Cuoc data tang cao", "TiepNhan");
    TEST_ASSERT(kn.getMaKhieuNai() == "KN0001", "Ma khieu nai phai khop");
    TEST_ASSERT(kn.getSoDienThoai() == "0912345678", "SĐT khieu nai phai khop");
    TEST_ASSERT(kn.getLoaiKhieuNai() == "CuocPhatSinh", "Loai khieu nai phai khop");
    TEST_ASSERT(kn.getTrangThai() == "TiepNhan", "Trang thai khieu nai ban dau phai la TiepNhan");

    kn.setTrangThai("DaGiaiQuyet");
    TEST_ASSERT(kn.getTrangThai() == "DaGiaiQuyet", "setTrangThai phai cap nhat dung");

    std::string knStr = kn.toFileString();
    TEST_ASSERT(knStr.find("KN0001") != std::string::npos, "toFileString KhieuNai phai chua Ma KN");

    KhieuNai parsedKn;
    TEST_ASSERT(parsedKn.fromFileString(knStr), "fromFileString KhieuNai phai thanh cong");
    TEST_ASSERT(parsedKn.getTrangThai() == "DaGiaiQuyet", "Parsed trang thai khieu nai phai khop");

    bool threwKnPhone = false;
    try {
        KhieuNai badKn("KN0002", "012345", "CuocPhatSinh", "02/03/2024", "Loi", "TiepNhan");
    } catch (const InvalidPhoneNumberException&) {
        threwKnPhone = true;
    }
    TEST_ASSERT(threwKnPhone, "SĐT khong hop le phai nem InvalidPhoneNumberException");
}

void testBTSRegistry() {
    std::cout << "[RUN] Kiem thu BTSRegistry va danh ba tram phat song...\n";

    BTSRegistry bts;
    TEST_ASSERT(bts.size() > 0, "BTSRegistry phai nap duoc it nhat 1 tram tu bts_vietnam.txt");

    const BTSRecord* r = bts.findById("BTS-BD-001");
    TEST_ASSERT(r != nullptr, "findById BTS-BD-001 phai tim thay");
    if (r) {
        TEST_ASSERT(r->carrier.find("VinaPhone") != std::string::npos, "Carrier BTS-BD-001 phai la VinaPhone");
    }

    TEST_ASSERT(bts.findById("NON_EXISTENT_BTS") == nullptr, "findById khong tim thay phai return nullptr");

    std::string nearest = bts.suggestNearestBTS("Thu Dau Mot");
    TEST_ASSERT(!nearest.empty(), "suggestNearestBTS phai goi y duoc ma tram");
}

void testRemainingMenuControllers() {
    std::cout << "[RUN] Kiem thu Menu Controllers moi (KhachHangMenu, NapTienMenu, HoaDonMenu)...\n";

    const std::string KH_TEST_FILE = "data/test_kh_ctrl.txt";
    const std::string TB_TEST_FILE = "data/test_tb_ctrl.txt";
    const std::string HD_TEST_FILE = "data/test_hd_ctrl.txt";
    const std::string NT_TEST_FILE = "data/test_nt_ctrl.txt";
    const std::string GHICUOCGOI_TEST_FILE = "data/test_ghicuocgoi_ctrl.txt";
    const std::string INV_TEST_FILE = "data/test_inv_ctrl.txt";
    const std::string KN_TEST_FILE = "data/test_kn_ctrl.txt";
    const std::string IMEI_TEST_FILE = "data/test_imei_ctrl.txt";

    Repository<KhachHang> khRepo(KH_TEST_FILE);
    khRepo.clear();
    KhachHang kh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    khRepo.add(kh);

    Repository<ThueBao> tbRepo(TB_TEST_FILE);
    tbRepo.clear();
    ThueBao tb("0981234567", "TraTruoc", "KH0001", "GC001", "15/01/2024", "HoatDong", 100000.0);
    tbRepo.add(tb);

    Repository<HopDong> hdRepo(HD_TEST_FILE);
    hdRepo.clear();
    HopDong hd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    hdRepo.add(hd);

    Repository<NapTien> ntRepo(NT_TEST_FILE);
    ntRepo.clear();

    Repository<CDR> cdrRepo(GHICUOCGOI_TEST_FILE);
    cdrRepo.clear();

    Repository<HoaDon> hoaDonRepo(INV_TEST_FILE);
    hoaDonRepo.clear();

    Repository<KhieuNai> knRepo(KN_TEST_FILE);
    knRepo.clear();

    Repository<ThietBiIMEI> imeiRepo(IMEI_TEST_FILE);
    imeiRepo.clear();

    KhachHangMenu khMenu(khRepo, tbRepo, hdRepo);
    ThietBiIMEIMenu imeiMenu(imeiRepo, tbRepo);
    NapTienMenu ntMenu(ntRepo, tbRepo, cdrRepo);
    HoaDonMenu hdMenu(hoaDonRepo, tbRepo, knRepo);

    ThueBao tbSau("0981234568", "TraSau", "KH0001", "GC001", "15/01/2024", "HoatDong", 0.0);
    tbRepo.add(tbSau);
    hoaDonRepo.add(HoaDon("HDNO", "0981234568", "01/2024", 0.0, 100.0, 100.0, HoaDonConstants::STATUS_UNPAID));
    TEST_ASSERT(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 1, "No cuoc qua han phai khoa mot thue bao");
    TEST_ASSERT(tbRepo.findById("0981234568")->getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT, "Thue bao tra sau no qua han phai bi khoa no");
    TEST_ASSERT(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 1, "Kiem tra no lap lai van nhan dien mot khoan no");
    HoaDon noDaTra = *hoaDonRepo.findById("HDNO");
    noDaTra.thanhToan();
    hoaDonRepo.update("HDNO", noDaTra);
    TEST_ASSERT(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 0, "Thanh toan het no phai xoa danh sach no qua han");
    TEST_ASSERT(tbRepo.findById("0981234568")->getTrangThai() == ThueBaoConstants::STATUS_ACTIVE, "Thanh toan het no phai mo khoa tu dong");

    std::streambuf* origCin = std::cin.rdbuf();
    std::streambuf* origCout = std::cout.rdbuf();

    // 0. Test ThietBiIMEIMenu: sinh IMEI mo phong
    {
        std::istringstream inputSim("1\n2\nDemo Phone\nDemo Brand\n\n01/01/2024\nBTS-DEMO-001\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        TEST_ASSERT(imeiRepo.size() == 1, "ThietBiIMEIMenu phai them thiet bi voi IMEI mo phong");
        if (!imeiRepo.empty()) {
            const std::string generated = imeiRepo.getAll().front().getMaIMEI();
            TEST_ASSERT(generated.length() == IMEIConstants::REQUIRED_IMEI_LENGTH,
                        "IMEI mo phong phai co dung 15 chu so");
            TEST_ASSERT(generated.find_first_not_of("0123456789") == std::string::npos,
                        "IMEI mo phong phai chi gom chu so ASCII");
            TEST_ASSERT(ThietBiIMEI::validateLuhn(generated),
                        "IMEI mo phong phai hop le theo Luhn");
            TEST_ASSERT(outputCapture.str().find("MO PHONG") != std::string::npos,
                        "Menu phai danh dau IMEI mo phong");
        }
    }

    // 1. Test KhachHangMenu: them khach hang moi
    {
        std::istringstream inputSim("1\nKH0002\nTran Thi Binh\n001202008912\n15/05/2022\nCuc C06\n0978999888\nHai Phong\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        khMenu.showMenu();

        TEST_ASSERT(khRepo.findById("KH0002") != nullptr, "KhachHangMenu phai them duoc KH0002");
    }

    // 1a. Test KhachHangMenu: tim kiem ma KH khong phan biet hoa thuong
    {
        std::istringstream inputSim("3\n1\n kh0001 \n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        khMenu.showMenu();

        TEST_ASSERT(outputCapture.str().find("KH0001") != std::string::npos &&
                        outputCapture.str().find("Khong tim thay khach hang") == std::string::npos,
                    "KhachHangMenu phai tim thay KH0001 khi nhap chu thuong");
    }

    // 1b. Test KhachHangMenu: tu choi CCCD sai
    {
        std::istringstream inputSim("1\nKH0000005\nNguoi Sai\n099085012345\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        khMenu.showMenu();

        TEST_ASSERT(khRepo.findById("KH0000005") == nullptr, "KhachHangMenu khong duoc luu CCCD sai");
        TEST_ASSERT(outputCapture.str().find("CCCD khong hop le") != std::string::npos,
                    "KhachHangMenu phai thong bao CCCD khong hop le");
    }

    // 2. Test KhachHangMenu: tu choi xoa khach hang co lien ket khoa ngoai
    {
        std::istringstream inputSim("5\nKH0001\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        khMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("Khong the xoa khach hang") != std::string::npos, "KhachHangMenu phai tu choi xoa KH co thue bao lien ket");
        TEST_ASSERT(khRepo.findById("KH0001") != nullptr, "KH0001 khong duoc bi xoa");
    }

    // 3. Test NapTienMenu: lap phieu nap tien va cong tien vao tai khoan
    {
        std::istringstream inputSim("1\nGD0001\n0981234567\n50000\n1\n15/01/2024\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        ntMenu.showMenu();

        TEST_ASSERT(ntRepo.findById("GD0001") != nullptr, "NapTienMenu phai luu duoc phieu nap tien GD0001");
        ThueBao* updatedTb = tbRepo.findById("0981234567");
        TEST_ASSERT(updatedTb != nullptr && updatedTb->getSoDuTaiKhoan() == 150000.0, "So du thue bao phai duoc cong them 50000 (100k -> 150k)");
    }

    // 4. Test HoaDonMenu: lap hoa don va thanh toan
    {
        std::istringstream inputSim("1\nHD01\n0981234567\n01/2024\n50000\n40000\n3\nHD01\ny\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        HoaDon* inv = hoaDonRepo.findById("HD01");
        TEST_ASSERT(inv != nullptr, "HoaDonMenu phai lap duoc hoa don HD01");
        if (inv) {
            TEST_ASSERT(inv->getTrangThai() == HoaDonConstants::STATUS_PAID, "Hoa don phai duoc cap nhat thanh DaThanhToan sau khi xac nhan");
        }
    }

    std::cin.rdbuf(origCin);
    std::cout.rdbuf(origCout);

    std::remove(KH_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(HD_TEST_FILE.c_str());
    std::remove(NT_TEST_FILE.c_str());
    std::remove(GHICUOCGOI_TEST_FILE.c_str());
    std::remove(INV_TEST_FILE.c_str());
    std::remove(KN_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
}

int main() {
    std::cout << "========================================================\n"
              << "       KTLT AUTOMATED TEST SUITE (VERIFICATION GATE)    \n"
              << "========================================================\n";

    testDate();
    testBirthDateAndAgeValidation();
    testPhoneNumberValidation();
    testCustomerIdValidation();
    testCredentialValidation();
    testNormalizedUtils();
    testIMEILuhn();
    testHopDong();
    testRepository();
    testDisplayHelper();
    testMenuControllers();
    testFileIOTmpfs();
    testKhachHangModel();
    testGoiCuocModel();
    testThueBaoModel();
    testHoaDonAndNapTienModels();
    testNewRepositoriesAndMenus();
    testCDRAndKhieuNaiModels();
    testBTSRegistry();
    testRemainingMenuControllers();

    std::cout << "========================================================\n";
    std::cout << "Ket qua kiem thu: "
              << TestStats::passed << " passed, "
              << TestStats::failed << " failed.\n";
    std::cout << "========================================================\n";

    if (TestStats::failed > 0) {
        return 1;
    }
    std::cout << "TAT CA KIEM THU DA VUOT QUA THANH CONG (100% PASS)!\n";
    return 0;
}
