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
                   tests/modules/test_menu_parity.cpp \
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

monitor: monitor-stress

monitor-project:
	@if test -n "$(PID)"; then \
	  test -r "/proc/$(PID)/status" || { echo "Cannot read /proc/$(PID)/status"; exit 2; }; \
	  watch -n 1 -d "grep -E '^(Name|State|VmPeak|VmSize|VmHWM|VmRSS|VmSwap):' /proc/$(PID)/status"; \
	else \
	  while :; do \
	    clear; printf 'Processes associated with %s (cwd or executable)\n' "$(CURDIR)"; \
	    printf '%-8s %-24s %-12s %-12s %s\n' PID COMM RSS_KB SWAP_KB MATCH; \
	    for proc in /proc/[0-9]*; do \
	      test -r "$$proc/status" || continue; \
	      cwd=$$(readlink "$$proc/cwd" 2>/dev/null) || cwd=; \
	      exe=$$(readlink "$$proc/exe" 2>/dev/null) || exe=; \
	      match=; \
	      case "$$cwd/" in "$(CURDIR)/"*) match=cwd;; esac; \
	      case "$$exe" in "$(CURDIR)/"*) match=$${match:+$$match,}exe;; esac; \
	      test -n "$$match" || continue; \
	      pid=$${proc##*/}; \
	      comm=$$(cat "$$proc/comm" 2>/dev/null) || comm=?; \
	      awk -v pid="$$pid" -v comm="$$comm" -v matched_by="$$match" 'BEGIN { rss="?"; swap="?" } /^VmRSS:/ { rss=$$2 } /^VmSwap:/ { swap=$$2 } END { printf "%-8s %-24s %-12s %-12s %s\n", pid, comm, rss, swap, matched_by }' "$$proc/status"; \
	    done; \
	    sleep 1; \
	  done; \
	fi

monitor-system:
	watch -n 1 -d "awk '/^(MemTotal|MemAvailable|SwapTotal|SwapFree):/ { print }' /proc/meminfo"

monitor-stress: $(STRESS_TARGET)
	@log="/dev/shm/ktlt-stress-monitor.$$$$.log"; \
	./$(STRESS_TARGET) --duration $${DURATION:-30} > "$$log" 2>&1 & pid=$$!; \
	trap 'if test -n "$$pid"; then kill "$$pid" 2>/dev/null || true; wait "$$pid" 2>/dev/null || true; fi; cat "$$log"; rm -f "$$log"' EXIT; \
	trap 'exit 130' INT TERM; \
	while kill -0 "$$pid" 2>/dev/null; do \
	  if test -r "/proc/$$pid/status"; then \
	    printf '\n--- stress runner PID %s ---\n' "$$pid"; \
	    grep -E '^(Name|State|VmPeak|VmSize|VmHWM|VmRSS|VmSwap):' "/proc/$$pid/status"; \
	  fi; \
	  sleep 1; \
	done; \
	wait "$$pid"; result=$$?; pid=; \
	cat "$$log"; rm -f "$$log"; trap - EXIT INT TERM; exit "$$result"

stress: $(STRESS_TARGET)
	./$(STRESS_TARGET) --duration 300

$(STRESS_TARGET): $(STRESS_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) -fsanitize=address,undefined -O2 $^ -o $@

clean:
	rm -f -- $(TARGET) $(TEST_TARGET) $(STRESS_TARGET) ./*.o *.gcov *.gcda *.gcno

run: $(TARGET)
	./$(TARGET)

.PHONY: all test clean run stress monitor monitor-project monitor-system monitor-stress
