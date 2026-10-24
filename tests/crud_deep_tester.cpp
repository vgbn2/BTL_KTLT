#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cassert>
#include <unistd.h>
#include <sys/stat.h>

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

static int g_testPassed = 0;
static int g_testFailed = 0;

#define CRUD_ASSERT(cond, msg) \
    do { \
        if (cond) { \
            g_testPassed++; \
            std::cout << "  [PASS] " << msg << "\n"; \
        } else { \
            g_testFailed++; \
            std::cerr << "  [FAIL] " << msg << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        } \
    } while (0)

struct TestSandbox {
    std::string dir;
    std::string khFile;
    std::string gcFile;
    std::string tbFile;
    std::string hdFile;
    std::string imeiFile;

    TestSandbox() {
        dir = "/dev/shm/ktlt_crud_test_" + std::to_string(getpid());
        mkdir(dir.c_str(), 0777);
        khFile = dir + "/khachhang.txt";
        gcFile = dir + "/CacGoiCuoc.txt";
        tbFile = dir + "/thuebao.txt";
        hdFile = dir + "/hopdong.txt";
        imeiFile = dir + "/imei.txt";

        // Create empty files
        std::ofstream(khFile).close();
        std::ofstream(gcFile).close();
        std::ofstream(tbFile).close();
        std::ofstream(hdFile).close();
        std::ofstream(imeiFile).close();
    }

    ~TestSandbox() {
        unlink(khFile.c_str());
        unlink(gcFile.c_str());
        unlink(tbFile.c_str());
        unlink(hdFile.c_str());
        unlink(imeiFile.c_str());
        rmdir(dir.c_str());
    }
};

void driveMenu(const std::string& input, std::function<void()> func) {
    std::istringstream in(input);
    std::ostringstream out;
    std::streambuf* cinBuf = std::cin.rdbuf(in.rdbuf());
    std::streambuf* coutBuf = std::cout.rdbuf(out.rdbuf());

    try {
        func();
    } catch (...) {
        std::cin.rdbuf(cinBuf);
        std::cout.rdbuf(coutBuf);
        std::cin.clear();
        throw;
    }

    std::cin.rdbuf(cinBuf);
    std::cout.rdbuf(coutBuf);
    std::cin.clear();
}

