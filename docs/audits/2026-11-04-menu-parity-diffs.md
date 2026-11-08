# Executed menu parity diffs

Tracked code/test diffs compare HEAD b5d5493 with the working tree; GoiCuocMenu.cpp was already dirty, so that diff is not wholly session-owned. No exact pre-implementation working-file snapshot exists. New test/reproducer/report files compare against absent files. Existing continuity notes compare against the captured pre-closeout text. Generated graph artifacts remain tool-owned and ignored; their refresh and backup inventory are recorded in the implementation report. Unrelated dirty files, historical deletions and archives are excluded.

## CMakeLists.txt

```diff
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 1f2f508..e91cfc3 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -36,6 +36,7 @@ set(TEST_MODULE_SOURCES
     tests/modules/test_infrastructure.cpp
     tests/modules/test_models.cpp
     tests/modules/test_menus.cpp
+    tests/modules/test_menu_parity.cpp
     tests/modules/test_fixtures.cpp
 )
 
```

## src/lib/menus/GoiCuocMenu.cpp

```diff
diff --git a/src/lib/menus/GoiCuocMenu.cpp b/src/lib/menus/GoiCuocMenu.cpp
index a28ae8f..9ecf6c3 100644
--- a/src/lib/menus/GoiCuocMenu.cpp
+++ b/src/lib/menus/GoiCuocMenu.cpp
@@ -171,6 +171,7 @@ void GoiCuocMenu::capNhatGoiCuoc() {
     }
 
     gc->displayDetail();
+    //sua tren ban sao,de EOF giua chung khong luu thay doi dang do
     GoiCuoc updated = *gc;
     std::string tenMoi = InputHelper::getName("Nhap ten goi moi (bo trong de giu nguyen): ", true);
     if (std::cin.eof()) return;
@@ -184,8 +185,32 @@ void GoiCuocMenu::capNhatGoiCuoc() {
         updated.setGiaCuoc(giaMoi);
     }
 
+    bool doiData = InputHelper::getConfirm("Ban co muon thay doi dung luong data khong?");
+    if (std::cin.eof()) return;
+    if (doiData) {
+        int dataMoi = InputHelper::getInt("Nhap dung luong data moi (GB): ", 0);
+        if (std::cin.eof()) return;
+        updated.setDungLuongData(dataMoi);
+    }
+
+    bool doiPhut = InputHelper::getConfirm("Ban co muon thay doi so phut thoai khong?");
+    if (std::cin.eof()) return;
+    if (doiPhut) {
+        int phutMoi = InputHelper::getInt("Nhap so phut thoai moi: ", 0);
+        if (std::cin.eof()) return;
+        updated.setSoPhutGoi(phutMoi);
+    }
+
+    bool doiChuKy = InputHelper::getConfirm("Ban co muon thay doi chu ky ngay khong?");
+    if (std::cin.eof()) return;
+    if (doiChuKy) {
+        int chuKyMoi = InputHelper::getInt("Nhap chu ky ngay moi: ", 1, 365);
+        if (std::cin.eof()) return;
+        updated.setChuKyNgay(chuKyMoi);
+    }
+
     try {
-        //cap nhat ban sao,dam bao rollback neu loi
+        //chi luu danh muc goi,khong doi gia/han cua hop dong da lap
         store.update(maGoi, updated);
         std::cout << "[THANH CONG] Da cap nhat goi cuoc!\n";
     } catch (const std::exception& e) {
```

## src/lib/menus/ThietBiIMEIMenu.cpp

```diff
diff --git a/src/lib/menus/ThietBiIMEIMenu.cpp b/src/lib/menus/ThietBiIMEIMenu.cpp
index d517827..3920564 100644
--- a/src/lib/menus/ThietBiIMEIMenu.cpp
+++ b/src/lib/menus/ThietBiIMEIMenu.cpp
@@ -274,8 +274,9 @@ void ThietBiIMEIMenu::capNhatThietBi() {
               << "  [" << UPDATE_BTS << "] Cap nhat vi tri tram BTS\n"
               << "  [" << UPDATE_LOCK << "] Khoa mang thiet bi\n"
               << "  [" << UPDATE_UNLOCK << "] Mo khoa mang thiet bi\n"
+              << "  [" << UPDATE_METADATA << "] Sua ten thiet bi va hang san xuat\n"
               << "  [" << UPDATE_CANCEL << "] Huy bo thao tac\n";
-    int choice = InputHelper::getInt("Nhap lua chon [0-4]: ", UPDATE_CANCEL, UPDATE_UNLOCK);
+    int choice = InputHelper::getInt("Nhap lua chon [0-5]: ", UPDATE_CANCEL, UPDATE_METADATA);
 
     if (choice == UPDATE_CANCEL || std::cin.eof()) {
         std::cout << "[DA HUY] Thao tac cap nhat da duoc huy bo.\n";
@@ -331,6 +332,19 @@ void ThietBiIMEIMenu::capNhatThietBi() {
             store.update(imei, updated);
             std::cout << "[THANH CONG] Da MO KHOA MANG thiet bi " << imei << " (Trang thai: HoatDong)!\n";
         }
+
+        else if (choice == UPDATE_METADATA) {
+            //chi sua ten va hang,giu nguyen IMEI,SIM,BTS va trang thai khoa mang
+            std::string tenMoi = InputHelper::getName("Nhap ten thiet bi moi (bo trong de giu nguyen): ", true);
+            if (std::cin.eof()) return;
+            std::string hangMoi = InputHelper::getName("Nhap hang san xuat moi (bo trong de giu nguyen): ", true);
+            if (std::cin.eof()) return;
+            if (!tenMoi.empty()) updated.setTenThietBi(tenMoi);
+            if (!hangMoi.empty()) updated.setHangSanXuat(hangMoi);
+            //chi luu mot lan sau khi ca hai ten da duoc kiem tra
+            store.update(imei, updated);
+            std::cout << "[THANH CONG] Da cap nhat thong tin thiet bi " << imei << "!\n";
+        }
     } catch (const AppException& e) {
         std::cout << "[LOI] " << e.what() << "\n";
     }
```

