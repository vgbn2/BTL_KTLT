cmake_minimum_required(VERSION 3.10)
project(QuanLyThueBao VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src src/lib)

set(LIB_SOURCES
    src/lib/Date.cpp
    src/lib/InputHelper.cpp
    src/lib/fileio.cpp
    src/lib/hopdong.cpp
    src/lib/imei.cpp
    src/lib/HopDongMenu.cpp
    src/lib/ThietBiIMEIMenu.cpp
)

add_executable(quanlythuebao src/main.cpp ${LIB_SOURCES})

add_executable(test_runner
    tests/test_runner.cpp
    src/lib/Date.cpp
    src/lib/InputHelper.cpp
    src/lib/fileio.cpp
    src/lib/hopdong.cpp
    src/lib/imei.cpp
)
