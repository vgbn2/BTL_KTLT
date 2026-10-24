#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <limits>

#include "lib/shared/Normalized.h"
#include "lib/shared/Date.h"
#include "lib/shared/InputHelper.h"
#include "lib/shared/Repository.h"
#include "lib/shared/Exceptions.h"
#include "lib/models/khachhang.h"
#include "lib/models/goicuoc.h"
#include "lib/models/thuebao.h"
#include "lib/models/hopdong.h"
#include "lib/models/imei.h"
#include "lib/models/hoadon.h"
#include "lib/models/naptien.h"
#include "lib/menus/KhachHangMenu.h"
#include "lib/menus/GoiCuocMenu.h"
#include "lib/menus/ThueBaoMenu.h"
#include "lib/menus/HopDongMenu.h"
#include "lib/menus/ThietBiIMEIMenu.h"
#include "lib/menus/NapTienMenu.h"
#include "lib/menus/HoaDonMenu.h"

static int g_fuzzPassed = 0;
static int g_fuzzFailed = 0;

#define FUZZ_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            g_fuzzPassed++; \
        } else { \
            g_fuzzFailed++; \
            std::cerr << "  [FUZZ FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        } \
    } while (0)

struct StreamGuard {
    std::streambuf* cinBuf;
    std::streambuf* coutBuf;
    StreamGuard() : cinBuf(std::cin.rdbuf()), coutBuf(std::cout.rdbuf()) {}
    ~StreamGuard() {
        std::cin.rdbuf(cinBuf);
        std::cout.rdbuf(coutBuf);
        std::cin.clear();
    }
};

void fuzzCustomerIds() {
    std::cout << "[FUZZ] Ingestion of malformed Customer IDs...\n";
    std::vector<std::string> malformedIds = {
        "", " ", "   ", "\t", "\n",
        "K", "KH", "KH1", "KH12", "KH123",
        "KH00001", "KH00002", "KH00010", // 5-digit format rejected
        "KH000001", "KH000002",           // 6-digit format rejected
        "KH10000000", "KH99999999",       // 8-digit format rejected
        "KH000000000000000000001",        // extreme length
        "XX0001", "AB1234", "CUST001", "ID0001",
        "KH000A", "KH000!", "KH000 ", "KH 0001",
        "KH00000A1", "KH0000!@#", "KH00000 1",
        "KH0001; DROP TABLE", "KH0001' OR '1'='1",
        "KH%s%d%n", "KH\x00\x01\x02", "KH\xFF\xFE"
    };

    for (const auto& badId : malformedIds) {
        FUZZ_ASSERT(!Normalized::isValidCustomerId(badId),
                    ("Malformed Customer ID must be rejected: '" + badId + "'").c_str());
        
        bool caught = false;
        try {
            KhachHang kh(badId, "Test Person", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
        } catch (const AppException&) {
            caught = true;
        }
        FUZZ_ASSERT(caught, ("KhachHang constructor must reject bad ID: '" + badId + "'").c_str());
    }

    // Valid Customer IDs must pass
    std::vector<std::string> validIds = {
        "KH0001", "KH0010", "KH9999",
        "KH0000001", "KH1234567", "KH9999999"
    };
    for (const auto& okId : validIds) {
        FUZZ_ASSERT(Normalized::isValidCustomerId(okId),
                    ("Valid Customer ID must pass: '" + okId + "'").c_str());
    }
}

void fuzzCredentials() {
    std::cout << "[FUZZ] Ingestion of malformed Credentials (CCCD 12-digits)...\n";
    std::vector<std::string> malformedCCCDs = {
        "", " ", "123", "123456789", "12345678901",       // short (< 12)
        "07908501234",                                    // 11 digits
        "0790850123456", "0790850123456789",             // long (> 12)
        "07908501234A", "079A85012345", "A79085012345", // letters
        "079-085-01234", "079 085 01234",                 // separators
        "079!@#$%^&*1", "07908501234\n",
        "099085012345", "999085012345",                   // invalid province code 099, 999
        "000085012345", "100085012345",                   // invalid province code 000, 100
        "079685012345"                                    // invalid century code 6
    };

    for (const auto& badCCCD : malformedCCCDs) {
        FUZZ_ASSERT(!Normalized::isValidCredential(badCCCD),
                    ("Malformed CCCD must be rejected: '" + badCCCD + "'").c_str());

        bool caught = false;
        try {
            KhachHang kh("KH0001", "Test Person", "CCCD", badCCCD, "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
        } catch (const InvalidCredentialException&) {
            caught = true;
        } catch (const AppException&) {
            caught = true;
        }
        FUZZ_ASSERT(caught, ("KhachHang constructor must reject bad CCCD: '" + badCCCD + "'").c_str());
    }

    // Valid CCCD samples (001 = Ha Noi, 079 = TP HCM, 038 = Thanh Hoa)
    std::vector<std::string> validCCCDs = {
        "001200012345", "079085012345", "038201004567", "001302009999"
    };
    for (const auto& okCCCD : validCCCDs) {
        FUZZ_ASSERT(Normalized::isValidCredential(okCCCD),
                    ("Valid CCCD must pass: '" + okCCCD + "'").c_str());
    }
}

void fuzzPhoneNumbers() {
    std::cout << "[FUZZ] Ingestion of malformed Phone Numbers...\n";
    std::vector<std::string> malformedPhones = {
        "", " ", "098", "0981234", "098123456", // short
        "09812345678", "098123456789",          // long (> 10)
        "0123456789", "0243456789", "0081234567", // bad prefixes (01, 02, 00)
        "0481234567", "0681234567",              // bad prefixes (04, 06)
        "098123456A", "098A234567", "A981234567", // letters
        "098-123-4567", "098 123 4567",          // separators
        "+84981234567", "84981234567",           // country code not raw national
        "098!@#$%^&", "098123456\0"
    };

    for (const auto& badPhone : malformedPhones) {
        FUZZ_ASSERT(!Normalized::isValidPhoneNumber(badPhone),
                    ("Malformed phone must be rejected: '" + badPhone + "'").c_str());
    }

    std::vector<std::string> validPhones = {
        "0981234567", "0381234567", "0581234567", "0781234567", "0881234567"
    };
    for (const auto& okPhone : validPhones) {
        FUZZ_ASSERT(Normalized::isValidPhoneNumber(okPhone),
                    ("Valid phone must pass: '" + okPhone + "'").c_str());
    }
}

void fuzzDates() {
    std::cout << "[FUZZ] Ingestion of malformed Dates and leap year boundaries...\n";
    std::vector<std::string> malformedDates = {
        "", "not_a_date", "32/01/2024", "00/01/2024", "15/00/2024", "15/13/2024",
        "31/04/2024", "31/06/2024", "31/09/2024", "31/11/2024", // 30-day months
        "29/02/2023", "30/02/2024",                              // Feb invalid
        "29/02/1900", "29/02/2100",                              // Non-leap century years
        "2024/01/15", "15/01-2024", "15.01.2024",                // invalid separators
        "99/99/9999", "-1/05/2024", "15/-5/2024"
    };

    for (const auto& badDate : malformedDates) {
        bool caught = false;
        try {
            Date d = Date::parse(badDate);
            (void)d;
        } catch (const std::exception&) {
            caught = true;
        }
        FUZZ_ASSERT(caught, ("Date::parse must reject bad date: '" + badDate + "'").c_str());
    }

    // Valid leap year century (2000) and quad-year (2024)
    try {
        Date d2000 = Date::parse("29/02/2000");
        FUZZ_ASSERT(d2000.getDay() == 29 && d2000.getMonth() == 2, "29/02/2000 leap century must pass");
        Date d2024 = Date::parse("29/02/2024");
        FUZZ_ASSERT(d2024.getDay() == 29 && d2024.getMonth() == 2, "29/02/2024 leap year must pass");
    } catch (...) {
        FUZZ_ASSERT(false, "Valid leap years must not throw");
    }
}

void fuzzIMEIs() {
    std::cout << "[FUZZ] Ingestion of malformed IMEIs and 3GPP Luhn check digits...\n";
    std::vector<std::string> badLengthOrChar = {
        "", "123", "86012345678901", "8601234567890123", // != 15 digits
        "86012345678901A", "86012345678901!", "86012345 678901"
    };
    for (const auto& bad : badLengthOrChar) {
        FUZZ_ASSERT(!ThietBiIMEI::validateLuhn(bad),
                    ("IMEI with bad length/char must be rejected: '" + bad + "'").c_str());
    }

    // Valid prefix "86012345678901" -> check digit is 4 ("860123456789014")
    std::string prefix14 = "86012345678901";
    for (int d = 0; d <= 9; ++d) {
        std::string candidate = prefix14 + std::to_string(d);
        if (d == 4) {
            FUZZ_ASSERT(ThietBiIMEI::validateLuhn(candidate), "Luhn 860123456789014 must pass");
        } else {
            FUZZ_ASSERT(!ThietBiIMEI::validateLuhn(candidate),
                        ("Corrupted Luhn check digit " + std::to_string(d) + " must fail").c_str());
        }
    }
}

void fuzzInputHelperLoopRecovery() {
    std::cout << "[FUZZ] Stream injection: InputHelper loop recovery under malformed inputs...\n";
    StreamGuard guard;

    // 1. getInt with strings, floats, out of bounds, followed by valid 5
    {
        std::istringstream badStream("abc\n-1\n999\n5\n");
        std::ostringstream dummyOut;
        std::cin.rdbuf(badStream.rdbuf());
        std::cout.rdbuf(dummyOut.rdbuf());

        int res = InputHelper::getInt("Choice [0-10]: ", 0, 10);
        FUZZ_ASSERT(res == 5, "InputHelper::getInt must recover after malformed inputs and return 5");
    }

    // 2. getDouble with bad chars, negative, followed by valid 150000.0
    {
        std::istringstream badStream("not_a_num\n-500\n150000.0\n");
        std::ostringstream dummyOut;
        std::cin.rdbuf(badStream.rdbuf());
        std::cout.rdbuf(dummyOut.rdbuf());

        double res = InputHelper::getDouble("Price: ", 0.0);
        FUZZ_ASSERT(res == 150000.0, "InputHelper::getDouble must recover after malformed inputs and return 150000.0");
    }

    // 3. getPhoneNumber with malformed phones followed by valid 0981234567
    {
        std::istringstream badStream("098\n0123456789\n098-123\n0981234567\n");
        std::ostringstream dummyOut;
        std::cin.rdbuf(badStream.rdbuf());
        std::cout.rdbuf(dummyOut.rdbuf());

        std::string res = InputHelper::getPhoneNumber("Phone: ", false);
        FUZZ_ASSERT(res == "0981234567", "InputHelper::getPhoneNumber must recover after malformed phones");
    }

    // 4. getConfirm with invalid answers followed by 'y'
    {
        std::istringstream badStream("invalid\n123\nmaybe\ny\n");
        std::ostringstream dummyOut;
        std::cin.rdbuf(badStream.rdbuf());
        std::cout.rdbuf(dummyOut.rdbuf());

        bool res = InputHelper::getConfirm("Confirm?");
        FUZZ_ASSERT(res == true, "InputHelper::getConfirm must recover after malformed answers and return true");
    }
}

void fuzzMenuAdversarialRejection() {
    std::cout << "[FUZZ] Adversarial injections into Menu controllers...\n";
    StreamGuard guard;

    std::string testKhFile = "/dev/shm/fuzz_kh.txt";
    std::string testTbFile = "/dev/shm/fuzz_tb.txt";
    std::string testHdFile = "/dev/shm/fuzz_hd.txt";
    std::string testGcFile = "/dev/shm/fuzz_gc.txt";

    Repository<KhachHang> khRepo(testKhFile);
    khRepo.clear();
    KhachHang kh1("KH0001", "Tran Van An", "CCCD", "001200012345", "10/01/2020", "Cuc C06", "0981234567", "Ha Noi");
    khRepo.add(kh1);

    Repository<ThueBao> tbRepo(testTbFile);
    tbRepo.clear();
    ThueBao tb1("0981234567", "TraTruoc", "KH0001", "GC001", "15/01/2024", "HoatDong", 50000.0);
    tbRepo.add(tb1);

    Repository<HopDong> hdRepo(testHdFile);
    hdRepo.clear();

    Repository<GoiCuoc> gcRepo(testGcFile);
    gcRepo.clear();
    GoiCuoc gc1("GC001", "Goi Chuan", 50000.0, 10, 100, 30);
    gcRepo.add(gc1);

    KhachHangMenu khMenu(khRepo, tbRepo, hdRepo);

    // 1. Ingestion of duplicate ID KH0001 into KhachHangMenu -> must be rejected
    {
        std::istringstream inputSim("1\nKH0001\n0\n"); // Add -> duplicate KH0001 -> Back
        std::ostringstream outputCap;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCap.rdbuf());

        khMenu.showMenu();
        std::string out = outputCap.str();
        FUZZ_ASSERT(out.find("[CANH BAO] Ma khach hang da ton tai") != std::string::npos,
                    "Duplicate Customer ID must be caught and warned");
        FUZZ_ASSERT(khRepo.size() == 1, "Duplicate customer must not increase store size");
    }

    // 2. Ingestion of malformed Customer ID format into KhachHangMenu -> rejected
    {
        std::istringstream inputSim("1\nINVALID_ID\n0\n");
        std::ostringstream outputCap;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCap.rdbuf());

        khMenu.showMenu();
        std::string out = outputCap.str();
        FUZZ_ASSERT(out.find("[LOI] Ma khach hang phai co dang") != std::string::npos,
                    "Malformed Customer ID must be rejected by KhachHangMenu");
    }

    // 3. Ingestion of malformed CCCD into KhachHangMenu -> rejected
    {
        std::istringstream inputSim("1\nKH0002\nNguyen Fuzz\n999000111222\n0\n"); // 999 = bad province
        std::ostringstream outputCap;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCap.rdbuf());

        khMenu.showMenu();
        std::string out = outputCap.str();
        FUZZ_ASSERT(out.find("[LOI] So CCCD khong hop le") != std::string::npos,
                    "Invalid CCCD with non-existent province must be rejected");
        FUZZ_ASSERT(khRepo.findById("KH0002") == nullptr, "Rejected customer must not exist in store");
    }

    // 4. Ingestion of foreign key violation in ThueBaoMenu (non-existent customer)
    {
        Repository<ThietBiIMEI> imeiRepo("/dev/shm/dummy_imei.txt");
        ThueBaoMenu tbMenu(tbRepo, gcRepo, khRepo, imeiRepo);

        // Add subscriber -> phone -> type -> non-existent KH9999999
        std::istringstream inputSim("1\n0989999999\n1\nKH9999999\n0\n");
        std::ostringstream outputCap;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCap.rdbuf());

        tbMenu.showMenu();
        std::string out = outputCap.str();
        FUZZ_ASSERT(out.find("[LOI] Ma khach hang") != std::string::npos ||
                    out.find("khong ton tai") != std::string::npos,
                    "Subscriber pointing to non-existent KhachHang must fail foreign key check");
    }
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "  KTLT ADVERSARIAL MALFORMED-INPUT & INGESTION FUZZER TEST SUITE    \n";
    std::cout << "====================================================================\n";

    fuzzCustomerIds();
    fuzzCredentials();
    fuzzPhoneNumbers();
    fuzzDates();
    fuzzIMEIs();
    fuzzInputHelperLoopRecovery();
    fuzzMenuAdversarialRejection();

    std::cout << "====================================================================\n";
    std::cout << "Ket qua fuzzer: " << g_fuzzPassed << " passed, " << g_fuzzFailed << " failed.\n";
    std::cout << "====================================================================\n";

    if (g_fuzzFailed > 0) {
        std::cerr << "[FAILED] Co loi phat hien trong kiem thu adversarial malformed input!\n";
        return 1;
    }
    std::cout << "TAT CA CAC BAI KIEM THU MALFORMED VA ADVERSARIAL DEU VUOT QUA (100% PASS)!\n";
    return 0;
}