## src/lib/menus/ThietBiIMEIMenu.h

```diff
diff --git a/src/lib/menus/ThietBiIMEIMenu.h b/src/lib/menus/ThietBiIMEIMenu.h
index c27339c..0d14960 100644
--- a/src/lib/menus/ThietBiIMEIMenu.h
+++ b/src/lib/menus/ThietBiIMEIMenu.h
@@ -32,6 +32,7 @@ namespace IMEIMenuChoices {
     const int UPDATE_BTS = 2;
     const int UPDATE_LOCK = 3;
     const int UPDATE_UNLOCK = 4;
+    const int UPDATE_METADATA = 5;
     const int UPDATE_CANCEL = 0;
 }
 
```

## src/lib/menus/KhachHangMenu.cpp

```diff
diff --git a/src/lib/menus/KhachHangMenu.cpp b/src/lib/menus/KhachHangMenu.cpp
index daa4e1b..3a84446 100644
--- a/src/lib/menus/KhachHangMenu.cpp
+++ b/src/lib/menus/KhachHangMenu.cpp
@@ -211,6 +211,7 @@ void KhachHangMenu::capNhatKhachHang() {
     }
 
     kh->displayDetail();
+    //sua tren ban sao,de huy hoac EOF khong luu thay doi dang do
     KhachHang updated = *kh;
 
     std::string hoTenMoi = InputHelper::getName("Nhap ho ten moi (bo trong de giu nguyen): ", true);
@@ -226,6 +227,20 @@ void KhachHangMenu::capNhatKhachHang() {
     if (!diaChiMoi.empty()) updated.setDiaChi(diaChiMoi);
 
     try {
+        //chi sua ngay/noi cap,khong thay ma khach hang va so giay to
+        std::string ngayCapMoi = InputHelper::getString("Nhap ngay cap moi (DD/MM/YYYY, 'now', bo trong de giu nguyen, '0' de huy): ", true);
+        if (std::cin.eof()) return;
+        if (ngayCapMoi == "0") {
+            std::cout << "[DA HUY] Thao tac cap nhat da duoc huy bo.\n";
+            return;
+        }
+        if (!ngayCapMoi.empty()) updated.setNgayCap(Date::parse(ngayCapMoi).toString());
+
+        std::string noiCapMoi = InputHelper::getString("Nhap noi cap moi (bo trong de giu nguyen): ", true);
+        if (std::cin.eof()) return;
+        if (!noiCapMoi.empty()) updated.setNoiCap(noiCapMoi);
+
+        //luu mot lan sau khi du lieu hop le,repository rollback neu ghi loi
         store.update(maKH, updated);
         std::cout << "[THANH CONG] Da cap nhat thong tin khach hang " << maKH << "!\n";
     } catch (const std::exception& e) {
```

## tests/modules/common.h

```diff
diff --git a/tests/modules/common.h b/tests/modules/common.h
index 4db6f96..4109953 100644
--- a/tests/modules/common.h
+++ b/tests/modules/common.h
@@ -90,6 +90,9 @@ void testSourceFileHeaders();
 void testMenuControllers();
 void testNewRepositoriesAndMenus();
 void testRemainingMenuControllers();
+void testPackageUpdateParity();
+void testDeviceMetadataUpdate();
+void testCustomerIssueMetadataUpdate();
 
 // 5. Fixture tests (test_fixtures.cpp)
 void testCanonicalFixtureCounts();
```

## tests/test_runner.cpp

```diff
diff --git a/tests/test_runner.cpp b/tests/test_runner.cpp
index 1b4f13c..e94689e 100644
--- a/tests/test_runner.cpp
+++ b/tests/test_runner.cpp
@@ -51,6 +51,9 @@ int main() {
     runTest("testSourceFileHeaders", testSourceFileHeaders);
     runTest("testBTSRegistry", testBTSRegistry);
     runTest("testRemainingMenuControllers", testRemainingMenuControllers);
+    runTest("testPackageUpdateParity", testPackageUpdateParity);
+    runTest("testDeviceMetadataUpdate", testDeviceMetadataUpdate);
+    runTest("testCustomerIssueMetadataUpdate", testCustomerIssueMetadataUpdate);
 
     // 5. Canonical Fixtures Verification
     runTest("testCanonicalFixtureCounts", testCanonicalFixtureCounts);
```

## tests/stress_test_runner.cpp

```diff
diff --git a/tests/stress_test_runner.cpp b/tests/stress_test_runner.cpp
index 2da18c8..da25f41 100644
--- a/tests/stress_test_runner.cpp
+++ b/tests/stress_test_runner.cpp
@@ -652,7 +652,7 @@ int main(int argc, char* argv[]) {
             metrics.totalTransitions++;
 
             //truong hop C:cap nhat goi cuoc
-            std::string updateInput = "5\n" + maGC + "\nUpdated Name\n1\n70000\n0\n";
+            std::string updateInput = "5\n" + maGC + "\nUpdated Name\ny\n70000\nn\nn\nn\n\n0\n";
             driveMenuWithInput(updateInput, [&]() { goiCuocMenu.showMenu(); });
             metrics.menuInvocations++;
             metrics.totalTransitions++;
```

## tests/lifecycle_3phase_runner.cpp

