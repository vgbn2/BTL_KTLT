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
#include "Normalized.h"
#include "DisplayHelper.h"
#include "HopDongMenu.h"
#include "ThietBiIMEIMenu.h"
#include <sstream>

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

    // ponytail: direct verification of Normalized::isValidPhoneNumber across VN telco prefixes
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

    // Kiem thu uy quyen InputHelper::isValidPhoneNumber
    TEST_ASSERT(InputHelper::isValidPhoneNumber("0981234567"), "InputHelper uy quyen Normalized phai hop le");
    TEST_ASSERT(!InputHelper::isValidPhoneNumber("0123456789"), "InputHelper uy quyen Normalized phai tu choi 01x");
    TEST_ASSERT(InputHelper::isValidPhoneNumber("ChuaGan", true), "InputHelper uy quyen Normalized hop le voi ChuaGan");
}

void testNormalizedUtils() {
    std::cout << "[RUN] Kiem thu Normalized string utils...\n";
    TEST_ASSERT(Normalized::trim("  hello world  \t\n") == "hello world", "Normalized::trim phai cat khoang trang hai dau");
    TEST_ASSERT(Normalized::Trim("  test  ") == "test", "Normalized::Trim alias phai hoat dong");
    TEST_ASSERT(Normalized::CollapseSpace("  a   b   c  ") == "a b c", "Normalized::CollapseSpace phai thu gon khoang trang");
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

    // ponytail: test ThietBiIMEI constructor & mutator phone validation against invalid prefixes/lengths
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

    // ponytail: test HopDong constructor & mutator phone validation against invalid prefixes/lengths
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

    // Kiem thu tich hop voi Model
    HopDong hd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    ThietBiIMEI tb("860123456789014", "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");

    // Redirect cout tam thoi de xac nhan khong crash va output non-empty
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

    Repository<HopDong> hdRepo(HD_TEST_FILE);
    hdRepo.clear();
    HopDong sampleHd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    hdRepo.add(sampleHd);

    Repository<ThietBiIMEI> imeiRepo(IMEI_TEST_FILE);
    imeiRepo.clear();
    ThietBiIMEI sampleImei("860123456789014", "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");
    imeiRepo.add(sampleImei);

    HopDongMenu hdMenu(hdRepo);
    ThietBiIMEIMenu imeiMenu(imeiRepo);

    std::streambuf* origCin = std::cin.rdbuf();
    std::streambuf* origCout = std::cout.rdbuf();

    // Test 1: HopDongMenu [0] Quay lai menu chinh
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

    // Test 2: ThietBiIMEIMenu [0] Quay lai menu chinh
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

    // Test 3: HopDongMenu Submenu Tim kiem -> Quay lai [0] -> Thoat [0]
    {
        std::istringstream inputSim("3\n0\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("TIM KIEM HOP DONG") != std::string::npos, "HopDongMenu timKiemHopDong phai vao submenu");
    }

    // Test 4: HopDongMenu Cap nhat -> Huy bo [0] -> Thoat [0]
    {
        std::istringstream inputSim("5\nHD0001\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "HopDongMenu cap nhat choice 0 phai thong bao huy bo");
    }

    // Test 5: ThietBiIMEIMenu Cap nhat -> Huy bo [0] -> Thoat [0]
    {
        std::istringstream inputSim("5\n860123456789014\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        TEST_ASSERT(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "ThietBiIMEIMenu cap nhat choice 0 phai thong bao huy bo");
    }

    // Test 6: InputHelper EOF safety
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

    // Restore cout & cin
    std::cin.rdbuf(origCin);
    std::cout.rdbuf(origCout);

    std::remove(HD_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
}

int main() {
    std::cout << "========================================================\n"
              << "       KTLT AUTOMATED TEST SUITE (VERIFICATION GATE)    \n"
              << "========================================================\n";

    testDate();
    testBirthDateAndAgeValidation();
    testPhoneNumberValidation();
    testNormalizedUtils();
    testIMEILuhn();
    testHopDong();
    testRepository();
    testDisplayHelper();
    testMenuControllers();

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
