CXX = g++
CXXFLAGS = -std=c++11 -g -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus -Itests/modules

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

TEST_MODULE_SRCS = tests/modules/test_shared.cpp \
                   tests/modules/test_infrastructure.cpp \
                   tests/modules/test_models.cpp \
                   tests/modules/test_menus.cpp \
                   tests/modules/test_fixtures.cpp

MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_runner.cpp
STRESS_SRC = tests/stress_test_runner.cpp

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(TEST_MODULE_SRCS) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

trace-stream: $(TEST_TARGET)
	gdb -batch \
	  -ex "file ./$(TEST_TARGET)" \
	  -ex "break FileIO::getTempStagingPath" \
	  -ex "break FileIO::commitFromTmp" \
	  -ex "run" \
	  -ex "print targetFilePath" \
	  -ex "continue" \
	  -ex "print tmpPath" \
	  -ex "print targetPath" \
	  -ex "quit"

stress: $(STRESS_TARGET)
	./$(STRESS_TARGET) --duration 300

$(STRESS_TARGET): $(STRESS_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) -fsanitize=address,undefined -O2 $^ -o $@

clean:
	rm -f -- $(TARGET) $(TEST_TARGET) $(STRESS_TARGET) ./*.o *.gcov *.gcda *.gcno

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run stress
