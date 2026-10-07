cmake_minimum_required(VERSION 3.10)
project(QuanLyThueBao VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src src/lib src/lib/shared src/lib/models src/lib/menus src/lib/stubs)

set(LIB_SOURCES
    src/lib/shared/Date.cpp
    src/lib/shared/InputHelper.cpp
    src/lib/shared/Normalized.cpp
    src/lib/shared/fileio.cpp
    src/lib/models/hopdong.cpp
    src/lib/models/imei.cpp
    src/lib/menus/HopDongMenu.cpp
    src/lib/menus/ThietBiIMEIMenu.cpp
)

add_executable(quanlythuebao src/main.cpp ${LIB_SOURCES})

add_executable(test_runner
    tests/test_runner.cpp
    ${LIB_SOURCES}
)
