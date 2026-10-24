#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <fstream>
#include <cstdio>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace TestStats {
    extern int passed;
    extern int failed;
}

// ponytail: RAII guard restoring cin and cout stream buffers automatically
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

// ponytail: RAII guard ensuring temporary files are always cleaned up
struct FileCleanupGuard {
    std::vector<std::string> paths;
    FileCleanupGuard(std::initializer_list<std::string> p) : paths(p) {}
    ~FileCleanupGuard() {
        for (const auto& path : paths) {
            std::remove(path.c_str());
        }
    }
    FileCleanupGuard(const FileCleanupGuard&) = delete;
    FileCleanupGuard& operator=(const FileCleanupGuard&) = delete;
};

#define ASSERT_TRUE(cond, msg) \
    do { \
        if (cond) { \
            TestStats::passed++; \
        } else { \
            TestStats::failed++; \
            std::cerr << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        } \
    } while (0)

#define TEST_ASSERT(cond, msg) ASSERT_TRUE(cond, msg)

// Test module declarations
// 1. Shared & Validation tests (test_shared.cpp)
void testDate();
void testBirthDateAndAgeValidation();
void testPhoneNumberValidation();
void testCustomerIdValidation();
void testCredentialValidation();
void testNormalizedUtils();
void testDisplayHelper();

// 2. Infrastructure tests (test_infrastructure.cpp)
void testRepository();
void testFileIOTmpfs();
void testBTSRegistry();

// 3. Models tests (test_models.cpp)
void testIMEILuhn();
void testHopDong();
void testKhachHangModel();
void testGoiCuocModel();
void testThueBaoModel();
void testHoaDonAndNapTienModels();
void testCDRAndKhieuNaiModels();
void testSourceFileHeaders();

// 4. Menu Controllers tests (test_menus.cpp)
void testMenuControllers();
void testNewRepositoriesAndMenus();
void testRemainingMenuControllers();

// 5. Fixture tests (test_fixtures.cpp)
void testCanonicalFixtureCounts();

#endif // TEST_COMMON_H
