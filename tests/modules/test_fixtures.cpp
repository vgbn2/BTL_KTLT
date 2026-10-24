#include "common.h"
#include "Repository.h"
#include "imei.h"
#include "naptien.h"
#include "ghicuocgoi.h"
#include "hoadon.h"
#include "khieunai.h"

void testCanonicalFixtureCounts() {
    std::cout << "[RUN] Kiem tra so luong fixture du lieu chuan...\n";

    Repository<ThietBiIMEI> imeiStore("data/imei.txt");
    Repository<NapTien> napTienStore("data/naptien.txt");
    Repository<CDR> cdrStore("data/ghicuocgoi.txt");
    Repository<HoaDon> hoaDonStore("data/hoadon.txt");
    Repository<KhieuNai> khieuNaiStore("data/khieunai.txt");

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
}

