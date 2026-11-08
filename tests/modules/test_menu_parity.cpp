#include "common.h"
#include "GoiCuocMenu.h"
#include "ThietBiIMEIMenu.h"
#include "KhachHangMenu.h"

namespace {
template <typename T, typename Menu>
void checkUpdate(Repository<T>& repo, Menu& menu, const std::string& path,
                 const T& seed, const std::string& input, const T& expected) {
    repo.update(seed.getId(), seed);
    {
        std::istringstream in(input);
        std::ostringstream out;
        StreamRedirectGuard streams;
        std::cin.clear();
        std::cin.rdbuf(in.rdbuf());
        std::cout.rdbuf(out.rdbuf());
        menu.showMenu();
    }
    Repository<T> disk(path);
    ASSERT_TRUE(disk.loadFromFile(), "Updated fixture must reload");
    const T* saved = disk.findById(seed.getId());
    ASSERT_TRUE(saved && saved->toFileString() == expected.toFileString(),
                "Persisted record must match all expected fields");
    const T* memory = repo.findById(seed.getId());
    ASSERT_TRUE(memory && memory->toFileString() == expected.toFileString(),
                "Memory and disk must agree");
}
}

void testPackageUpdateParity() {
    const std::string path = "data/test_package_parity.txt";
    FileCleanupGuard cleanup({path});
    Repository<GoiCuoc> repo(path);
    repo.clear();
    GoiCuoc seed("AUDIT", "Original", 50000, 5, 100, 30);
    repo.add(seed);
    GoiCuocMenu menu(repo);
    const std::string start = "5\nAUDIT\n";
    const std::string end = "\n0\n"; //menu goi cuoc dung cho Enter truoc khi quay lai

    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\nn\n" + end, seed);
    GoiCuoc expected = seed;
    expected.setTenGoi("Renamed");
    checkUpdate(repo, menu, path, seed, start + "Renamed\nn\nn\nn\nn\n" + end, expected);
    expected = seed;
    expected.setGiaCuoc(60000);
    checkUpdate(repo, menu, path, seed, start + "\ny\n60000\nn\nn\nn\n" + end, expected);
    expected = seed;
    expected.setDungLuongData(10);
    checkUpdate(repo, menu, path, seed, start + "\nn\ny\n10\nn\nn\n" + end, expected);
    expected = seed;
    expected.setSoPhutGoi(250);
    checkUpdate(repo, menu, path, seed, start + "\nn\nn\ny\n250\nn\n" + end, expected);
    expected = seed;
    expected.setChuKyNgay(60);
    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\ny\n60\n" + end, expected);

    expected = seed;
    expected.setGiaCuoc(0);
    expected.setDungLuongData(0);
    expected.setSoPhutGoi(0);
    expected.setChuKyNgay(1);
    checkUpdate(repo, menu, path, seed,
        start + "\ny\n-1\n0\ny\n-1\n0\ny\n-1\n0\ny\n0\n366\n1\n" + end, expected);
    expected = seed;
    expected.setChuKyNgay(365);
    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\ny\n365\n" + end, expected);
    checkUpdate(repo, menu, path, seed, start + "Pending\ny\n60000\ny\n", seed);
    checkUpdate(repo, menu, path, seed, "5\nMISSING\n" + end, seed);
}

