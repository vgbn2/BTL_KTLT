#include "common.h"

namespace TestStats {
    int passed = 0;
    int failed = 0;
}

// ponytail: execute test function with exception isolation
static void runTest(const char* name, void (*fn)()) {
    try {
        fn();
    } catch (const std::exception& e) {
        TestStats::failed++;
        std::cerr << "  [CRASH] Unhandled exception in " << name << ": " << e.what() << "\n";
    } catch (...) {
        TestStats::failed++;
        std::cerr << "  [CRASH] Unknown exception in " << name << "\n";
    }
}

int main() {
    std::cout << "========================================================\n"
              << "       KTLT AUTOMATED TEST SUITE (VERIFICATION GATE)    \n"
              << "========================================================\n";

    // 1. Shared & Validation suite
    runTest("testDate", testDate);
    runTest("testBirthDateAndAgeValidation", testBirthDateAndAgeValidation);
    runTest("testPhoneNumberValidation", testPhoneNumberValidation);
    runTest("testCustomerIdValidation", testCustomerIdValidation);
    runTest("testCredentialValidation", testCredentialValidation);
    runTest("testNormalizedUtils", testNormalizedUtils);

    // 2. Core Models suite
    runTest("testIMEILuhn", testIMEILuhn);
    runTest("testHopDong", testHopDong);

    // 3. Infrastructure & Shared Tools suite
    runTest("testRepository", testRepository);
    runTest("testDisplayHelper", testDisplayHelper);
    runTest("testMenuControllers", testMenuControllers);
    runTest("testFileIOTmpfs", testFileIOTmpfs);

    // 4. Extended Models & Controllers suite
    runTest("testKhachHangModel", testKhachHangModel);
    runTest("testGoiCuocModel", testGoiCuocModel);
    runTest("testThueBaoModel", testThueBaoModel);
    runTest("testHoaDonAndNapTienModels", testHoaDonAndNapTienModels);
    runTest("testNewRepositoriesAndMenus", testNewRepositoriesAndMenus);
    runTest("testCDRAndKhieuNaiModels", testCDRAndKhieuNaiModels);
    runTest("testSourceFileHeaders", testSourceFileHeaders);
    runTest("testBTSRegistry", testBTSRegistry);
    runTest("testRemainingMenuControllers", testRemainingMenuControllers);

    // 5. Canonical Fixtures Verification
    runTest("testCanonicalFixtureCounts", testCanonicalFixtureCounts);

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