// =========================================================================
// 1. KIEM THU U TRONG CRUD: CAP NHAT VA TRAO DOI THONG TIN DINH DANH (SWAP)
// =========================================================================
void testUpdateAndCredentialSwap() {
    std::cout << "\n====================================================================\n";
    std::cout << "  [TEST 1] UPDATE (U) & CREDENTIAL SWAPPING (CCCD / PHONE NUMBERS)  \n";
    std::cout << "====================================================================\n";

    TestSandbox box;
    Repository<KhachHang> khRepo(box.khFile);
    Repository<ThueBao> tbRepo(box.tbFile);
    Repository<HopDong> hdRepo(box.hdFile);

    // Seed 2 customers
    std::string cccdA = "001200000001";
    std::string cccdB = "079085000002";
    std::string phoneA = "0981111111";
    std::string phoneB = "0982222222";

    KhachHang kh1("KH0001", "Nguyen Van A", "CCCD", cccdA, "10/01/2021", "Cuc C06", phoneA, "Ha Noi");
    KhachHang kh2("KH0002", "Tran Thi B", "CCCD", cccdB, "15/02/2021", "Cuc C06", phoneB, "TP HCM");
    khRepo.add(kh1);
    khRepo.add(kh2);

    CRUD_ASSERT(khRepo.size() == 2, "Seed 2 customers in repository");

    // Case 1.1: Direct duplicate personal credential check during update
    // Testing what happens when updating KH0001's contact phone to KH0002's phone (phoneB)
    KhachHangMenu khMenu(khRepo, tbRepo, hdRepo);
    // Menu choice 4 = Cap nhat, ma KH = KH0001, ho ten = bo trong, sdt moi = 0982222222 (trung voi KH0002), dia chi = bo trong
    driveMenu("4\nKH0001\n\n" + phoneB + "\n\n0\n", [&]() { khMenu.showMenu(); });

    KhachHang* checkKh1 = khRepo.findById("KH0001");
    CRUD_ASSERT(checkKh1 != nullptr, "KH0001 exists after update attempt");
    std::cout << "  [EMPIRICAL PROBE] Contact Phone after duplicate update: " << checkKh1->getSoDienThoai() << "\n";
    if (checkKh1->getSoDienThoai() == phoneB) {
        std::cout << "  [OBSERVATION] Secondary credential (contact phone) update was accepted without exception.\n"
                  << "  [REASON] Repository<T> enforces uniqueness on Primary Key (MaKH) only; secondary fields allow shared contacts.\n";
    } else {
        std::cout << "  [OBSERVATION] Secondary credential duplicate was intercepted and excepted.\n";
    }

    // Testing duplicate CCCD update on model layer:
    bool duplicateCccdExcepted = false;
    try {
        // Attempting to set KH0001's CCCD to KH0002's CCCD
        checkKh1->setSoGiayTo(cccdB);
        // Note: setSoGiayTo validates format (12 digits, valid province, century).
        // Since cccdB is valid format, does model-level setSoGiayTo throw?
    } catch (const AppException& e) {
        duplicateCccdExcepted = true;
    }
    std::cout << "  [EMPIRICAL PROBE] Model setSoGiayTo duplicate CCCD excepted: "
              << (duplicateCccdExcepted ? "YES (Threw Exception)" : "NO (Accepted - format valid)") << "\n";
    CRUD_ASSERT(!duplicateCccdExcepted, "setSoGiayTo validates format/province/century; entity model is decoupled from repo store");

    // Restore checkKh1 CCCD to cccdA for subsequent swap test
    checkKh1->setSoGiayTo(cccdA);
    checkKh1->setSoDienThoai(phoneA);
    khRepo.update("KH0001", *checkKh1);

    // Case 1.2: Invalid Credential rollback during update
    // Attempting to set an invalid CCCD (province 099 or century 6)
    bool threwInvalidCccd = false;
    try {
        checkKh1->setSoGiayTo("099085012345"); // invalid province 099
    } catch (const InvalidCredentialException&) {
        threwInvalidCccd = true;
    } catch (const AppException&) {
        threwInvalidCccd = true;
    }
    CRUD_ASSERT(threwInvalidCccd, "Setting invalid province CCCD (099...) must throw InvalidCredentialException");
    CRUD_ASSERT(checkKh1->getSoGiayTo() == cccdA, "Original CCCD unchanged after failed update (Rollback)");

    // Case 1.3: Credential Swapping between 2 customers (3-step exchange)
    // Step 1: KH0001 -> CCCD_TEMP ("038200000003")
    std::string cccdTemp = "038200000003";
    checkKh1->setSoGiayTo(cccdTemp);
    khRepo.update("KH0001", *checkKh1);

    // Step 2: KH0002 -> cccdA ("001200000001")
    KhachHang* checkKh2 = khRepo.findById("KH0002");
    checkKh2->setSoGiayTo(cccdA);
    khRepo.update("KH0002", *checkKh2);

    // Step 3: KH0001 -> cccdB ("079085000002")
    checkKh1->setSoGiayTo(cccdB);
    khRepo.update("KH0001", *checkKh1);

    // Verify swap outcome
    Repository<KhachHang> verifyRepo(box.khFile);
    verifyRepo.loadFromFile();
    KhachHang* vKh1 = verifyRepo.findById("KH0001");
    KhachHang* vKh2 = verifyRepo.findById("KH0002");

    CRUD_ASSERT(vKh1 != nullptr && vKh1->getSoGiayTo() == cccdB,
                "KH0001 now safely holds cccdB (079085000002) after 3-step swap");
    CRUD_ASSERT(vKh2 != nullptr && vKh2->getSoGiayTo() == cccdA,
                "KH0002 now safely holds cccdA (001200000001) after 3-step swap");
    CRUD_ASSERT(verifyRepo.size() == 2, "Repository integrity intact after credential swap");
}

