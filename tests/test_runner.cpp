#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <cstdio>
#include "Date.h"
#include "hopdong.h"
#include "imei.h"
#include "Repository.h"
#include "Exceptions.h"
#include "InputHelper.h"

namespace TestStats {
    int passed = 0;
    int failed = 0;
}

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

    TEST_ASSERT(InputHelper::isValidPhoneNumber("0981234567"), "0981234567 (Viettel) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0912345678"), "0912345678 (VinaPhone) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0903456789"), "0903456789 (MobiFone) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0888123456"), "0888123456 (VinaPhone) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0778901234"), "0778901234 (MobiFone) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0381234567"), "0381234567 (Viettel) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0851234567"), "0851234567 (VinaPhone) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0581234567"), "0581234567 (Vietnamobile) phai hop le");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0591234567"), "0591234567 (Gmobile) phai hop le");

    TEST_ASSERT(!InputHelper::isValidPhoneNumber("098123456"), "So 9 chu so phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("09812345678"), "So 11 chu so phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("1981234567"), "Khong bat dau bang 0 phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("0181234567"), "Dau so 01x cu phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("0281234567"), "Dau so co dinh 02x phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("0481234567"), "Dau so 04x phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("098123456A"), "Chua ky tu chu cai phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("098 123456"), "Chua khoang trang phai bi tu choi");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber(""), "Chuoi rong phai bi tu choi khi khong cho phep ChuaGan");

    TEST_ASSERT(InputHelper::isValidPhoneNumber("ChuaGan", true), "ChuaGan phai hop le khi cho phep unassigned");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("", true), "Chuoi rong phai hop le khi cho phep unassigned");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("ChuaGan", false), "ChuaGan khong the hop le khi bat buoc nhap sdt");
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

    bool threwInvalidPhone = false;
    try {
        tb.ganSIM("012345");
    } catch (const InvalidPhoneNumberException&) {
        threwInvalidPhone = true;
    }
    TEST_ASSERT(threwInvalidPhone, "ganSIM voi sdt sai dinh dang phai nem InvalidPhoneNumberException");

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

    bool threwInvalidPhone = false;
    try {
        HopDong badPhone("HD8888", "KH8888", "12345", "GC001", start, end, "TraSau", "HieuLuc", 100.0);
    } catch (const InvalidPhoneNumberException&) {
        threwInvalidPhone = true;
    }
    TEST_ASSERT(threwInvalidPhone, "HopDong tao voi sdt sai dinh dang phai nem InvalidPhoneNumberException");

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
    TEST_ASSERT(repo.size() == 2, "Repo phai co 2 phan tu");

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
    TEST_ASSERT(reloadedRepo.size() == 1, "Reloaded repo phai co dung 1 ban ghi da luu");
    TEST_ASSERT(reloadedRepo.findById("HD0002") != nullptr, "Reloaded repo phai chua HD0002");

    std::remove(TEST_FILE.c_str());
}

int main() {
    std::cout << "========================================================\n"
              << "       KTLT AUTOMATED TEST SUITE (VERIFICATION GATE)    \n"
              << "========================================================\n";

    testDate();
    testBirthDateAndAgeValidation();
    testPhoneNumberValidation();
    testIMEILuhn();
    testHopDong();
    testRepository();

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
