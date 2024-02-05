CXX = g++
CXXFLAGS = --std=c++20 -Wall -Wextra -ggdb

SRC_DIR = ..
TEST_DIR = .

ROOT_DIR = ../..
3RDP_DIR = $(ROOT_DIR)/3rdp
COMMON_DIR = $(ROOT_DIR)/common

GTEST_INCLUDE = $(3RDP_DIR)/googletest/googletest/include
GTEST_LIB = $(3RDP_DIR)/googletest/build/lib

GMOCK_INCLUDE = $(3RDP_DIR)/googletest/googlemock/include
NLOHMANN_INCLUDE = $(3RDP_DIR)/json/single_include

LDIR = -L$(GTEST_LIB)

IDIRS = -I$(SRC_DIR) -I$(COMMON_DIR) \
		-I$(GTEST_INCLUDE) -I$(GMOCK_INCLUDE) -I$(NLOHMANN_INCLUDE)

CXXFLAGS += $(IDIRS)

LIBS = -lgtest -lgmock -lgtest_main

BUILD_DIR = .

default: tests
all: tests
.PHONY: clean

HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(COMMON_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
OBJS += $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(TEST_DIR)/*.cpp))

# remove real main to use test main
TMPOBJS := $(OBJS)
OBJS = $(filter-out $(BUILD_DIR)/ServerMain.o, $(TMPOBJS));

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

tests: $(OBJS)
		$(CXX) -o $@ $^ $(CXXFLAGS) $(LDIR) $(LIBS)

valgrind: tests
		valgrind -q --leak-check=full --show-leak-kinds=all  ./tests

xsan: CXXFLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan: clean tests
	./tests && rm -f $(BUILD_DIR)/*.o tests

clean:
	rm -f $(BUILD_DIR)/*.o tests