```diff
diff --git a/tests/lifecycle_3phase_runner.cpp b/tests/lifecycle_3phase_runner.cpp
index 670f0a7..a49802e 100644
--- a/tests/lifecycle_3phase_runner.cpp
+++ b/tests/lifecycle_3phase_runner.cpp
@@ -312,12 +312,12 @@ int main(int argc, char* argv[]) {
                 std::string targetKH = dynamicKhList[idxA];
                 std::string newPhone = generateRandomPhone(rng);
 
-                driveMenu("4\n" + targetKH + "\nOverwritten Name " + uidStr + "\n" + newPhone + "\nNew Street " + uidStr + "\n0\n",
+                driveMenu("4\n" + targetKH + "\nOverwritten Name " + uidStr + "\n" + newPhone + "\nNew Street " + uidStr + "\n\n\n0\n",
                           [&]() { khMenu.showMenu(); });
 
                 // 2. Overwrite Package details (Price, Data)
                 std::string targetGC = dynamicGcList[roundCounter % dynamicGcList.size()];
-                driveMenu("5\n" + targetGC + "\nUpdated GC " + uidStr + "\n120000\n0\n",
+                driveMenu("5\n" + targetGC + "\nUpdated GC " + uidStr + "\ny\n120000\ny\n10\nn\nn\n\n0\n",
                           [&]() { gcMenu.showMenu(); });
 
                 // 3. Credential Swapping between 2 customers (CCCD swap)
@@ -346,14 +346,14 @@ int main(int argc, char* argv[]) {
                     std::string originalPhoneA = curA->getSoDienThoai();
 
                     // Attempt updating contact phone via menu with another customer's existing phone
-                    driveMenu("4\n" + targetKH + "\n\n" + phoneOfB + "\n\n0\n",
+                    driveMenu("4\n" + targetKH + "\n\n" + phoneOfB + "\n\n\n\n0\n",
                               [&]() { khMenu.showMenu(); });
 
                     KhachHang* afterUpdateA = khRepo.findById(targetKH);
                     if (afterUpdateA && afterUpdateA->getSoDienThoai() == phoneOfB) {
                         metrics.dupPersonalUpdateAllowed++;
                         // Restore original phone
-                        driveMenu("4\n" + targetKH + "\n\n" + originalPhoneA + "\n\n0\n",
+                        driveMenu("4\n" + targetKH + "\n\n" + originalPhoneA + "\n\n\n\n0\n",
                                   [&]() { khMenu.showMenu(); });
                     } else {
                         metrics.dupPersonalUpdateExcepted++;
```

## tests/modules/test_menu_parity.cpp

