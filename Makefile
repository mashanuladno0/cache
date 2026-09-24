CXX      = g++
CXXFLAGS = -std=c++17 -I include

MAIN_SRC = src/main.cpp src/Config.cpp
MAIN_EXE = cache_sim

TEST_SRC = tests/test_main.cpp \
           tests/test_lfu.cpp \
           tests/test_arc.cpp \
           tests/test_2q.cpp \
           tests/test_lirs.cpp \
           tests/test_ideal.cpp \
           tests/test_multilevel.cpp \
           tests/test_config.cpp
TEST_EXE = tests_runner

all: $(MAIN_EXE)

$(MAIN_EXE): $(MAIN_SRC)
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN_EXE)

$(TEST_EXE): $(TEST_SRC) src/Config.cpp
	$(CXX) $(CXXFLAGS) $(TEST_SRC) src/Config.cpp -o $(TEST_EXE)

test: $(TEST_EXE)
	./$(TEST_EXE)

run: $(MAIN_EXE)
	./$(MAIN_EXE) config.txt input.txt

clean:
	rm -f $(MAIN_EXE) $(TEST_EXE)

.PHONY: all test run clean