void testDeviceMetadataUpdate() {
    const std::string path = "data/test_device_metadata.txt";
    const std::string phones = "data/test_device_metadata_phones.txt";
    FileCleanupGuard cleanup({path, phones});
    Repository<ThietBiIMEI> repo(path);
    Repository<ThueBao> subscribers(phones);
    repo.clear();
    subscribers.clear();
    ThietBiIMEI seed("860123456789014", "Original", "Apple", "0981234567",
                    Date(15, 1, 2024), "KhoaMang", "BTS-HN-001");
    repo.add(seed);
    ThietBiIMEIMenu menu(repo, subscribers);
    const std::string start = "6\n860123456789014\n5\n";
    const std::string end = "\n0\n";
    ThietBiIMEI expected = seed;
    expected.setTenThietBi("New Model");
    expected.setHangSanXuat("New Brand");
    checkUpdate(repo, menu, path, seed, start + "New Model\nNew Brand\n" + end, expected);
    checkUpdate(repo, menu, path, seed, start + "\n\n" + end, seed);
    expected = seed;
    expected.setTenThietBi(std::string(50, 'A'));
    checkUpdate(repo, menu, path, seed,
        start + std::string(51, 'A') + "\nBad|Name\n" + std::string(50, 'A') + "\n\n" + end, expected);
    checkUpdate(repo, menu, path, seed, start + "Pending\n", seed);
    checkUpdate(repo, menu, path, seed, "6\n860123456789014\n0\n" + end, seed);
    checkUpdate(repo, menu, path, seed, "6\nMISSING\n" + end, seed);
}

void testCustomerIssueMetadataUpdate() {
    const std::string path = "data/test_customer_metadata.txt";
    const std::string phones = "data/test_customer_metadata_phones.txt";
    const std::string contracts = "data/test_customer_metadata_contracts.txt";
    FileCleanupGuard cleanup({path, phones, contracts});
    Repository<KhachHang> repo(path);
    Repository<ThueBao> subscribers(phones);
    Repository<HopDong> agreements(contracts);
    repo.clear();
    subscribers.clear();
    agreements.clear();
    KhachHang seed("KH0001", "Nguyen Van An", "CCCD", "001201004567",
                  "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    repo.add(seed);
    KhachHangMenu menu(repo, subscribers, agreements);
    const std::string start = "4\nKH0001\n\n\n\n";
    KhachHang expected = seed;
    expected.setNgayCap("29/02/2024");
    expected.setNoiCap("Cuc C08");
    checkUpdate(repo, menu, path, seed, start + "29/02/2024\nCuc C08\n0\n", expected);
    expected = seed;
    expected.setNoiCap("Cuc C08");
    checkUpdate(repo, menu, path, seed, start + "\nBad|Authority\nCuc C08\n0\n", expected);
    checkUpdate(repo, menu, path, seed, start + "\n\n0\n", seed);
    checkUpdate(repo, menu, path, seed, "4\nKH0001\nPending\n\n\n0\n0\n", seed);
    checkUpdate(repo, menu, path, seed, "4\nKH0001\nPending\n\n\n29/02/2023\n0\n", seed);
    checkUpdate(repo, menu, path, seed, start + "01/01/2024junk\n0\n", seed);
    checkUpdate(repo, menu, path, seed, start + "15/05/2023\n", seed);
    checkUpdate(repo, menu, path, seed, "4\nMISSING\n0\n", seed);

    //chap nhan ngay truoc hoac sau neu kiem thu chay qua nua dem
    const std::string before = Date::now().toString();
    {
        std::istringstream in(start + "now\n\n0\n");
        std::ostringstream out;
        StreamRedirectGuard streams;
        std::cin.clear();
        std::cin.rdbuf(in.rdbuf());
        std::cout.rdbuf(out.rdbuf());
        repo.update(seed.getId(), seed);
        menu.showMenu();
    }
    const std::string after = Date::now().toString();
    Repository<KhachHang> disk(path);
    ASSERT_TRUE(disk.loadFromFile(), "Current issue date must reload");
    const KhachHang* saved = disk.findById(seed.getId());
    ASSERT_TRUE(saved && (saved->getNgayCap() == before || saved->getNgayCap() == after),
                "Now must use the current local date");
    if (saved) {
        expected = seed;
        expected.setNgayCap(saved->getNgayCap());
        ASSERT_TRUE(saved->toFileString() == expected.toFileString(),
                    "Now must preserve customer identity and other metadata");
    }
}
