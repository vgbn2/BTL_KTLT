@echo off
chcp 65001 > nul
title Quan Ly Thue Bao Di Dong - PTIT KTLT

echo ====================================================================
echo        HE THONG QUAN LY THUE BAO DI DONG (KTLT - PTIT)
echo ====================================================================
echo Dang kiem tra trinh bien dich g++...

where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo [LOI] Khong tim thay trinh bien dich g++ tren may cua ban!
    echo Vui long cai dat MinGW-w64 hoac chay bang Dev-C++ / Code::Blocks / Visual Studio.
    echo Huong dan chi tiet co tai: docs/UserGuide/USER_MANUAL.md
    pause
    exit /b 1
)

echo Dang bien dich ma nguon...
g++ -std=c++11 -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus -Isrc/lib/stubs src/main.cpp src/lib/shared/Date.cpp src/lib/shared/InputHelper.cpp src/lib/shared/fileio.cpp src/lib/models/hopdong.cpp src/lib/models/imei.cpp src/lib/menus/HopDongMenu.cpp src/lib/menus/ThietBiIMEIMenu.cpp -o quanlythuebao.exe

if %errorlevel% neq 0 (
    echo [LOI] Bien dich that bai!
    pause
    exit /b 1
)

echo [THANH CONG] Bien dich hoan tat. Dang khoi chay chuong trinh...
echo.
quanlythuebao.exe

pause
