#include "common.h"
#include "HopDongMenu.h"
#include "ThietBiIMEIMenu.h"
#include "GoiCuocMenu.h"
#include "ThueBaoMenu.h"
#include "KhachHangMenu.h"
#include "NapTienMenu.h"
#include "HoaDonMenu.h"
#include "InputHelper.h"
#include "Repository.h"
#include "Exceptions.h"

void testMenuControllers() {
    std::cout << "[RUN] Kiem thu Menu Controllers (HopDongMenu & ThietBiIMEIMenu)...\n";

    FileCleanupGuard cleanup({"data/test_hd_menu.txt", "data/test_imei_menu.txt", "data/test_kh_menu.txt", "data/test_gc_menu.txt", "data/test_tb_menu.txt"});
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

    StreamRedirectGuard streamGuard;

    //test thoat hopdongmenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("QUAN LY HOP DONG DANG KY") != std::string::npos, "HopDongMenu phai hien thi tieu de menu");
        ASSERT_TRUE(out.find("[0] Quay lai menu chinh") != std::string::npos, "HopDongMenu phai co option [0] Quay lai menu chinh");
    }

    //test thoat imeimenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("QUAN LY THIET BI DAU CUOI (IMEI)") != std::string::npos, "ThietBiIMEIMenu phai hien thi tieu de menu");
        ASSERT_TRUE(out.find("[0] Quay lai menu chinh") != std::string::npos, "ThietBiIMEIMenu phai co option [0] Quay lai menu chinh");
        ASSERT_TRUE(out.find("[4] Sap xep thiet bi theo Ma IMEI") < out.find("[5] Danh sach thiet bi bi khoa mang") &&
                        out.find("[7] Xoa thiet bi") < out.find("[0] Quay lai menu chinh"),
                    "ThietBiIMEIMenu phai hien thi cac tuy chon theo thu tu so");
    }

    //test submenu tim kiem
    {
        std::istringstream inputSim("3\n0\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("TIM KIEM HOP DONG") != std::string::npos, "HopDongMenu timKiemHopDong phai vao submenu");
    }

    //test huy cap nhat hd
    {
        std::istringstream inputSim("5\nHD0001\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "HopDongMenu cap nhat choice 0 phai thong bao huy bo");
    }

    //test huy cap nhat imei
    {
        std::istringstream inputSim("6\n860123456789014\n0\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[DA HUY] Thao tac cap nhat da duoc huy bo.") != std::string::npos, "ThietBiIMEIMenu cap nhat choice 0 phai thong bao huy bo");
    }

    //test eof inputhelper
    {
        std::istringstream emptyInput("");
        std::ostringstream outputCapture;
        std::cin.rdbuf(emptyInput.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        std::string phone = InputHelper::getPhoneNumber("Nhap sdt: ", false);
        ASSERT_TRUE(phone.empty(), "getPhoneNumber tren EOF phai return chuoi rong ma khong loop vo han");

        bool confirm = InputHelper::getConfirm("Xac nhan?");
        ASSERT_TRUE(!confirm, "getConfirm tren EOF phai return false ma khong loop vo han");
    }

    //test tu choi khoa ngoai
    {
        std::istringstream inputSim("1\nHD0002\nKH9999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("khong ton tai trong he thong") != std::string::npos, "HopDongMenu phai tu choi Ma KH khong ton tai");
        ASSERT_TRUE(hdRepo.findById("HD0002") == nullptr, "HopDongMenu khong duoc luu ban ghi khi khoa ngoai sai");
    }

    //test tu choi sdt
    {
        std::istringstream inputSim("1\n1\n861987654321096\nXiaomi 14\nXiaomi\n0989999999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("chua duoc dang ky tren he thong") != std::string::npos, "ThietBiIMEIMenu phai tu choi SDT chua dang ky");
        ASSERT_TRUE(imeiRepo.findById("861987654321096") == nullptr, "ThietBiIMEIMenu khong duoc luu ban ghi khi SDT chua dang ky");
    }

    //test chan gan trung lap SIM vao nhieu thiet bi IMEI (SIM collision)
    {
        std::istringstream inputSim("1\n1\n861987654321096\nXiaomi 14\nXiaomi\n0981234567\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("da duoc gan voi thiet bi IMEI 860123456789014") != std::string::npos,
                    "ThietBiIMEIMenu phai phat hien va chan trung lap SIM tren nhieu thiet bi");
        ASSERT_TRUE(imeiRepo.findById("861987654321096") == nullptr,
                    "ThietBiIMEIMenu khong duoc luu thiet bi khi bi trung lap SIM");
    }

    //test chan gan SIM vao thiet bi bi khoa mang (EIR Blacklist)
    {
        ThietBiIMEI blackImei("861987654321096", "Pixel 8", "Google", IMEIConstants::UNASSIGNED_PHONE,
                              Date(1, 1, 2024), IMEIConstants::STATUS_LOCKED, "BTS-HN-001");
        imeiRepo.add(blackImei);

        std::istringstream inputSim("6\n861987654321096\n1\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("EIR Blacklist") != std::string::npos,
                    "ThietBiIMEIMenu phai chan gan SIM vao thiet bi trong danh sach den EIR");
        ThietBiIMEI* checkDev = imeiRepo.findById("861987654321096");
        ASSERT_TRUE(checkDev != nullptr && checkDev->getSoDienThoai() == IMEIConstants::UNASSIGNED_PHONE,
                    "Thiet bi bi khoa mang phai khong the gan SIM hoat dong");
        imeiRepo.remove("861987654321096");
    }

    std::remove(HD_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
    std::remove(KH_TEST_FILE.c_str());
    std::remove(GC_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
}

void testNewRepositoriesAndMenus() {
    std::cout << "[RUN] Kiem thu Repository CRUD va Menu Controllers moi (GoiCuocMenu & ThueBaoMenu)...\n";

    const std::string GC_TEST_FILE = "data/test_gc_repo.txt";
    const std::string TB_TEST_FILE = "data/test_tb_repo.txt";
    const std::string KH_TEST_FILE = "data/test_kh_repo.txt";
    const std::string IMEI_TEST_FILE = "data/test_imei_repo.txt";
    const std::string HD_FK_TEST_FILE = "data/test_hd_fk.txt";
    const std::string INV_FK_TEST_FILE = "data/test_inv_fk.txt";

    Repository<GoiCuoc> gcRepo(GC_TEST_FILE);
    gcRepo.clear();
    GoiCuoc gc("M50", "Mobi 50", 50000.0, 5, 100, 30);
    gcRepo.add(gc);

    ASSERT_TRUE(gcRepo.size() == 1, "gcRepo size phai bang 1 sau khi them");
    ASSERT_TRUE(gcRepo.findById("M50") != nullptr, "findById M50 phai tim thay");

    Repository<KhachHang> khRepo(KH_TEST_FILE);
    khRepo.clear();
    KhachHang kh("KH0001", "Nguyen Van An", "CCCD", "001201004567", "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
    khRepo.add(kh);

    Repository<ThueBao> tbRepo(TB_TEST_FILE);
    tbRepo.clear();
    ThueBao tb("0981234567", "TraTruoc", "KH0001", "M50", "15/01/2024", "HoatDong", 100000.0);
    tbRepo.add(tb);

    Repository<ThietBiIMEI> imeiRepo(IMEI_TEST_FILE);
    imeiRepo.clear();
    imeiRepo.add(ThietBiIMEI(
        "359876543210983", "Assigned Phone", "Apple", "0981234567",
        Date(10, 3, 2024), IMEIConstants::STATUS_ACTIVE, "BTS-TEST-001"));

    Repository<HopDong> hdRepo(HD_FK_TEST_FILE);
    hdRepo.clear();

    Repository<HoaDon> hoaDonRepo(INV_FK_TEST_FILE);
    hoaDonRepo.clear();

    ASSERT_TRUE(tbRepo.size() == 1, "tbRepo size phai bang 1 sau khi them");
    ASSERT_TRUE(tbRepo.findById("0981234567") != nullptr, "findById 0981234567 phai tim thay");

    GoiCuocMenu gcMenu(gcRepo, &tbRepo, &hdRepo);
    ThueBaoMenu tbMenu(tbRepo, gcRepo, khRepo, imeiRepo, &hdRepo, &hoaDonRepo);

    StreamRedirectGuard streamGuard;

    //test thoat goicuocmenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        gcMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("QUAN LY GOI CUOC DI DONG") != std::string::npos, "GoiCuocMenu phai hien thi tieu de menu");
        ASSERT_TRUE(out.find("[0] Quay lai menu chinh") != std::string::npos, "GoiCuocMenu phai co option [0] Quay lai menu chinh");
    }

    //test thoat thuebaomenu
    {
        std::istringstream inputSim("0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("QUAN LY THUE BAO DI DONG") != std::string::npos, "ThueBaoMenu phai hien thi tieu de menu");
        ASSERT_TRUE(out.find("[0] Quay lai menu chinh") != std::string::npos, "ThueBaoMenu phai co option [0] Quay lai menu chinh");
        ASSERT_TRUE(out.find("[7] Xoa thue bao") < out.find("[8] Sap xep thue bao theo So dien thoai"),
                    "ThueBaoMenu phai hien thi cac tuy chon theo thu tu so");
    }

    //test tu choi xoa thue bao dang gan IMEI
    {
        std::istringstream inputSim("7\n0981234567\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        ASSERT_TRUE(tbRepo.findById("0981234567") != nullptr,
                    "Khong duoc xoa thue bao dang duoc gan IMEI");
        ASSERT_TRUE(imeiRepo.findById("359876543210983") != nullptr &&
                        imeiRepo.findById("359876543210983")->getSoDienThoai() == "0981234567",
                    "IMEI phai giu nguyen lien ket khi tu choi xoa thue bao");
        ASSERT_TRUE(outputCapture.str().find("Khong the xoa thue bao") != std::string::npos,
                    "Menu phai bao loi khi thue bao con IMEI dang gan");
    }

    //test tu choi ma kh
    {
        std::istringstream inputSim("1\n0989999999\n1\nKH9999\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("khong ton tai trong he thong") != std::string::npos, "ThueBaoMenu phai tu choi Ma KH khong ton tai");
        ASSERT_TRUE(tbRepo.findById("0989999999") == nullptr, "ThueBaoMenu khong duoc luu thue bao khi Ma KH sai");
    }

    //test them thue bao voi ngay kich hoat = now
    {
        std::istringstream inputSim("1\n0981112233\n1\nKH0001\n\nnow\n50000\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        ThueBao* createdTb = tbRepo.findById("0981112233");
        ASSERT_TRUE(createdTb != nullptr, "ThueBaoMenu phai tao duoc thue bao voi ngay 'now'");
        if (createdTb) {
            ASSERT_TRUE(createdTb->getNgayKichHoat() == Date::now().toString(),
                        "Ngay kich hoat bang 'now' phai khop voi Date::now().toString()");
        }
    }

    //test huy them thue bao bang so 0 tai prompt ngay
    {
        std::istringstream inputSim("1\n0981112244\n1\nKH0001\n\n0\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[DA HUY] Thao tac them thue bao da duoc huy bo.") != std::string::npos,
                    "Nhap '0' tai prompt ngay phai thong bao [DA HUY]");
        ASSERT_TRUE(tbRepo.findById("0981112244") == nullptr,
                    "Nhap '0' tai prompt ngay khong duoc tao ban ghi thue bao");
    }

    // Test GoiCuocMenu: tu choi xoa goi cuoc khi co thue bao dang su dung
    {
        std::istringstream inputSim("6\nM50\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        gcMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("Khong the xoa goi cuoc! Hien dang co") != std::string::npos,
                    "GoiCuocMenu phai tu choi xoa goi cuoc dang co thue bao su dung");
        ASSERT_TRUE(gcRepo.findById("M50") != nullptr,
                    "Goi cuoc M50 khong duoc bi xoa khi co thue bao su dung");
    }

    // Test GoiCuocMenu: tu choi xoa goi cuoc khi co hop dong dang su dung
    {
        GoiCuoc gc2("M100", "Mobi 100", 100000.0, 10, 200, 30);
        gcRepo.add(gc2);
        HopDong hdTest("HD_GC", "KH0001", "0981112233", "M100", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", "HieuLuc", 100000.0);
        hdRepo.add(hdTest);

        std::istringstream inputSim("6\nM100\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        gcMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("Khong the xoa goi cuoc! Hien dang co") != std::string::npos &&
                    out.find("hop dong") != std::string::npos,
                    "GoiCuocMenu phai tu choi xoa goi cuoc dang co hop dong su dung");
        ASSERT_TRUE(gcRepo.findById("M100") != nullptr,
                    "Goi cuoc M100 khong duoc bi xoa khi co hop dong su dung");

        hdRepo.remove("HD_GC");
        gcRepo.remove("M100");
    }

    // Test ThueBaoMenu: tu choi xoa thue bao khi co hop dong dang hieu luc
    {
        HopDong hdActive("HD_TB", "KH0001", "0981112233", "M50", Date(1, 1, 2024), Date(1, 1, 2025), "TraSau", HopDongConstants::STATUS_ACTIVE, 100000.0);
        hdRepo.add(hdActive);

        std::istringstream inputSim("7\n0981112233\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("Khong the xoa thue bao! Hien dang co") != std::string::npos &&
                    out.find("hop dong") != std::string::npos,
                    "ThueBaoMenu phai tu choi xoa thue bao co hop dong con hieu luc");
        ASSERT_TRUE(tbRepo.findById("0981112233") != nullptr,
                    "Thue bao co hop dong hieu luc khong duoc bi xoa");

        hdRepo.remove("HD_TB");
    }

    // Test ThueBaoMenu: tu choi xoa thue bao khi con hoa don chua thanh toan
    {
        HoaDon hdUnpaid("INV_TB", "0981112233", "02/2024", 50000.0, 50000.0, 50000.0, HoaDonConstants::STATUS_UNPAID);
        hoaDonRepo.add(hdUnpaid);

        std::istringstream inputSim("7\n0981112233\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        tbMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("Khong the xoa thue bao! Thue bao con") != std::string::npos &&
                    out.find("hoa don cuoc chua thanh toan") != std::string::npos,
                    "ThueBaoMenu phai tu choi xoa thue bao con hoa don no chua thanh toan");
        ASSERT_TRUE(tbRepo.findById("0981112233") != nullptr,
                    "Thue bao con hoa don chua thanh toan khong duoc bi xoa");

        hoaDonRepo.remove("INV_TB");
    }

    // Test ThueBaoMenu: dang ky goi cuoc va tru tien thue bao tra truoc
    {
        gcRepo.add(GoiCuoc("M200", "Mobi 200", 200000.0, 30, 500, 30));
        gcRepo.add(GoiCuoc("M70", "Mobi 70", 70000.0, 7, 150, 30));

        // 1. So du khong du (100k < 200k) -> bi tu choi, giu nguyen so du va goi cuoc cu
        {
            std::istringstream inputSim("4\n0981234567\nM200\n0\n");
            std::ostringstream outputCapture;
            std::cin.rdbuf(inputSim.rdbuf());
            std::cout.rdbuf(outputCapture.rdbuf());

            tbMenu.showMenu();

            std::string out = outputCapture.str();
            ASSERT_TRUE(out.find("khong du de dang ky goi cuoc") != std::string::npos,
                        "ThueBaoMenu phai tu choi dang ky goi cuoc khi so du khong du");
            ThueBao* checkTb = tbRepo.findById("0981234567");
            ASSERT_TRUE(checkTb != nullptr && checkTb->getSoDuTaiKhoan() == 100000.0,
                        "So du thue bao khong duoc bi tru khi dang ky that bai");
            ASSERT_TRUE(checkTb != nullptr && checkTb->getMaGoiCuoc() == "M50",
                        "Goi cuoc thue bao phai giu nguyen goi cu khi dang ky that bai");
        }

        // 2. So du du (100k >= 70k) -> dang ky thanh cong, tru tien (100k -> 30k), cap nhat ma goi
        {
            std::istringstream inputSim("4\n0981234567\nM70\n0\n");
            std::ostringstream outputCapture;
            std::cin.rdbuf(inputSim.rdbuf());
            std::cout.rdbuf(outputCapture.rdbuf());

            tbMenu.showMenu();

            std::string out = outputCapture.str();
            ASSERT_TRUE(out.find("thanh cong") != std::string::npos,
                        "ThueBaoMenu phai dang ky thanh cong khi so du hop le");
            ThueBao* checkTb = tbRepo.findById("0981234567");
            ASSERT_TRUE(checkTb != nullptr && checkTb->getSoDuTaiKhoan() == 30000.0,
                        "So du thue bao phai bi tru 70000 (100k -> 30k)");
            ASSERT_TRUE(checkTb != nullptr && checkTb->getMaGoiCuoc() == "M70",
                        "Goi cuoc thue bao phai duoc cap nhat sang M70");
        }

        gcRepo.remove("M200");
        gcRepo.remove("M70");
    }

    std::remove(GC_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(KH_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
    std::remove(HD_FK_TEST_FILE.c_str());
    std::remove(INV_FK_TEST_FILE.c_str());
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
    HoaDonMenu hdMenu(hoaDonRepo, tbRepo, knRepo, cdrRepo);

    ThueBao tbSau("0981234568", "TraSau", "KH0001", "GC001", "15/01/2024", "HoatDong", 0.0);
    tbRepo.add(tbSau);
    hoaDonRepo.add(HoaDon("HDNO", "0981234568", "01/2024", 0.0, 100.0, 100.0, HoaDonConstants::STATUS_UNPAID));
    ASSERT_TRUE(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 1, "No cuoc qua han phai khoa mot thue bao");
    ASSERT_TRUE(tbRepo.findById("0981234568")->getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT, "Thue bao tra sau no qua han phai bi khoa no");
    ASSERT_TRUE(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 1, "Kiem tra no lap lai van nhan dien mot khoan no");
    HoaDon noDaTra = *hoaDonRepo.findById("HDNO");
    noDaTra.thanhToan();
    hoaDonRepo.update("HDNO", noDaTra);
    ASSERT_TRUE(hdMenu.kiemTraNo(Date(1, 2, 2024)) == 0, "Thanh toan het no phai xoa danh sach no qua han");
    ASSERT_TRUE(tbRepo.findById("0981234568")->getTrangThai() == ThueBaoConstants::STATUS_ACTIVE, "Thanh toan het no phai mo khoa tu dong");

    StreamRedirectGuard streamGuard;

    // 0. Test ThietBiIMEIMenu: sinh IMEI mo phong
    {
        std::istringstream inputSim("1\n2\nDemo Phone\nDemo Brand\n\n2\n01/01/2024\nBTS-DEMO-001\n\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        imeiMenu.showMenu();

        ASSERT_TRUE(imeiRepo.size() == 1, "ThietBiIMEIMenu phai them thiet bi voi IMEI mo phong");
        if (!imeiRepo.empty()) {
            const std::string generated = imeiRepo.getAll().front().getMaIMEI();
            ASSERT_TRUE(generated.length() == IMEIConstants::REQUIRED_IMEI_LENGTH,
                        "IMEI mo phong phai co dung 15 chu so");
            ASSERT_TRUE(generated.find_first_not_of("0123456789") == std::string::npos,
                        "IMEI mo phong phai chi gom chu so ASCII");
            ASSERT_TRUE(ThietBiIMEI::validateLuhn(generated),
                        "IMEI mo phong phai hop le theo Luhn");
            ASSERT_TRUE(outputCapture.str().find("MO PHONG") != std::string::npos,
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

            ASSERT_TRUE(khRepo.findById("KH0002") != nullptr, "KhachHangMenu phai them duoc KH0002");
        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[3] Tra cuu khach hang") < out.find("[4] Cap nhat thong tin khach hang") &&
                        out.find("[7] Sap xep khach hang theo Ma KH") < out.find("[0] Quay lai menu chinh"),
                    "KhachHangMenu phai hien thi cac tuy chon theo thu tu so");
    }

    // 1a. Test KhachHangMenu: tim kiem ma KH khong phan biet hoa thuong
    {
        std::istringstream inputSim("3\n1\n kh0001 \n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        khMenu.showMenu();

        ASSERT_TRUE(outputCapture.str().find("KH0001") != std::string::npos &&
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

        ASSERT_TRUE(khRepo.findById("KH0000005") == nullptr, "KhachHangMenu khong duoc luu CCCD sai");
        ASSERT_TRUE(outputCapture.str().find("CCCD khong hop le") != std::string::npos,
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
        ASSERT_TRUE(out.find("Khong the xoa khach hang") != std::string::npos, "KhachHangMenu phai tu choi xoa KH co thue bao lien ket");
        ASSERT_TRUE(khRepo.findById("KH0001") != nullptr, "KH0001 khong duoc bi xoa");
    }

    // 3. Test NapTienMenu: lap phieu nap tien va cong tien vao tai khoan
    {
        std::istringstream inputSim("1\nGD0001\n0981234567\n50000\n1\n15/01/2024\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        ntMenu.showMenu();

        ASSERT_TRUE(ntRepo.findById("GD0001") != nullptr, "NapTienMenu phai luu duoc phieu nap tien GD0001");
        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[3] Tra cuu lich su nap tien") < out.find("[4] Ghi nhan ban ghi cuoc goi") &&
                        out.find("[6] Sap xep phieu nap / CDR") < out.find("[0] Quay lai menu chinh"),
                    "NapTienMenu phai hien thi cac tuy chon theo thu tu so");
        ThueBao* updatedTb = tbRepo.findById("0981234567");
        ASSERT_TRUE(updatedTb != nullptr && updatedTb->getSoDuTaiKhoan() == 150000.0, "So du thue bao phai duoc cong them 50000 (100k -> 150k)");
    }

    // 4. Test HoaDonMenu: lap hoa don va thanh toan
    {
        std::istringstream inputSim("1\nHD01\n0981234567\n01/2024\n50000\n40000\n3\nHD01\ny\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("[4] Tra cuu hoa don theo so dien thoai") < out.find("[5] Tiep nhan phieu") &&
                        out.find("[8] Sap xep hoa don / khieu nai") < out.find("[0] Quay lai menu chinh"),
                    "HoaDonMenu phai hien thi cac tuy chon theo thu tu so");
        HoaDon* inv = hoaDonRepo.findById("HD01");
        ASSERT_TRUE(inv != nullptr, "HoaDonMenu phai lap duoc hoa don HD01");
        if (inv) {
            ASSERT_TRUE(inv->getTrangThai() == HoaDonConstants::STATUS_PAID, "Hoa don phai duoc cap nhat thanh DaThanhToan sau khi xac nhan");
        }
    }

    // 5. Test HoaDonMenu: mo khoa no cuoc theo pham vi tung thue bao (subscriber-scoped debt unblocking)
    {
        // Tao 2 thue bao tra sau deu bi khoa no
        ThueBao tbNo1("0983334455", "TraSau", "KH0001", "GC001", "01/01/2024", ThueBaoConstants::STATUS_LOCKED_DEBT, 0.0);
        ThueBao tbNo2("0985556677", "TraSau", "KH0001", "GC001", "01/01/2024", ThueBaoConstants::STATUS_LOCKED_DEBT, 0.0);
        tbRepo.add(tbNo1);
        tbRepo.add(tbNo2);

        // Tao 2 hoa don no qua han cho 2 thue bao
        HoaDon hdDebt1("HD_DEBT1", "0983334455", "01/2024", 0.0, 150000.0, 150000.0, HoaDonConstants::STATUS_UNPAID);
        HoaDon hdDebt2("HD_DEBT2", "0985556677", "01/2024", 0.0, 200000.0, 200000.0, HoaDonConstants::STATUS_UNPAID);
        hoaDonRepo.add(hdDebt1);
        hoaDonRepo.add(hdDebt2);

        // Thanh toan hoa don HD_DEBT1 cua thue bao 1 qua menu
        std::istringstream inputSim("3\nHD_DEBT1\ny\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        hdMenu.showMenu();

        // Thue bao 1 phai duoc mo khoa HoatDong vi da tra het no cua chinh minh
        ThueBao* checkTb1 = tbRepo.findById("0983334455");
        ASSERT_TRUE(checkTb1 != nullptr && checkTb1->getTrangThai() == ThueBaoConstants::STATUS_ACTIVE,
                    "Thue bao 1 phai duoc tu dong mo khoa HoatDong khi thanh toan het no cua minh");

        // Thue bao 2 van phai bi KhoaNo vi con hoa don no chua thanh toan
        ThueBao* checkTb2 = tbRepo.findById("0985556677");
        ASSERT_TRUE(checkTb2 != nullptr && checkTb2->getTrangThai() == ThueBaoConstants::STATUS_LOCKED_DEBT,
                    "Thue bao 2 van phai bi KhoaNo khi chua thanh toan hoa don cua minh");

        tbRepo.remove("0983334455");
        tbRepo.remove("0985556677");
        hoaDonRepo.remove("HD_DEBT1");
        hoaDonRepo.remove("HD_DEBT2");
    }

    // 6. Test NapTienMenu: tu choi lap phieu nap tien cho so thue bao chua dang ky
    {
        std::istringstream inputSim("1\nGD9999\n0989999999\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        ntMenu.showMenu();

        std::string out = outputCapture.str();
        ASSERT_TRUE(out.find("chua duoc dang ky tren he thong") != std::string::npos,
                    "NapTienMenu phai tu choi lap phieu cho so dien thoai chua ton tai");
        ASSERT_TRUE(ntRepo.findById("GD9999") == nullptr,
                    "Khong duoc luu phieu nap GD9999 khi so dien thoai khong ton tai");
    }

    // 7. Test NapTienMenu: ghi nhan ban ghi CDR va tu dong tru tien thue bao tra truoc
    {
        std::istringstream inputSim("4\nCDR_TEST1\n0981234567\n1\n0989999888\n01/01/2024 10:00:00\n120\n20000\n0\n");
        std::ostringstream outputCapture;
        std::cin.rdbuf(inputSim.rdbuf());
        std::cout.rdbuf(outputCapture.rdbuf());

        ntMenu.showMenu();

        ASSERT_TRUE(cdrRepo.findById("CDR_TEST1") != nullptr,
                    "NapTienMenu phai ghi nhan va luu ban ghi CDR_TEST1 vao he thong");
        ThueBao* afterCdrTb = tbRepo.findById("0981234567");
        ASSERT_TRUE(afterCdrTb != nullptr && afterCdrTb->getSoDuTaiKhoan() == 130000.0,
                    "Thue bao tra truoc phai duoc tru cuoc phi tu dong sau khi ghi nhan CDR (150k -> 130k)");
    }

    std::remove(KH_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(HD_TEST_FILE.c_str());
    std::remove(NT_TEST_FILE.c_str());
    std::remove(GHICUOCGOI_TEST_FILE.c_str());
    std::remove(INV_TEST_FILE.c_str());
    std::remove(KN_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
}