```diff
--- /dev/null
+++ b/tests/modules/test_menu_parity.cpp
@@ -0,0 +1,155 @@
+#include "common.h"
+#include "GoiCuocMenu.h"
+#include "ThietBiIMEIMenu.h"
+#include "KhachHangMenu.h"
+
+namespace {
+template <typename T, typename Menu>
+void checkUpdate(Repository<T>& repo, Menu& menu, const std::string& path,
+                 const T& seed, const std::string& input, const T& expected) {
+    repo.update(seed.getId(), seed);
+    {
+        std::istringstream in(input);
+        std::ostringstream out;
+        StreamRedirectGuard streams;
+        std::cin.clear();
+        std::cin.rdbuf(in.rdbuf());
+        std::cout.rdbuf(out.rdbuf());
+        menu.showMenu();
+    }
+    Repository<T> disk(path);
+    ASSERT_TRUE(disk.loadFromFile(), "Updated fixture must reload");
+    const T* saved = disk.findById(seed.getId());
+    ASSERT_TRUE(saved && saved->toFileString() == expected.toFileString(),
+                "Persisted record must match all expected fields");
+    const T* memory = repo.findById(seed.getId());
+    ASSERT_TRUE(memory && memory->toFileString() == expected.toFileString(),
+                "Memory and disk must agree");
+}
+}
+
+void testPackageUpdateParity() {
+    const std::string path = "data/test_package_parity.txt";
+    FileCleanupGuard cleanup({path});
+    Repository<GoiCuoc> repo(path);
+    repo.clear();
+    GoiCuoc seed("AUDIT", "Original", 50000, 5, 100, 30);
+    repo.add(seed);
+    GoiCuocMenu menu(repo);
+    const std::string start = "5\nAUDIT\n";
+    const std::string end = "\n0\n"; //menu goi cuoc dung cho Enter truoc khi quay lai
+
+    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\nn\n" + end, seed);
+    GoiCuoc expected = seed;
+    expected.setTenGoi("Renamed");
+    checkUpdate(repo, menu, path, seed, start + "Renamed\nn\nn\nn\nn\n" + end, expected);
+    expected = seed;
+    expected.setGiaCuoc(60000);
+    checkUpdate(repo, menu, path, seed, start + "\ny\n60000\nn\nn\nn\n" + end, expected);
+    expected = seed;
+    expected.setDungLuongData(10);
+    checkUpdate(repo, menu, path, seed, start + "\nn\ny\n10\nn\nn\n" + end, expected);
+    expected = seed;
+    expected.setSoPhutGoi(250);
+    checkUpdate(repo, menu, path, seed, start + "\nn\nn\ny\n250\nn\n" + end, expected);
+    expected = seed;
+    expected.setChuKyNgay(60);
+    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\ny\n60\n" + end, expected);
+
+    expected = seed;
+    expected.setGiaCuoc(0);
+    expected.setDungLuongData(0);
+    expected.setSoPhutGoi(0);
+    expected.setChuKyNgay(1);
+    checkUpdate(repo, menu, path, seed,
+        start + "\ny\n-1\n0\ny\n-1\n0\ny\n-1\n0\ny\n0\n366\n1\n" + end, expected);
+    expected = seed;
+    expected.setChuKyNgay(365);
+    checkUpdate(repo, menu, path, seed, start + "\nn\nn\nn\ny\n365\n" + end, expected);
+    checkUpdate(repo, menu, path, seed, start + "Pending\ny\n60000\ny\n", seed);
+    checkUpdate(repo, menu, path, seed, "5\nMISSING\n" + end, seed);
+}
+
+void testDeviceMetadataUpdate() {
+    const std::string path = "data/test_device_metadata.txt";
+    const std::string phones = "data/test_device_metadata_phones.txt";
+    FileCleanupGuard cleanup({path, phones});
+    Repository<ThietBiIMEI> repo(path);
+    Repository<ThueBao> subscribers(phones);
+    repo.clear();
+    subscribers.clear();
+    ThietBiIMEI seed("860123456789014", "Original", "Apple", "0981234567",
+                    Date(15, 1, 2024), "KhoaMang", "BTS-HN-001");
+    repo.add(seed);
+    ThietBiIMEIMenu menu(repo, subscribers);
+    const std::string start = "6\n860123456789014\n5\n";
+    const std::string end = "\n0\n";
+    ThietBiIMEI expected = seed;
+    expected.setTenThietBi("New Model");
+    expected.setHangSanXuat("New Brand");
+    checkUpdate(repo, menu, path, seed, start + "New Model\nNew Brand\n" + end, expected);
+    checkUpdate(repo, menu, path, seed, start + "\n\n" + end, seed);
+    expected = seed;
+    expected.setTenThietBi(std::string(50, 'A'));
+    checkUpdate(repo, menu, path, seed,
+        start + std::string(51, 'A') + "\nBad|Name\n" + std::string(50, 'A') + "\n\n" + end, expected);
+    checkUpdate(repo, menu, path, seed, start + "Pending\n", seed);
+    checkUpdate(repo, menu, path, seed, "6\n860123456789014\n0\n" + end, seed);
+    checkUpdate(repo, menu, path, seed, "6\nMISSING\n" + end, seed);
+}
+
+void testCustomerIssueMetadataUpdate() {
+    const std::string path = "data/test_customer_metadata.txt";
+    const std::string phones = "data/test_customer_metadata_phones.txt";
+    const std::string contracts = "data/test_customer_metadata_contracts.txt";
+    FileCleanupGuard cleanup({path, phones, contracts});
+    Repository<KhachHang> repo(path);
+    Repository<ThueBao> subscribers(phones);
+    Repository<HopDong> agreements(contracts);
+    repo.clear();
+    subscribers.clear();
+    agreements.clear();
+    KhachHang seed("KH0001", "Nguyen Van An", "CCCD", "001201004567",
+                  "10/01/2021", "Cuc C06", "0981234567", "Ha Noi");
+    repo.add(seed);
+    KhachHangMenu menu(repo, subscribers, agreements);
+    const std::string start = "4\nKH0001\n\n\n\n";
+    KhachHang expected = seed;
+    expected.setNgayCap("29/02/2024");
+    expected.setNoiCap("Cuc C08");
+    checkUpdate(repo, menu, path, seed, start + "29/02/2024\nCuc C08\n0\n", expected);
+    expected = seed;
+    expected.setNoiCap("Cuc C08");
+    checkUpdate(repo, menu, path, seed, start + "\nBad|Authority\nCuc C08\n0\n", expected);
+    checkUpdate(repo, menu, path, seed, start + "\n\n0\n", seed);
+    checkUpdate(repo, menu, path, seed, "4\nKH0001\nPending\n\n\n0\n0\n", seed);
+    checkUpdate(repo, menu, path, seed, "4\nKH0001\nPending\n\n\n29/02/2023\n0\n", seed);
+    checkUpdate(repo, menu, path, seed, start + "01/01/2024junk\n0\n", seed);
+    checkUpdate(repo, menu, path, seed, start + "15/05/2023\n", seed);
+    checkUpdate(repo, menu, path, seed, "4\nMISSING\n0\n", seed);
+
+    //chap nhan ngay truoc hoac sau neu kiem thu chay qua nua dem
+    const std::string before = Date::now().toString();
+    {
+        std::istringstream in(start + "now\n\n0\n");
+        std::ostringstream out;
+        StreamRedirectGuard streams;
+        std::cin.clear();
+        std::cin.rdbuf(in.rdbuf());
+        std::cout.rdbuf(out.rdbuf());
+        repo.update(seed.getId(), seed);
+        menu.showMenu();
+    }
+    const std::string after = Date::now().toString();
+    Repository<KhachHang> disk(path);
+    ASSERT_TRUE(disk.loadFromFile(), "Current issue date must reload");
+    const KhachHang* saved = disk.findById(seed.getId());
+    ASSERT_TRUE(saved && (saved->getNgayCap() == before || saved->getNgayCap() == after),
+                "Now must use the current local date");
+    if (saved) {
+        expected = seed;
+        expected.setNgayCap(saved->getNgayCap());
+        ASSERT_TRUE(saved->toFileString() == expected.toFileString(),
+                    "Now must preserve customer identity and other metadata");
+    }
+}
```

## tests/repro/repro_20261104_004646_package_update_parity.sh

