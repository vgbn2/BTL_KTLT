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

    ASSERT_TRUE(tbRepo.size() == 1, "tbRepo size phai bang 1 sau khi them");
    ASSERT_TRUE(tbRepo.findById("0981234567") != nullptr, "findById 0981234567 phai tim thay");

    GoiCuocMenu gcMenu(gcRepo);
    ThueBaoMenu tbMenu(tbRepo, gcRepo, khRepo, imeiRepo);

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

    std::remove(GC_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(KH_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
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

    std::remove(KH_TEST_FILE.c_str());
    std::remove(TB_TEST_FILE.c_str());
    std::remove(HD_TEST_FILE.c_str());
    std::remove(NT_TEST_FILE.c_str());
    std::remove(GHICUOCGOI_TEST_FILE.c_str());
    std::remove(INV_TEST_FILE.c_str());
    std::remove(KN_TEST_FILE.c_str());
    std::remove(IMEI_TEST_FILE.c_str());
}

