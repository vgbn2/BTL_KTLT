CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus

TARGET = quanlythuebao
TEST_TARGET = test_runner
STRESS_TARGET = stress_test_runner

LIB_SRCS = src/lib/shared/Date.cpp \
           src/lib/shared/InputHelper.cpp \
           src/lib/shared/Normalized.cpp \
           src/lib/shared/fileio.cpp \
           src/lib/shared/BTSRegister.cpp \
           src/lib/models/hopdong.cpp \
           src/lib/models/imei.cpp \
           src/lib/models/khachhang.cpp \
           src/lib/models/goicuoc.cpp \
           src/lib/models/thuebao.cpp \
           src/lib/models/hoadon.cpp \
           src/lib/models/naptien.cpp \
           src/lib/models/ghicuocgoi.cpp \
           src/lib/models/khieunai.cpp \
           src/lib/menus/HopDongMenu.cpp \
           src/lib/menus/ThietBiIMEIMenu.cpp \
           src/lib/menus/GoiCuocMenu.cpp \
           src/lib/menus/ThueBaoMenu.cpp \
           src/lib/menus/KhachHangMenu.cpp \
           src/lib/menus/NapTienMenu.cpp \
           src/lib/menus/HoaDonMenu.cpp

MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_runner.cpp
STRESS_SRC = tests/stress_test_runner.cpp

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

stress: $(STRESS_TARGET)
	./$(STRESS_TARGET) --duration 300

$(STRESS_TARGET): $(STRESS_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) -fsanitize=address,undefined -O2 $^ -o $@

clean:
	rm -f -- $(TARGET) $(TEST_TARGET) $(STRESS_TARGET) ./*.o *.gcov *.gcda *.gcno

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run stress