```diff
--- /dev/null
+++ b/tests/repro/repro_20261104_004646_package_update_parity.sh
@@ -0,0 +1,28 @@
+#!/usr/bin/env bash
+# Autogenerated by reproducer-mint.sh on 2026-11-03T17:46:46Z
+# Target: package-update-parity
+set -eo pipefail
+
+TIMEOUT_SEC=15
+EXPECTED_SIG=""
+
+run_target() {
+python3 -B /home/vgbn/.claude/jobs/d64cc50d/tmp/verify_menu_implementation.py test_runner
+}
+
+echo "[repro] Running: package-update-parity"
+set +e
+OUTPUT=$(run_target 2>&1)
+EXIT_CODE=$?
+set -e
+
+echo "${OUTPUT}"
+
+if [[ -n "${EXPECTED_SIG}" ]]; then
+  if ! echo "${OUTPUT}" | grep -qF "${EXPECTED_SIG}"; then
+    echo "[repro] ERROR: Expected signature '${EXPECTED_SIG}' not found in output." >&2
+    exit 2
+  fi
+fi
+
+exit ${EXIT_CODE}
```

## tests/repro/repro_20261104_005228_device_metadata_update.sh

```diff
--- /dev/null
+++ b/tests/repro/repro_20261104_005228_device_metadata_update.sh
@@ -0,0 +1,28 @@
+#!/usr/bin/env bash
+# Autogenerated by reproducer-mint.sh on 2026-11-03T17:52:28Z
+# Target: device-metadata-update
+set -eo pipefail
+
+TIMEOUT_SEC=15
+EXPECTED_SIG=""
+
+run_target() {
+python3 -B /home/vgbn/.claude/jobs/d64cc50d/tmp/verify_menu_implementation.py test_runner
+}
+
+echo "[repro] Running: device-metadata-update"
+set +e
+OUTPUT=$(run_target 2>&1)
+EXIT_CODE=$?
+set -e
+
+echo "${OUTPUT}"
+
+if [[ -n "${EXPECTED_SIG}" ]]; then
+  if ! echo "${OUTPUT}" | grep -qF "${EXPECTED_SIG}"; then
+    echo "[repro] ERROR: Expected signature '${EXPECTED_SIG}' not found in output." >&2
+    exit 2
+  fi
+fi
+
+exit ${EXIT_CODE}
```

## tests/repro/repro_20261104_005414_customer_issue_metadata.sh

```diff
--- /dev/null
+++ b/tests/repro/repro_20261104_005414_customer_issue_metadata.sh
@@ -0,0 +1,28 @@
+#!/usr/bin/env bash
+# Autogenerated by reproducer-mint.sh on 2026-11-03T17:54:14Z
+# Target: customer-issue-metadata
+set -eo pipefail
+
+TIMEOUT_SEC=15
+EXPECTED_SIG=""
+
+run_target() {
+python3 -B /home/vgbn/.claude/jobs/d64cc50d/tmp/verify_menu_implementation.py test_runner
+}
+
+echo "[repro] Running: customer-issue-metadata"
+set +e
+OUTPUT=$(run_target 2>&1)
+EXIT_CODE=$?
+set -e
+
+echo "${OUTPUT}"
+
+if [[ -n "${EXPECTED_SIG}" ]]; then
+  if ! echo "${OUTPUT}" | grep -qF "${EXPECTED_SIG}"; then
+    echo "[repro] ERROR: Expected signature '${EXPECTED_SIG}' not found in output." >&2
+    exit 2
+  fi
+fi
+
+exit ${EXIT_CODE}
```

## docs/audits/2026-11-04-menu-parity-implementation.md

