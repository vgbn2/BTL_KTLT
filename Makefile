CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Isrc -Isrc/lib

TARGET = quanlythuebao
TEST_TARGET = test_runner

LIB_SRCS = src/lib/Date.cpp \
           src/lib/InputHelper.cpp \
           src/lib/fileio.cpp \
           src/lib/hopdong.cpp \
           src/lib/imei.cpp \
           src/lib/HopDongMenu.cpp \
           src/lib/ThietBiIMEIMenu.cpp

MAIN_SRC = src/main.cpp
TEST_SRC = tests/test_runner.cpp

all: $(TARGET)

$(TARGET): $(MAIN_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) src/lib/Date.cpp src/lib/InputHelper.cpp src/lib/fileio.cpp src/lib/hopdong.cpp src/lib/imei.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run
