#include "common.h"
#include "Repository.h"
#include "imei.h"
#include "naptien.h"
#include "ghicuocgoi.h"
#include "hoadon.h"
#include "khieunai.h"
#include "hopdong.h"
#include "goicuoc.h"
#include "thuebao.h"
#include "khachhang.h"

void testCanonicalFixtureCounts() {
    std::cout << "[RUN] Kiem tra so luong fixture du lieu chuan (9 kho du lieu)...\n";

    Repository<ThietBiIMEI> imeiStore("data/imei.txt");
    Repository<NapTien> napTienStore("data/naptien.txt");
    Repository<CDR> cdrStore("data/ghicuocgoi.txt");
    Repository<HoaDon> hoaDonStore("data/hoadon.txt");
    Repository<KhieuNai> khieuNaiStore("data/khieunai.txt");
    Repository<HopDong> hopDongStore("data/hopdong.txt");
    Repository<GoiCuoc> goiCuocStore("data/CacGoiCuoc.txt");
    Repository<ThueBao> thueBaoStore("data/thuebao.txt");
    Repository<KhachHang> khachHangStore("data/khachhang.txt");

    ASSERT_TRUE(imeiStore.loadFromFile() && imeiStore.size() >= 10,
                "Fixture IMEI phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(napTienStore.loadFromFile() && napTienStore.size() >= 10,
                "Fixture nap tien phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(cdrStore.loadFromFile() && cdrStore.size() >= 10,
                "Fixture CDR phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(hoaDonStore.loadFromFile() && hoaDonStore.size() >= 10,
                "Fixture hoa don phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(khieuNaiStore.loadFromFile() && khieuNaiStore.size() >= 10,
                "Fixture khieu nai phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(hopDongStore.loadFromFile() && hopDongStore.size() >= 10,
                "Fixture hop dong phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(goiCuocStore.loadFromFile() && goiCuocStore.size() >= 10,
                "Fixture goi cuoc phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(thueBaoStore.loadFromFile() && thueBaoStore.size() >= 10,
                "Fixture thue bao phai nap du 10 ban ghi hop le");
    ASSERT_TRUE(khachHangStore.loadFromFile() && khachHangStore.size() >= 10,
                "Fixture khach hang phai nap du 10 ban ghi hop le");
}

