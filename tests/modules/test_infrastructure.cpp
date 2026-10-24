#include "common.h"
#include "Repository.h"
#include "fileio.h"
#include "BTSRegister.h"
#include "hopdong.h"
#include "imei.h"
#include "Exceptions.h"

void testRepository() {
    std::cout << "[RUN] Kiem thu Repository<T> CRUD va atomic persistence...\n";

    FileCleanupGuard cleanup({"data/test_repo.txt", "data/test_invalid_imei.txt", "data/test_invalid_hopdong.txt", "data/test_malformed_row.txt", "data/test_missing_file.txt", "data/test_load_preserve.txt"});
    const std::string TEST_FILE = "data/test_repo.txt";
    Repository<HopDong> repo(TEST_FILE);
    repo.clear();

    HopDong hd1("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    HopDong hd2("HD0002", "KH0002", "0978999888", "GC002", Date(1, 2, 2024), Date(1, 2, 2025), "TraTruoc", "HieuLuc", 90000.0);

    ASSERT_TRUE(repo.add(hd1), "Them hd1 phai thanh cong");
    ASSERT_TRUE(repo.add(hd2), "Them hd2 phai thanh cong");
    ASSERT_TRUE(repo.size() == 2, "Repository phai co 2 phan tu");

    bool threwDuplicate = false;
    try {
        repo.add(hd1);
    } catch (const DuplicateIdException&) {
        threwDuplicate = true;
    }
    ASSERT_TRUE(threwDuplicate, "Them trung id phai nem DuplicateIdException");

    HopDong* found = repo.findById("HD0001");
    ASSERT_TRUE(found != nullptr, "Phai tim thay HD0001");
    ASSERT_TRUE(found->getMaKhachHang() == "KH0001", "Khach hang phai la KH0001");

    HopDong notFound = HopDong("HD9999", "KH9999", "0989999999", "GC999", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 100.0);
    bool threwNotFound = false;
    try {
        repo.update("HD9999", notFound);
    } catch (const NotFoundException&) {
        threwNotFound = true;
    }
    ASSERT_TRUE(threwNotFound, "Update ma khong ton tai phai nem NotFoundException");

    repo.remove("HD0001");
    ASSERT_TRUE(repo.size() == 1, "Sau khi xoa size phai la 1");
    ASSERT_TRUE(repo.findById("HD0001") == nullptr, "HD0001 khong con ton tai");

    Repository<HopDong> reloadedRepo(TEST_FILE);
    reloadedRepo.loadFromFile();
    ASSERT_TRUE(reloadedRepo.size() == 1, "Reloaded store phai co dung 1 ban ghi da luu");
    ASSERT_TRUE(reloadedRepo.findById("HD0002") != nullptr, "Reloaded store phai chua HD0002");

    const std::string SORT_FILE = "data/test_repo_sort.txt";
    Repository<HopDong> sortRepo(SORT_FILE);
    sortRepo.clear();
    HopDong sortHd1("HD0002", "KH0002", "0978999888", "GC002", Date(1, 2, 2024), Date(1, 2, 2025), "TraTruoc", "HieuLuc", 90000.0);
    HopDong sortHd2("HD0001", "KH0001", "0981234567", "GC001", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 150000.0);
    sortRepo.add(sortHd1);
    sortRepo.add(sortHd2);
    ASSERT_TRUE(sortRepo.sortById(true), "Sort ID tang dan phai luu thanh cong");
    ASSERT_TRUE(sortRepo.getAll()[0].getId() == "HD0001" && sortRepo.getAll()[1].getId() == "HD0002",
                "Sort ID tang dan phai sap xep trong bo nho");
    Repository<HopDong> ascendingSortRepo(SORT_FILE);
    ascendingSortRepo.loadFromFile();
    ASSERT_TRUE(ascendingSortRepo.getAll()[0].getId() == "HD0001",
                "Sort ID tang dan phai duoc luu tren tep");
    ASSERT_TRUE(sortRepo.sortById(false), "Sort ID giam dan phai luu thanh cong");
    ASSERT_TRUE(sortRepo.getAll()[0].getId() == "HD0002" && sortRepo.getAll()[1].getId() == "HD0001",
                "Sort ID giam dan phai sap xep trong bo nho");
    Repository<HopDong> descendingSortRepo(SORT_FILE);
    ASSERT_TRUE(descendingSortRepo.loadFromFile() && descendingSortRepo.getAll()[0].getId() == "HD0002",
                "Sort ID giam dan phai duoc luu tren tep");

    Repository<HopDong> failedSortRepo("/proc/ktlt_repository_sort.txt");
    failedSortRepo.getAll().push_back(sortHd1);
    bool sortThrew = false;
    try {
        failedSortRepo.sortAndSave([](const HopDong& first, const HopDong& second) {
            return first.getId() > second.getId();
        });
    } catch (const FileIOException&) {
        sortThrew = true;
    }
    ASSERT_TRUE(sortThrew, "Sort luu that bai phai nem FileIOException");
    ASSERT_TRUE(failedSortRepo.getAll()[0].getId() == "HD0002",
                "Sort luu that bai phai rollback thu tu trong bo nho");

    const std::string INVALID_IMEI_FILE = "data/test_invalid_imei.txt";
    {
        std::ofstream out(INVALID_IMEI_FILE);
        out << "not-an-imei|Phone|Brand|0241234567|15/01/2024|HoatDong|BTS-001\\n";
    }
    Repository<ThietBiIMEI> invalidImeiStore(INVALID_IMEI_FILE);
    ASSERT_TRUE(!invalidImeiStore.loadFromFile(), "File IMEI co dong sai phai bao that bai");
    ASSERT_TRUE(invalidImeiStore.empty(), "Repository khong duoc cong bo dong IMEI sai");

    const std::string INVALID_HOPDONG_FILE = "data/test_invalid_hopdong.txt";
    {
        std::ofstream out(INVALID_HOPDONG_FILE);
        out << "HD-BAD|KH-1|0241234567|GC-1|01/01/2024|01/01/2025|TraSau|HieuLuc|1000\\n";
    }
    Repository<HopDong> invalidHopDongStore(INVALID_HOPDONG_FILE);
    ASSERT_TRUE(!invalidHopDongStore.loadFromFile(), "File hop dong co dong sai phai bao that bai");
    ASSERT_TRUE(invalidHopDongStore.empty(), "Repository khong duoc cong bo dong hop dong sai");

    const std::string MALFORMED_ROW_FILE = "data/test_malformed_row.txt";
    {
        std::ofstream out(MALFORMED_ROW_FILE);
        out << hd2.toFileString() << "\\n";
        out << "MALFORMED|ROW\\n";
    }
    Repository<HopDong> malformedRowStore(MALFORMED_ROW_FILE);
    ASSERT_TRUE(!malformedRowStore.loadFromFile(), "Dong malformed phai lam load that bai");
    ASSERT_TRUE(malformedRowStore.empty(), "Load that bai phai khong cong bo dong hop le truoc do");
    std::ifstream malformedRowInput(MALFORMED_ROW_FILE);
    std::string malformedRowContents(
        (std::istreambuf_iterator<char>(malformedRowInput)),
        std::istreambuf_iterator<char>());
    ASSERT_TRUE(malformedRowContents.find("MALFORMED|ROW") != std::string::npos,
                "Load that bai phai giu nguyen tep tren dia");

    const std::string MISSING_FILE = "data/test_missing_file.txt";
    std::remove(MISSING_FILE.c_str());
    Repository<HopDong> missingStore(MISSING_FILE);
    ASSERT_TRUE(!missingStore.loadFromFile(), "File khong ton tai phai bao that bai khi nap");

    const std::string LOAD_PRESERVE_FILE = "data/test_load_preserve.txt";
    Repository<HopDong> loadPreserveStore(LOAD_PRESERVE_FILE);
    loadPreserveStore.clear();
    loadPreserveStore.add(hd1);
    std::remove(LOAD_PRESERVE_FILE.c_str());
    ASSERT_TRUE(!loadPreserveStore.loadFromFile(), "Nap file da bi xoa phai bao that bai");
    ASSERT_TRUE(loadPreserveStore.size() == 1 && loadPreserveStore.findById("HD0001") != nullptr,
                "Nap file that bai phai giu nguyen du lieu dang co trong bo nho");

    Repository<HopDong> rollbackStore("/proc/ktlt_repository_rollback.txt");
    bool saveFailed = false;
    try {
        rollbackStore.add(hd1);
    } catch (const FileIOException& e) {
        saveFailed = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(saveFailed, "Duong dan khong ghi duoc phai nem FileIOException");
    ASSERT_TRUE(rollbackStore.empty(), "Them that bai phai rollback ban ghi trong bo nho");

    Repository<HopDong> updateRollbackStore("/proc/ktlt_repository_update.txt");
    updateRollbackStore.getAll().push_back(hd1);
    HopDong changed("HD0001", "KH0002", "0978999888", "GC002", Date(1, 1, 2024), Date(1, 1, 2025), "TraTruoc", "HieuLuc", 90000.0);
    bool updateThrew = false;
    try {
        updateRollbackStore.update("HD0001", changed);
    } catch (const FileIOException& e) {
        updateThrew = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(updateThrew, "Cap nhat tren file loi phai nem FileIOException");
    ASSERT_TRUE(updateRollbackStore.findById("HD0001")->getMaKhachHang() == "KH0001",
                "Cap nhat that bai phai khoi phuc ban ghi cu");

    Repository<HopDong> removeRollbackStore("/proc/ktlt_repository_remove.txt");
    removeRollbackStore.getAll().push_back(hd1);
    bool removeThrew = false;
    try {
        removeRollbackStore.remove("HD0001");
    } catch (const FileIOException& e) {
        removeThrew = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(removeThrew, "Xoa tren file loi phai nem FileIOException");
    ASSERT_TRUE(removeRollbackStore.size() == 1 && removeRollbackStore.findById("HD0001") != nullptr,
                "Xoa that bai phai chen lai ban ghi dung vi tri");

    Repository<HopDong> clearRollbackStore("/proc/ktlt_repository_clear.txt");
    clearRollbackStore.getAll().push_back(hd1);
    clearRollbackStore.getAll().push_back(hd2);
    bool clearThrew = false;
    try {
        clearRollbackStore.clear();
    } catch (const FileIOException& e) {
        clearThrew = (!std::string(e.what()).empty());
    }
    ASSERT_TRUE(clearThrew, "Clear tren file loi phai nem FileIOException");
    ASSERT_TRUE(clearRollbackStore.size() == 2 && clearRollbackStore.findById("HD0001") != nullptr && clearRollbackStore.findById("HD0002") != nullptr,
                "Clear that bai phai khoi phuc toan bo danh sach");

    std::remove(TEST_FILE.c_str());
    std::remove(INVALID_IMEI_FILE.c_str());
    std::remove(INVALID_HOPDONG_FILE.c_str());
    std::remove(MALFORMED_ROW_FILE.c_str());
}

void testFileIOTmpfs() {
    std::cout << "[RUN] Kiem thu FileIO staging qua tmps/ va tmpfs zero-SSD...\n";

    ASSERT_TRUE(FileIO::joinPath("data", "khachhang.txt") == "data/khachhang.txt",
                "joinPath phai tao duong dan data on dinh");
    ASSERT_TRUE(FileIO::resolveDataDirectory("/tmp/quanlythuebao") == "data",
                "resolveDataDirectory phai tim thay bo data tu thu muc hien tai");
    ASSERT_TRUE(FileIO::initTmpDir(), "initTmpDir phai return true");
    ASSERT_TRUE(FileIO::ensureDirectoryExists("tmps/test_dir/file.txt"),
                "ensureDirectoryExists voi POSIX_DIR_PERMISSIONS phai thanh cong");

    std::string testTarget = "data/test_tmpfs_target.txt";
    std::string stagingPath = FileIO::getTempStagingPath(testTarget);

    ASSERT_TRUE(stagingPath.find("tmps/") != std::string::npos, "Staging path phai nam trong thu muc tmps/");

    //ghi file staging tmps
    {
        std::ofstream out(stagingPath);
        ASSERT_TRUE(out.is_open(), "Mo file staging tren tmps/ phai thanh cong");
        out << "SAMPLE_DATA_LINE_1|VAL1\nSAMPLE_DATA_LINE_2|VAL2\n";
        out.close();
    }

    //commit tu tmps
    bool committed = FileIO::commitFromTmp(stagingPath, testTarget);
    ASSERT_TRUE(committed, "commitFromTmp phai thanh cong");

    //kiem tra noi dung file
    {
        std::ifstream in(testTarget);
        ASSERT_TRUE(in.is_open(), "Target file tren SSD phai ton tai sau khi commit tu tmps/");
        std::string line1, line2;
        std::getline(in, line1);
        std::getline(in, line2);
        ASSERT_TRUE(line1 == "SAMPLE_DATA_LINE_1|VAL1", "Noi dung dong 1 phai khop");
        ASSERT_TRUE(line2 == "SAMPLE_DATA_LINE_2|VAL2", "Noi dung dong 2 phai khop");
        in.close();
    }

    //kiem tra don dep tmps
    {
        std::ifstream tmpCheck(stagingPath);
        ASSERT_TRUE(!tmpCheck.is_open(), "File tam tren tmps/ phai duoc xoa sau khi commit vao SSD");
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
    ASSERT_TRUE(!replaceDirectory, "Commit vao dich la thu muc phai that bai");
    ASSERT_TRUE(stagedAfterFailure.is_open(), "Commit that bai phai giu lai file staging");
    stagedAfterFailure.close();
    std::remove(preservedTmp.c_str());
#if defined(_WIN32)
    _rmdir(preservedTarget.c_str());
#else
    rmdir(preservedTarget.c_str());
#endif
}

void testBTSRegistry() {
    std::cout << "[RUN] Kiem thu BTSRegistry va danh ba tram phat song...\n";

    BTSRegistry bts;
    ASSERT_TRUE(bts.size() > 0, "BTSRegistry phai nap duoc it nhat 1 tram tu bts_vietnam.txt");

    const BTSRecord* r = bts.findById("BTS-BD-001");
    ASSERT_TRUE(r != nullptr, "findById BTS-BD-001 phai tim thay");
    if (r) {
        ASSERT_TRUE(r->carrier.find("VinaPhone") != std::string::npos, "Carrier BTS-BD-001 phai la VinaPhone");
    }

    ASSERT_TRUE(bts.findById("NON_EXISTENT_BTS") == nullptr, "findById khong tim thay phai return nullptr");

    std::string nearest = bts.suggestNearestBTS("Thu Dau Mot");
    ASSERT_TRUE(!nearest.empty() && bts.findById(nearest) != nullptr, "suggestNearestBTS phai goi y ma tram ton tai hop le trong danh ba");

    // VAL-02: hang toa do xau phai bi bo qua, khong luu (0,0) gia.
    {
        FileCleanupGuard cleanup({"data/test_bts_badcoord.txt"});
        const std::string BTS_BAD_COORD = "data/test_bts_badcoord.txt";
        {
            std::ofstream f(BTS_BAD_COORD);
            //dong hop le
            f << "BTS-T01|452|02|1001|2001|HN|Ba Dinh|Truc Bach|Dia chi test|21.034000|105.848000|LTE|Viettel|30|0\n";
            //dong toa do xau: phai bi bo qua, khong them vao danh sach
            f << "BTS-T02|452|02|1002|2002|HN|Hoan Kiem|Hoan Kiem|Dia chi test|BADLAT|BADLON|LTE|Viettel|30|0\n";
        }
        BTSRegistry bts2;
        bts2.LoadFromFile(BTS_BAD_COORD);
        ASSERT_TRUE(bts2.size() == 1, "Hang toa do xau phai bi bo qua, chi giu hang hop le");
        ASSERT_TRUE(bts2.findById("BTS-T01") != nullptr, "Hang hop le phai duoc nap");
        ASSERT_TRUE(bts2.findById("BTS-T02") == nullptr, "Hang toa do xau khong duoc them vao danh sach");
        std::remove(BTS_BAD_COORD.c_str());
    }

    // VAL-02: LoadFromFile that bai khong xoa du lieu cu trong towers.
    {
        BTSRegistry bts3;
        bts3.LoadFromFile("data/bts_vietnam.txt");
        size_t sizeBefore = bts3.size();
        ASSERT_TRUE(sizeBefore > 0, "Phai co du lieu truoc khi thu load tep xau");
        bool ok = bts3.LoadFromFile("data/KHONG_TON_TAI_____bts.txt");
        ASSERT_TRUE(!ok, "LoadFromFile tep khong ton tai phai return false");
        ASSERT_TRUE(bts3.size() == sizeBefore, "Towers cu phai duoc giu nguyen khi load that bai");
    }
}