// =========================================================================
// 2. KIEM THU D TRONG CRUD: XOA DU LIEU QUA CHINH CAC MENU CUA UNG DUNG
// =========================================================================
void testDeleteViaAppMenus() {
    std::cout << "\n====================================================================\n";
    std::cout << "  [TEST 2] DELETION (D) IN CRUD USING APP MENUS & INTEGRITY GATES   \n";
    std::cout << "====================================================================\n";

    TestSandbox box;
    Repository<KhachHang> khRepo(box.khFile);
    Repository<GoiCuoc> gcRepo(box.gcFile);
    Repository<ThueBao> tbRepo(box.tbFile);
    Repository<HopDong> hdRepo(box.hdFile);
    Repository<ThietBiIMEI> imeiRepo(box.imeiFile);

    // Step 0: Add a complete cluster of entities through the application menus
    std::string testKh = "KH9999991";
    std::string testGc = "GC_DEL_01";
    std::string testPhone = "0989999991";
    std::string testHd = "HD_DEL_01";
    // 35894109123456 -> check digit 5 -> 358941091234565 (valid Luhn)
    std::string testImei = "860123456789014";

    KhachHangMenu khMenu(khRepo, tbRepo, hdRepo);
    GoiCuocMenu gcMenu(gcRepo);
    ThueBaoMenu tbMenu(tbRepo, gcRepo, khRepo, imeiRepo);
    HopDongMenu hdMenu(hdRepo, khRepo, tbRepo, gcRepo);
    ThietBiIMEIMenu imeiMenu(imeiRepo, tbRepo);

    // 1. Add Customer via KhachHangMenu
    driveMenu("1\n" + testKh + "\nTest Delete Customer\n001200999991\n10/01/2021\nCuc C06\n" + testPhone + "\nHa Noi\n0\n",
              [&]() { khMenu.showMenu(); });
    CRUD_ASSERT(khRepo.findById(testKh) != nullptr, "Created test Customer via KhachHangMenu");

    // 2. Add Package via GoiCuocMenu
    driveMenu("1\n" + testGc + "\nGoi Delete Test\n50000\n5\n100\n30\n0\n",
              [&]() { gcMenu.showMenu(); });
    CRUD_ASSERT(gcRepo.findById(testGc) != nullptr, "Created test Package via GoiCuocMenu");

    // 3. Add Subscriber via ThueBaoMenu
    driveMenu("1\n" + testPhone + "\n1\n" + testKh + "\n" + testGc + "\n15/01/2024\n50000\n0\n",
              [&]() { tbMenu.showMenu(); });
    CRUD_ASSERT(tbRepo.findById(testPhone) != nullptr, "Created test Subscriber via ThueBaoMenu");

    // 4. Add Contract via HopDongMenu
    driveMenu("1\n" + testHd + "\n" + testKh + "\n" + testPhone + "\n" + testGc + "\n15/01/2024\n1\n50000\n\n0\n",
              [&]() { hdMenu.showMenu(); });
    CRUD_ASSERT(hdRepo.findById(testHd) != nullptr, "Created test Contract via HopDongMenu");

    // 5. Add IMEI Device via ThietBiIMEIMenu
    driveMenu("1\n1\n" + testImei + "\nDelete Model\nDelete Brand\n" + testPhone + "\n1\nBTS-DEL-01\n\n0\n",
              [&]() { imeiMenu.showMenu(); });
    CRUD_ASSERT(imeiRepo.findById(testImei) != nullptr, "Created test IMEI Device via ThietBiIMEIMenu");

    // =====================================================================
    // GIAI DOAN A: KIEM THU XOA BI CHAN (REFERENTIAL INTEGRITY BLOCK)
    // =====================================================================
    std::cout << "\n--- Testing Delete Referential Integrity Gates (Fail-Closed) ---\n";

    // A.1: Thu xoa KhachHang khi con ThueBao va HopDong -> PHAI BI TU CHOI
    // KhachHangMenu::MENU_DELETE = 5
    driveMenu("5\n" + testKh + "\ny\n0\n", [&]() { khMenu.showMenu(); });
    CRUD_ASSERT(khRepo.findById(testKh) != nullptr,
                "GATE PASS: Customer deletion REJECTED because active subscriber/contract exists");

    // A.2: Thu xoa ThueBao khi con ThietBiIMEI lien ket -> PHAI BI TU CHOI
    // ThueBaoMenu::MENU_DELETE = 7
    driveMenu("7\n" + testPhone + "\ny\n\n0\n", [&]() { tbMenu.showMenu(); });
    CRUD_ASSERT(tbRepo.findById(testPhone) != nullptr,
                "GATE PASS: Subscriber deletion REJECTED because linked IMEI device exists");

    // =====================================================================
    // GIAI DOAN B: XOA THU TU NGUOC (LEAF -> ROOT) QUA CHINH CAC MENU APP
    // =====================================================================
    std::cout << "\n--- Testing Successful Deletion in Topological Order (App Menus) ---\n";

    // B.1: Xoa IMEI Device truoc (la thiet bi la) qua ThietBiIMEIMenu
    // ThietBiIMEIMenu::MENU_DELETE = 7 (choice 7, imei, confirm y, pause inside, pause menu, back 0)
    driveMenu("7\n" + testImei + "\ny\n\n\n0\n", [&]() { imeiMenu.showMenu(); });
    CRUD_ASSERT(imeiRepo.findById(testImei) == nullptr,
                "App Menu SUCCESS: IMEI device deleted and removed from in-memory store");
    {
        Repository<ThietBiIMEI> verifyImei(box.imeiFile);
        verifyImei.loadFromFile();
        CRUD_ASSERT(verifyImei.findById(testImei) == nullptr,
                    "App Menu SUCCESS: IMEI device permanently removed from disk file");
    }

    // B.2: Xoa HopDong qua HopDongMenu
    // HopDongMenu::MENU_DELETE = 6
    driveMenu("6\n" + testHd + "\ny\n\n0\n", [&]() { hdMenu.showMenu(); });
    CRUD_ASSERT(hdRepo.findById(testHd) == nullptr,
                "App Menu SUCCESS: HopDong deleted and removed from in-memory store");
    {
        Repository<HopDong> verifyHd(box.hdFile);
        verifyHd.loadFromFile();
        CRUD_ASSERT(verifyHd.findById(testHd) == nullptr,
                    "App Menu SUCCESS: HopDong permanently removed from disk file");
    }

    // B.3: Bay gio xoa ThueBao qua ThueBaoMenu (khong con IMEI rang buoc)
    // ThueBaoMenu::MENU_DELETE = 7
    driveMenu("7\n" + testPhone + "\ny\n\n0\n", [&]() { tbMenu.showMenu(); });
    CRUD_ASSERT(tbRepo.findById(testPhone) == nullptr,
                "App Menu SUCCESS: Subscriber deleted after IMEI cleared");
    {
        Repository<ThueBao> verifyTb(box.tbFile);
        verifyTb.loadFromFile();
        CRUD_ASSERT(verifyTb.findById(testPhone) == nullptr,
                    "App Menu SUCCESS: Subscriber permanently removed from disk file");
    }

    // B.4: Bay gio xoa KhachHang qua KhachHangMenu (khong con ThueBao / HopDong)
    // KhachHangMenu::MENU_DELETE = 5
    driveMenu("5\n" + testKh + "\ny\n0\n", [&]() { khMenu.showMenu(); });
    CRUD_ASSERT(khRepo.findById(testKh) == nullptr,
                "App Menu SUCCESS: Customer deleted after Subscriber and Contract cleared");
    {
        Repository<KhachHang> verifyKh(box.khFile);
        verifyKh.loadFromFile();
        CRUD_ASSERT(verifyKh.findById(testKh) == nullptr,
                    "App Menu SUCCESS: Customer permanently removed from disk file");
    }

    // B.5: Xoa GoiCuoc qua GoiCuocMenu
    // GoiCuocMenu::MENU_DELETE = 6 (confirm 'y', pause menu, back 0)
    driveMenu("6\n" + testGc + "\ny\n\n0\n", [&]() { gcMenu.showMenu(); });
    CRUD_ASSERT(gcRepo.findById(testGc) == nullptr,
                "App Menu SUCCESS: Package deleted via GoiCuocMenu");
    {
        Repository<GoiCuoc> verifyGc(box.gcFile);
        verifyGc.loadFromFile();
        CRUD_ASSERT(verifyGc.findById(testGc) == nullptr,
                    "App Menu SUCCESS: Package permanently removed from disk file");
    }
}

int main() {
    std::cout << "====================================================================\n";
    std::cout << "  KTLT CRUD DEEP TESTER: UPDATE / SWAP CREDENTIALS & APP DELETION   \n";
    std::cout << "====================================================================\n";

    testUpdateAndCredentialSwap();
    testDeleteViaAppMenus();

    std::cout << "\n====================================================================\n";
    std::cout << "Ket qua kiem thu CRUD: " << g_testPassed << " passed, " << g_testFailed << " failed.\n";
    std::cout << "====================================================================\n";

    if (g_testFailed > 0) {
        std::cerr << "[FAILED] Phat hien loi trong kiem thu Update/Delete!\n";
        return 1;
    }
    std::cout << "TAT CA KIEM THU U & D VA THAO TAC QUA APP MENU DEU HOAN HAO (100% PASS)!\n";
    return 0;
}