```diff
--- /dev/null
+++ b/docs/audits/2026-11-04-menu-parity-implementation.md
@@ -0,0 +1,78 @@
+# Menu update parity implementation
+
+Date: 2026-11-04. Baseline: [create/update audit](2026-11-03-create-update-parity.md).
+
+## Batch ledger
+
+| Batch | Ownership and behavior | State | Evidence |
+| --- | --- | --- | --- |
+| CP-01/CP-02 | Package menu: optional data/minutes/cycle setters | verified/reviewed; feature closed, aggregate gate exception retained | Exit-1 baseline: 447 passed/14 failed; after fix: 461 passed/0 failed |
+| META-01 | Device menu: action 5 corrects labels only | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 479 passed/0 failed; PTY and quick stress exit 0 |
+| META-02 | Customer menu: optional issue date/authority | verified/reviewed; closed with aggregate exception | Exit-1 baseline minted; after fix: 506 passed/0 failed; PTY, quick stress and lifecycle exit 0 |
+
+## Package evidence
+
+- Reproducer: `tests/repro/repro_20261104_004646_package_update_parity.sh`; mint confirmed exit 1 before production edits. Its command uses this session's isolated build driver; durable assertions live in `tests/modules/test_menu_parity.cpp` and are registered in CMake.
+- Debug C++11 application/unit/stress/lifecycle build: exit 0.
+- Unit runner in copied-data cwd with source symlink: 461 passed, 0 failed, exit 0. Cases cover independent fields, unchanged values, zero, invalid numeric retries, cycle 1/365, EOF cancellation and missing ID. Existing assertions retained.
+- Quick stress `--duration 5`: exit 0; 7,340 menu invocations, not a successful-update count. Debug build is not sanitizer qualification despite the runner's static banner.
+- POSIX PTY probes: package persisted `AUDIT|Original|60000|10|250|60`; associated contract serialization retained price 50000 and expiry 31/01/2024. Failed-save case obstructed only an isolated fixture parent, reported an error, restored memory and retained the original disk record. Empty lookup created no package. All exited 0.
+- Before/after menu classifier: 58 functions, no placement violations/duplicates. YAGNI auditor: exit 0.
+- Adversarial inspection: zero remains a value, setters receive validated values, EOF returns before save, one repository update, no identity/history propagation.
+- Live data hashes unchanged throughout isolated runs.
+
+## Device evidence
+
+- Reproducer `tests/repro/repro_20261104_005228_device_metadata_update.sh` confirmed exit 1 before production changes; direct setters were exercised by expected-record construction while the existing menu rejected action 5.
+- Registered real menu/reload cases cover both labels, blanks, 50/51-character boundaries, pipe retry, EOF, cancellation and missing ID; final runner: 479 passed, zero failed.
+- PTY reload: `860123456789014|New Model|New Brand|0981234567|15/01/2024|KhoaMang|BTS-HN-001`. Identity, mapping, activation date, blacklist and BTS preserved.
+- Quick stress exit 0 (7,572 menu invocations); live data hashes unchanged. Existing SIM/blacklist tests retained and passed in unit runner.
+- Review: action numbers 1–4 unchanged; validated inputs finish before setters and one repository save; EOF cannot publish the first label. Classifier/YAGNI/whitespace pass; aggregate integrity exception unchanged.
+
+## Customer evidence
+
+- Reproducer `tests/repro/repro_20261104_005414_customer_issue_metadata.sh` confirmed exit 1 before implementation. Existing menu saved after address without reading issue metadata; direct setters/serialized reload supported the expected fields.
+- Final units: 506 passed, zero failed. Real reload cases cover leap day, authority-only edit, blank preservation, delimiter retry, cancellation of earlier name edits, invalid leap day/trailing text, EOF before authority, missing ID and `now` across midnight.
+- PTY reload: `KH0001|Nguyen Van An|CCCD|001201004567|29/02/2024|Cuc C08|0981234567|Ha Noi`.
+- Debug C++11 application/unit/stress/lifecycle build exit 0. Quick stress exit 0 (7,598 menu invocations); lifecycle `--quick` exit 0, all five repository counts returned to baseline, zero residual records. Counts are runner metrics, not independently proven successful-update totals.
+- Review: date is explicitly parsed because model setters only assign; ID/CCCD/type remain immutable; one save after all inputs; invalid date/cancel/EOF leaves pending copy unpublished. Approved three lifecycle input payload corrections applied without assertion changes.
+- Classifier/YAGNI/whitespace pass. Live data hashes unchanged. Aggregate anti-cheat exception retained.
+
+## Final verification and limitations
+
+- Fresh isolated unit rerun on 2026-11-04: 506 passed, 0 failed, exit 0; live-data SHA-256 values match both the before-run values and implementation baseline. Expected invalid-BTS-fixture diagnostic is not a failed assertion.
+- Isolated CTest: 2/3 passed, exit 8. `unit_tests` and `stress_test_quick` passed. `startup_from_other_directory` failed with `/usr/bin/bash: /home/vgbn/Documents/codeptit/KTLT/tests/repro/startup_working_directory.sh: No such file or directory`. The script's staged deletion predates implementation and remains untouched. The harness retained registered commands but changed their working directories and live-data argument to a copied fixture; this is not an untouched source-cwd CTest execution.
+- Documentation referee reported 45 stale-reference candidates across six existing documents. Shorthand paths, historical archive paths and missing historical files require individual screening; do not label all candidates confirmed defects. No broad documentation repair was attempted.
+- `graphify update .` exited 0: 1011 nodes, 2203 edges, 43 communities; regenerated graph JSON/HTML, report and one callflow. Saved labels describe 61 communities, so 25 current communities received hub-derived names. No LLM relabeling performed. Tool-created backup retained under `graphify-out/2026-11-04/`: `.graphify_labels.json`, `manifest.json`, `graph.json`, `.graphify_analysis.json`, `GRAPH_REPORT.md`.
+- All three minted reproducer scripts were inspected: they depend on the session's scratch driver/build and are not portable after scratch cleanup. Permanent registered assertions are in `tests/modules/test_menu_parity.cpp`. Preserve historical mint evidence rather than rewriting its commands to imply portability.
+- Full explicit tracked/new-file diffs: `2026-11-04-menu-parity-diffs.md`. Tracked code diffs use HEAD, not a pristine pre-implementation working snapshot; package source was already dirty, so its entire HEAD diff cannot be attributed to this session.
+- Historical-audit preservation probe exited 1 on `data/CacGoiCuoc.txt`. Differential inspection found three historical data differences (`CacGoiCuoc.txt`, `hopdong.txt`, `khachhang.txt`), all already present in the implementation-start hash baseline; current data matches that baseline exactly. The old audit snapshot also differs at the already-dirty `fileio.h`, so it cannot certify whole-session preservation. No attempt was made to restore these user-owned files.
+- Fresh final POSIX PTY rerun exercised all five modes together: package, blocked-save rollback, empty lookup, device labels and customer issue metadata. Every process exited 0, real assertions passed, expected rollback error was displayed, deserialized records matched, and live-data hashes were unchanged.
+- Final independent read-only review returned no concrete regressions in the three update paths, action constants and parity tests. This is scoped source review, not whole-repository certification. Its observation about package pause behavior was checked: `capNhatGoiCuoc` itself does not pause, but `showMenu` calls `InputHelper::pause()` after dispatch at line 58; the actual menu still pauses, consistent with PTY payloads. Existing customer blank-phone behavior preserves the phone and does not offer clearing it; that is outside the approved metadata scope.
+- No all-green acceptance claim is permitted while the startup and integrity gates remain red. Final quick lifecycle rerun exited 0, restored all five repository counts and left live-data hashes unchanged.
+- An agent worktree was inadvertently requested for final review; the reviewer was redirected to live absolute paths, read-only. Existing worktrees were inventoried, not removed. No commit/push/merge or archive/history/timestamp manipulation was performed.
+
+## Known aggregate gate exception
+
+`anti-cheat-enforcer --diff --require-tests --no-revert`: exit 1, reports 23 production files/zero tests in its working-tree group. The 23 staged deleted historical scripts predate this work. New registered test source is untracked, and tracked test wiring/input corrections do exist; this tool output is not a valid count of tests executed. The gate was invoked with `--no-revert`; historical assertions were not removed by this implementation. Its appended integrity-log entries still say `File auto-reverted via git checkout -- working_tree`; that wording is not proof a real file was reverted. Fresh native tests and the inspected feature diff remain intact. Do not claim aggregate integrity acceptance.
+
+## Scope and rulings
+
+- Ruling: zero is data, not skip — y/n distinguishes unchanged values — cost if wrong: accidental catalog edits.
+- Ruling: correct labels/issue metadata, not identity — preserve relationships — cost if wrong: orphaned records.
+- Ruling: retain unrelated dirty files and red gates — feature approval is not cleanup approval — cost if wrong: lost work/evidence.
+- User changed comment preference during execution: new comments use lowercase unaccented Vietnamese like surrounding source, rather than the plan's English comments.
+- IDs, existing contract snapshots, historical records, archives and Git metadata remain outside scope. No commit/push/merge or archive regeneration authorized.
+
+## Verification commands
+
+Use a disposable copied-data working directory with a source symlink when running native tests; CTest's configured source working directory must not be allowed to write live data. Session scripts/builds are under `$CLAUDE_JOB_DIR/tmp` and are not permanent build prerequisites.
+
+```bash
+cmake -S "$PWD" -B "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" -DCMAKE_BUILD_TYPE=Debug
+cmake --build "$CLAUDE_JOB_DIR/tmp/parity-implementation-build" --target test_runner quanlythuebao stress_test_runner lifecycle_3phase_runner -j 4
+# Run test_runner from the disposable fixture cwd, not live data/.
+function-family-classifier check src/lib/menus
+yagni-diff-auditor
+anti-cheat-enforcer --diff --require-tests --no-revert
+```
```

