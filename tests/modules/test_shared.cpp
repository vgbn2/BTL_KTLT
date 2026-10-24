#include "common.h"
#include "Date.h"
#include "Normalized.h"
#include "DisplayHelper.h"
#include "khachhang.h"
#include "hopdong.h"
#include "imei.h"
#include "Exceptions.h"

void testDate() {
    std::cout << "[RUN] Kiem thu Date va Leap Year...\n";

    ASSERT_TRUE(Date::isLeapYear(2000) && Date::isLeapYear(2400) && Date::isLeapYear(1600), "Nam 2000, 2400, 1600 chia het cho 400 phai la nam nhuan");
    ASSERT_TRUE(Date::isLeapYear(2024) && Date::isLeapYear(2028) && Date::isLeapYear(2032), "Nam 2024, 2028, 2032 chia het cho 4 khong chia het cho 100 phai la nam nhuan");
    ASSERT_TRUE(!Date::isLeapYear(1900) && !Date::isLeapYear(1800) && !Date::isLeapYear(1700) && !Date::isLeapYear(2100), "Cac nam the ky 1700, 1800, 1900, 2100 khong phai nam nhuan");
    ASSERT_TRUE(!Date::isLeapYear(2023) && !Date::isLeapYear(2025) && !Date::isLeapYear(2026), "Cac nam le khong the la nam nhuan");

    ASSERT_TRUE(Date::isValid(29, 2, 2024) && Date::isValid(28, 2, 2024) && Date::isValid(1, 2, 2024), "29/02 nam nhuan va cac ngay thang 2 hop le");
    ASSERT_TRUE(!Date::isValid(29, 2, 2023) && !Date::isValid(30, 2, 2023) && !Date::isValid(0, 2, 2023), "29/02 nam thuong va ngay 0 phai bi tu choi");
    ASSERT_TRUE(!Date::isValid(31, 4, 2024) && !Date::isValid(31, 6, 2024) && !Date::isValid(31, 9, 2024) && !Date::isValid(31, 11, 2024), "Cac thang 4, 6, 9, 11 khong the co ngay 31");
    ASSERT_TRUE(Date::isValid(31, 12, 2024) && Date::isValid(31, 1, 2024) && Date::isValid(31, 3, 2024) && Date::isValid(31, 5, 2024) && Date::isValid(31, 7, 2024) && Date::isValid(31, 8, 2024) && Date::isValid(31, 10, 2024), "Cac thang 1, 3, 5, 7, 8, 10, 12 co ngay 31 hop le");

    Date d1(15, 1, 2024);
    Date d2(15, 1, 2025);
    Date d3(15, 1, 2024);

    ASSERT_TRUE((d1 < d2) && !(d2 < d1) && !(d1 == d2) && (d1 <= d2), "d1 < d2 phai dam bao tinh bat doi xung va khong bang nhau");
    ASSERT_TRUE((d1 <= d3) && (d3 <= d1) && (d1 == d3) && !(d1 < d3) && !(d3 < d1), "d1 <= d3 va d3 <= d1 phai suy ra d1 == d3");
    ASSERT_TRUE((d1 == d3) && (d3 == d1) && !(d1 != d3), "d1 == d3 phai doi xung va phu dinh toan tu !=");
    ASSERT_TRUE((d2 > d1) && (d2 >= d1) && !(d1 > d2) && !(d2 <= d1), "d2 > d1 phai dong nhat voi !(d2 <= d1)");

    Date parsed = Date::parse("05/04/2024");
    ASSERT_TRUE(parsed.getDay() == 5 && parsed.getMonth() == 4 && parsed.getYear() == 2024, "Parse Date sai gia tri");
    ASSERT_TRUE(parsed.toString() == "05/04/2024" && Date::parse(parsed.toString()) == parsed, "Date toString sai format hoac khong roundtrip");

    bool caughtTrailingDate = false;
    try {
        Date::parse("15/01/2024abc");
    } catch (const InvalidDateException& e) {
        caughtTrailingDate = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtTrailingDate, "Date::parse voi chuoi co ky tu thua phai quang InvalidDateException");

    bool caughtMixedSep = false;
    try {
        Date::parse("15/01-2024");
    } catch (const InvalidDateException& e) {
        caughtMixedSep = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(caughtMixedSep, "Date::parse voi dau phan cach khong dong nhat phai quang InvalidDateException");

    Date parsedNow = Date::parse("now");
    Date actualNow = Date::now();
    ASSERT_TRUE(parsedNow.getDay() == actualNow.getDay() &&
                parsedNow.getMonth() == actualNow.getMonth() &&
                parsedNow.getYear() == actualNow.getYear(),
                "Date::parse('now') phai tra ve ngay hien tai");
    Date parsedNowUpper = Date::parse("NOW");
    ASSERT_TRUE(parsedNowUpper == actualNow, "Date::parse('NOW') phai tra ve ngay hien tai");
}

void testBirthDateAndAgeValidation() {
    std::cout << "[RUN] Kiem thu Ngay sinh (Birth Date) va Do tuoi chu thue bao...\n";

    Date referenceDate(15, 10, 2024);

    Date validBirth(20, 5, 2000);
    ASSERT_TRUE(validBirth.calculateAge(referenceDate) == 24, "Nguoi sinh 20/05/2000 den 15/10/2024 phai 24 tuoi");
    ASSERT_TRUE(Date::isValidBirthDate(validBirth, 14, 120, referenceDate), "Nguoi 24 tuoi phai hop le");

    Date youngValid(15, 10, 2010);
    ASSERT_TRUE(youngValid.calculateAge(referenceDate) == 14, "Nguoi sinh 15/10/2010 den 15/10/2024 dung 14 tuoi");
    ASSERT_TRUE(Date::isValidBirthDate(youngValid, 14, 120, referenceDate), "Dung 14 tuoi phai duoc dang ky thue bao");

    Date tooYoung(16, 10, 2010);
    ASSERT_TRUE(tooYoung.calculateAge(referenceDate) == 13, "Nguoi sinh 16/10/2010 den 15/10/2024 la 13 tuoi (chua den sinh nhat)");
    ASSERT_TRUE(!Date::isValidBirthDate(tooYoung, 14, 120, referenceDate), "13 tuoi phai bi tu choi");

    Date futureDate(1, 1, 2025);
    ASSERT_TRUE(futureDate.isFuture(referenceDate), "Ngay 01/01/2025 phai la tuong lai so voi 15/10/2024");
    ASSERT_TRUE(!Date::isValidBirthDate(futureDate, 14, 120, referenceDate), "Ngay sinh trong tuong lai phai bi tu choi");

    Date tooOld(1, 1, 1901);
    ASSERT_TRUE(!Date::isValidBirthDate(tooOld, 14, 120, referenceDate), "Tuoi > 120 phai bi tu choi");

    bool threwInvalidYear = false;
    try {
        Date invalidYear(1, 1, 1850);
    } catch (const InvalidDateException&) {
        threwInvalidYear = true;
    }
    ASSERT_TRUE(threwInvalidYear, "Nam < 1900 phai nem InvalidDateException");
}

void testPhoneNumberValidation() {
    std::cout << "[RUN] Kiem thu So dien thoai di dong Viet Nam (10 chu so)...\n";

    //kiem tra cac dau so nha mang viet nam
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0981234567"), "0981234567 (Viettel) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0912345678"), "0912345678 (VinaPhone) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0903456789"), "0903456789 (MobiFone) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0888123456"), "0888123456 (VinaPhone) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0778901234"), "0778901234 (MobiFone) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0381234567"), "0381234567 (Viettel) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0851234567"), "0851234567 (VinaPhone) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0581234567"), "0581234567 (Vietnamobile) phai hop le");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("0591234567"), "0591234567 (Gmobile) phai hop le");

    ASSERT_TRUE(!Normalized::isValidPhoneNumber("098123456"), "So 9 chu so phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("09812345678"), "So 11 chu so phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("1981234567"), "Khong bat dau bang 0 phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("8498123456"), "Dau ma quoc gia khong dung chuan 0x phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("0181234567"), "Dau so 01x cu phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("0281234567"), "Dau so co dinh 02x phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("0481234567"), "Dau so 04x phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("0681234567"), "Dau so 06x phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("098123456A"), "Chua ky tu chu cai phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("098 123456"), "Chua khoang trang phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("098-123456"), "Chua dau gach noi phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber(""), "Chuoi rong phai bi tu choi khi khong cho phep ChuaGan");

    ASSERT_TRUE(Normalized::isValidPhoneNumber("ChuaGan", true), "ChuaGan phai hop le khi cho phep unassigned");
    ASSERT_TRUE(Normalized::isValidPhoneNumber("", true), "Chuoi rong phai hop le khi cho phep unassigned");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("ChuaGan", false), "ChuaGan khong the hop le khi bat buoc nhap sdt");
    ASSERT_TRUE(!Normalized::isValidPhoneNumber("", false), "Chuoi rong khong hop le khi khong cho phep unassigned");

}

void testCustomerIdValidation() {
    std::cout << "[RUN] Kiem thu ma khach hang legacy va mo rong...\n";

    ASSERT_TRUE(Normalized::isValidCustomerId("KH0001"), "Ma KH legacy KH0001 phai hop le");
    ASSERT_TRUE(Normalized::NormalizedCustomerId(" kh0001 ") == "KH0001",
                "NormalizedCustomerId phai chuan hoa khoang trang va chu thuong");
    ASSERT_TRUE(Normalized::isValidCustomerId("KH0000001"), "Ma KH mo rong KH0000001 phai hop le");
    ASSERT_TRUE(Normalized::isValidCustomerId("KH9999999"), "Ma KH mo rong KH9999999 phai hop le");
    ASSERT_TRUE(!Normalized::isValidCustomerId("KH00001"), "Ma KH 5 chu so phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidCustomerId("KH10000000"), "Ma KH 8 chu so phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidCustomerId("KX0000001"), "Ma KH sai tien to phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidCustomerId("KH00000A1"), "Ma KH chua ky tu chu phai bi tu choi");

    KhachHang expanded("KH0000001", "Nguoi Mo Rong", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    ASSERT_TRUE(expanded.getMaKhachHang() == "KH0000001", "KhachHang phai giu ma KH mo rong");

    bool caughtBadId = false;
    try {
        KhachHang invalid("KH1", "Nguoi Sai", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    } catch (const AppException&) {
        caughtBadId = true;
    }
    ASSERT_TRUE(caughtBadId, "KhachHang phai tu choi ma KH sai format");
}

void testCredentialValidation() {
    std::cout << "[RUN] Kiem thu CCCD 12 chu so va ma tinh...\n";

    ASSERT_TRUE(Normalized::isValidCredential("079085012345"), "CCCD 079085012345 phai hop le");
    ASSERT_TRUE(Normalized::isValidCredential("079285012345"), "CCCD 079285012345 phai hop le");
    ASSERT_TRUE(!Normalized::isValidCredential("099085012345"), "CCCD voi ma tinh 099 phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidCredential("07908501234"), "CCCD 11 chu so phai bi tu choi");
    ASSERT_TRUE(!Normalized::isValidCredential("079A85012345"), "CCCD chua ky tu chu phai bi tu choi");

    bool caughtConstructor = false;
    try {
        KhachHang invalid("KH0000003", "Nguoi Sai", "CCCD", "099085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    } catch (const InvalidCredentialException&) {
        caughtConstructor = true;
    }
    ASSERT_TRUE(caughtConstructor, "KhachHang constructor phai tu choi CCCD sai");

    KhachHang kh("KH0000004", "Nguoi Dung", "CCCD", "079085012345", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    bool caughtSetter = false;
    try {
        kh.setSoGiayTo("099085012345");
    } catch (const InvalidCredentialException&) {
        caughtSetter = true;
    }
    ASSERT_TRUE(caughtSetter, "KhachHang setter phai tu choi CCCD sai");
    ASSERT_TRUE(!kh.fromFileString("KH-BAD|Nguoi Sai|CCCD|099085012345|10/01/2021|Cuc C06|0981234567|Ha Noi"),
                "KhachHang fromFileString phai tu choi CCCD sai");
}

void testNormalizedUtils() {
    std::cout << "[RUN] Kiem thu Normalized string utils...\n";
    ASSERT_TRUE(Normalized::trim("  hello world  \t\n") == "hello world", "Normalized::trim phai cat khoang trang hai dau");
    ASSERT_TRUE(Normalized::CollapseSpace("  a   b   c  ") == "a b c", "Normalized::CollapseSpace phai thu gon khoang trang");
    ASSERT_TRUE(Normalized::removeDigits("Tran 35267 Duc") == "Tran Duc", "removeDigits phai loai bo chu so");
    ASSERT_TRUE(Normalized::removeSymbols("Tran* Duc") == "Tran Duc", "removeSymbols phai loai bo ky tu cam");
    ASSERT_TRUE(Normalized::NormalizedName("  Nguyen   Van   A!  ") == "Nguyen Van A", "NormalizedName phai chuan hoa ten va loai ky tu cam");
    ASSERT_TRUE(Normalized::toUpper("abcDef") == "ABCDEF", "Normalized::toUpper phai viet hoa toan bo");
    ASSERT_TRUE(Normalized::toLower("ABCdef") == "abcdef", "Normalized::toLower phai viet thuong toan bo");
}

void testDisplayHelper() {
    std::cout << "[RUN] Kiem thu DisplayHelper va format ASCII table/card...\n";

    std::ostringstream ossBorder;
    DisplayHelper::printBorder({6, 8}, ossBorder);
    std::string borderExpected = "+------+--------+\n";
    ASSERT_TRUE(ossBorder.str() == borderExpected, "DisplayHelper printBorder phai dung dinh dang +------+--------+");

    std::ostringstream ossHeader;
    DisplayHelper::printHeader({"Col A", "Col B"}, {7, 8}, {false, true}, ossHeader);
    std::string headerStr = ossHeader.str();
    ASSERT_TRUE(headerStr.find("+-------+--------+") != std::string::npos, "Header phai co duong vien tren va duoi");
    ASSERT_TRUE(headerStr.find("| Col A |  Col B |") != std::string::npos, "Header phai can chinh dung le trai va le phai");

    std::ostringstream ossRow;
    DisplayHelper::printRow({"val1", "val2"}, {7, 8}, {false, true}, ossRow);
    std::string rowStr = ossRow.str();
    ASSERT_TRUE(rowStr == "| val1  |   val2 |\n", "Row phai can chinh dung le va do rong");

    std::ostringstream ossCard;
    DisplayHelper::printCard({{"Key A", "Value A"}, {"Key B", "Value B"}}, 10, ossCard);
    std::string cardStr = ossCard.str();
    ASSERT_TRUE(cardStr.find("Key A     : Value A") != std::string::npos, "Card phai in nhan va gia tri dung dinh dang");

    HopDong tableItem("HD-TABLE", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    std::ostringstream ossTable;
    DisplayHelper::printTable(std::vector<HopDong>{tableItem}, ossTable);
    ASSERT_TRUE(ossTable.str().find("HD-TABLE") != std::string::npos,
                "printTable phai forward output stream vao display methods");

    //kiem thu tich hop model
    HopDong hd("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    ThietBiIMEI tb("860123456789014", "iPhone 15", "Apple", "0981234567", Date(15, 1, 2024), "HoatDong", "BTS-HN-001");

    std::ostringstream ossModel;
    {
        StreamRedirectGuard guard;
        std::cout.rdbuf(ossModel.rdbuf());
        hd.displayHeader();
        hd.displayRow();
        hd.displayDetail();
        tb.displayHeader();
        tb.displayRow();
        tb.displayDetail();
    }

    std::string modelOut = ossModel.str();
    ASSERT_TRUE(!modelOut.empty(), "Model display methods phai tao ra output qua DisplayHelper");
    ASSERT_TRUE(modelOut.find("HD0001") != std::string::npos, "Output phai chua Ma HD0001");
    ASSERT_TRUE(modelOut.find("860123456789014") != std::string::npos, "Output phai chua Ma IMEI");
}

