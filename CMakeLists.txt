cmake_minimum_required(VERSION 3.10)
project(QuanLyThueBao VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src src/lib src/lib/shared src/lib/models src/lib/menus tests/modules)

set(LIB_SOURCES
    src/lib/shared/Date.cpp
    src/lib/shared/InputHelper.cpp
    src/lib/shared/Normalized.cpp
    src/lib/shared/fileio.cpp
    src/lib/shared/BTSRegister.cpp
    src/lib/models/hopdong.cpp
    src/lib/models/imei.cpp
    src/lib/models/khachhang.cpp
    src/lib/models/goicuoc.cpp
    src/lib/models/thuebao.cpp
    src/lib/models/hoadon.cpp
    src/lib/models/naptien.cpp
    src/lib/models/ghicuocgoi.cpp
    src/lib/models/khieunai.cpp
    src/lib/menus/HopDongMenu.cpp
    src/lib/menus/ThietBiIMEIMenu.cpp
    src/lib/menus/GoiCuocMenu.cpp
    src/lib/menus/ThueBaoMenu.cpp
    src/lib/menus/KhachHangMenu.cpp
    src/lib/menus/NapTienMenu.cpp
    src/lib/menus/HoaDonMenu.cpp
)

set(TEST_MODULE_SOURCES
    tests/modules/test_shared.cpp
    tests/modules/test_infrastructure.cpp
    tests/modules/test_models.cpp
    tests/modules/test_menus.cpp
    tests/modules/test_fixtures.cpp
)

add_executable(quanlythuebao src/main.cpp ${LIB_SOURCES})

add_executable(test_runner
    tests/test_runner.cpp
    ${TEST_MODULE_SOURCES}
    ${LIB_SOURCES}
)

add_executable(stress_test_runner
    tests/stress_test_runner.cpp
    ${LIB_SOURCES}
)

enable_testing()
add_test(NAME unit_tests COMMAND test_runner)
set_tests_properties(unit_tests PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_test(NAME stress_test_quick COMMAND stress_test_runner --duration 5)
set_tests_properties(stress_test_quick PROPERTIES WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_test(NAME startup_from_other_directory
    COMMAND bash "${CMAKE_CURRENT_SOURCE_DIR}/tests/repro/startup_working_directory.sh"
            "$<TARGET_FILE:quanlythuebao>"
            "${CMAKE_CURRENT_SOURCE_DIR}/data")