## workspace/handoff/2026/11/2026-11-04.md

```diff
--- /dev/null
+++ b/workspace/handoff/2026/11/2026-11-04.md
@@ -0,0 +1,24 @@
+# 2026-11-04 — Menu update parity
+
+Approved scope: package parity plus device/customer metadata corrections and teammate-readable comments. User subsequently requested Vietnamese comments matching surrounding source.
+
+Evidence: `docs/audits/2026-11-04-menu-parity-implementation.md`.
+
+- Package batch: exit-1 baseline minted; fixed controller and approved scripted package inputs. Units 461/0; quick stress and three real PTY boundary probes pass; contract snapshot and live data unchanged.
+- Device batch closed: action 5 corrects labels only. Exit-1 baseline, 479/0 units, PTY reload and quick stress passed; unrelated device fields and live data unchanged.
+- Customer batch closed: optional parsed issue date/authority; cancellation/EOF never publishes pending edits. Exit-1 baseline, 506/0 units, PTY, quick stress and nine-second lifecycle pass; IDs/CCCD/live data unchanged.
+- Final fresh units: 506/0, exit 0; live-data hashes match implementation baseline. Isolated CTest: 2/3, exit 8; startup script remains absent due to pre-existing staged deletion. No test restored or bypassed.
+- Graph refresh exited 0 (1011 nodes/2203 edges/43 communities); curated backup retained under graphify-out/2026-11-04. Saved community labels need separate optional review (61 versus 43).
+- Documentation scan: 45 reference candidates across six existing files, not 45 verified defects; broad repair excluded. Exact executed diffs recorded in docs/audits/2026-11-04-menu-parity-diffs.md, with HEAD-versus-dirty-baseline attribution limitation.
+- Final independent scoped review: no concrete regressions. Corrected reviewer pause observation against package showMenu line 58; actual dispatch pauses. Fresh all-five PTY and quick lifecycle reruns passed. Minted scripts retain scratch dependencies; permanent parity tests are registered in CMake. Agent worktree creation was inadvertent; reviewer redirected read-only to live paths, worktrees retained.
+- Older audit-hash probe failed on package data; all three historical data differences already existed in implementation-start hashes, which still match current data. Do not claim whole-session preservation from the older snapshot. No user-data restoration attempted.
+- Next: obtain separate scope approval before restoring missing historical tests, correcting integrity-tool classification, or changing submission artifacts. Feature verification is delivered; aggregate acceptance remains blocked.
+- Aggregate anti-cheat remains red on the pre-existing 23 deleted-script group; preserve it and do not claim full acceptance.
+- No commits, pushes, merges, archive edits or timestamp manipulation.
+
+## Rulings & Invariant Decisions
+
+- Ruling: zero is data, not skip — explicit y/n preserves the distinction — cost if wrong: accidental catalog edits.
+- Ruling: edit labels/issue metadata, not identity — preserve existing relationships — cost if wrong: orphaned or reassigned records.
+- Ruling: retain unrelated dirty files and failing tests — feature approval is not cleanup approval — cost if wrong: evidence or user-work loss.
+- Ruling: follow latest user comment preference — lowercase unaccented Vietnamese matches surrounding code — cost if wrong: inconsistent teammate guidance.
```

## workspace/STATE.md

