CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Isrc -Isrc/lib -Isrc/lib/shared -Isrc/lib/models -Isrc/lib/menus -Isrc/lib/stubs

TARGET = quanlythuebao
TEST_TARGET = test_runner

LIB_SRCS = src/lib/shared/Date.cpp \
           src/lib/shared/InputHelper.cpp \
           src/lib/shared/Normalized.cpp \
           src/lib/shared/fileio.cpp \
           src/lib/models/hopdong.cpp \
           src/lib/models/imei.cpp \
           src/lib/menus/HopDongMenu.cpp \
           src/lib/menus/ThietBiIMEIMenu.cpp

MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_runner.cpp

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run