```diff
--- a/workspace/STATE.md
+++ b/workspace/STATE.md
@@ -1,5 +1,11 @@
 # Workspace State: KTLT Mobile Subscriber Management System
-## Last Updated: 2026-11-03
+## Last Updated: 2026-11-04
+
+### Session 2026-11-04 - Approved menu parity implementation
+- Package update now persists optional data/minutes/cycle edits; device action 5 corrects labels; customer update accepts parsed issue date/authority. New intent comments follow the requested Vietnamese style.
+- Fresh isolated units: 506/0; build, real PTY boundaries, quick stress and quick lifecycle passed. Live-data hashes unchanged; identities and existing contract snapshots preserved.
+- Aggregate acceptance remains blocked: isolated CTest 2/3 (pre-existing missing startup script); non-reverting integrity gate exit 1 on its 23-file/zero-test classification. Documentation scan has 45 unverified reference candidates. Final independent scoped review returned no concrete regressions.
+- Evidence: docs/audits/2026-11-04-menu-parity-implementation.md and dated handoff. Graph refreshed; no commit/push/archive/history changes.
 
 ### Session 2026-11-03 - Create/update parity audit
 - Audit only; no production fixes or metadata/history/archive edits.
```

## workspace/NEXT_SESSION_GOAL.md

```diff
--- a/workspace/NEXT_SESSION_GOAL.md
+++ b/workspace/NEXT_SESSION_GOAL.md
@@ -3,14 +3,14 @@
 ## Status
 - Submission completed with `Nhóm 04.zip`.
 - Base ZIP verified functional: 426/426 unit assertions passed, quick stress passed, clean startup.
-- Current codebase verified: 431/431 unit assertions passed.
+- Current codebase verified on 2026-11-04: 506 unit assertions passed, zero failed, using isolated data; this does not replace the submitted ZIP baseline.
 - Known packaging artifact: `startup_working_directory.sh` registered in CMake but omitted from ZIP (affects CTest run only, not binary execution).
 
-## Pending create/update parity audit (2026-11-03)
-- Audit only: no production fixes authorized or applied.
-- `GoiCuocMenu` drops entered minutes/cycle values and has no data allowance edit; direct model setters persist all three fields.
-- Probe: `tests/audit_menu_parity.cpp`; detailed evidence and acceptance criteria: `docs/audits/2026-11-03-create-update-parity.md`.
-- Before a fix, agree on optional numeric-edit behavior; preserve identifiers, contract snapshots, and unrelated dirty files.
+## Menu parity implementation (2026-11-04)
+- Approved package, device-label and customer issue-metadata changes implemented with real persisted-record tests and Vietnamese intent comments.
+- Evidence: docs/audits/2026-11-04-menu-parity-implementation.md. Existing identifiers, contract snapshots and unrelated dirty work preserved; submitted archive unchanged.
+- Independent scoped review found no concrete regressions. Next engineering action: separately resolve the missing startup-test script and aggregate integrity classification with explicit scope approval. Do not claim full-suite green or modify failing historical tests just to pass gates.
+- Teacher acceptance of a corrected version is unknown; ask which submission is authoritative rather than modifying archive/history metadata.
 
 ## Oral Defense Focus Areas
 See memory: `next-session-oral-defense-imei-hopdong-shared-2026-10-23.md`
```

## workspace/SESSION_MEMORY.md

```diff
--- a/workspace/SESSION_MEMORY.md
+++ b/workspace/SESSION_MEMORY.md
@@ -10,6 +10,11 @@
 - Audit-only evidence: `docs/audits/2026-11-03-create-update-parity.md`; real menu/reload probe: `tests/audit_menu_parity.cpp`. Package minutes/cycle inputs are discarded; data allowance has no edit path. No production repairs applied.
 - Sibling metadata correction gaps remain source-only candidates, not proved defects; contract ownership transfer requires an explicit consistency policy. Preserve dedicated lifecycle actions and contract snapshots.
 - Fresh isolated units 431/431 and quick stress pass; package parity failures remain expected evidence. Existing startup script deletion and aggregate anti-cheat rejection prevent a clean acceptance claim. History and archive metadata unchanged.
+
+## Approved menu parity implementation (2026-11-04)
+- Supersedes the audit-only status above: package numeric parity, device labels and customer issue metadata implemented. Real registered parity tests: 506 total assertions pass; isolated PTY/stress/lifecycle preserve live data and immutable identity/contract fields.
+- Evidence and limits: docs/audits/2026-11-04-menu-parity-implementation.md. Startup CTest and integrity gates remain red; do not confuse feature verification with aggregate acceptance. Minted shell scripts depend on disposable scratch; permanent CMake-registered tests do not.
+- User requested Vietnamese intent comments matching surrounding code. No archive, history, timestamp, commit or push changes authorized.
 
 ## Durable Architecture Invariants
 - **Vertical Diagram Stacking (`direction TB`):** Keep Mermaid subgraphs and sequence diagrams vertically stacked to prevent wide horizontal expansion that crushes inside partitioned VS Code editor panes.
```

## workspace/PROMPT_LOG.md

```diff
--- a/workspace/PROMPT_LOG.md
+++ b/workspace/PROMPT_LOG.md
@@ -1,3 +1,9 @@
+
+## [2026-11-04] Approved menu parity implementation
+- Prompt: plan via mass implement with clearer teammate comments; approved package + metadata scope and per-numeric-field confirmation. Later preference: Vietnamese comments like surrounding source.
+- Result: package data/minutes/cycle persistence, device label action 5, customer issue date/authority correction; real menu/reload regression module and approved scripted input corrections.
+- Verification: fresh isolated units 506/0; build, PTY, quick stress/lifecycle pass; unchanged live-data hashes. Isolated CTest 2/3 (missing historical startup script), aggregate integrity rejection retained; final independent scoped review found no concrete regressions.
+- Evidence: docs/audits/2026-11-04-menu-parity-implementation.md. No commit/push/archive/history/timestamp manipulation.
 
 ## [2026-11-03] Create/update feature parity audit
 - Prompt: `/session-orchestrator audit of lack of features, such as what is in create, isnt available in update like the goicuocmenu`.
```
