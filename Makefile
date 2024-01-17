CXX = g++ #clang++
CXXFLAGS = --std=c++20 -Wall -Wextra -ggdb  #-stdlib=libc++

SRC_DIR = server_src
3RDP_DIR = 3rdp
COMMON_DIR = common

IDIRS = -I$(SRC_DIR) -I$(3RDP_DIR) -I$(COMMON_DIR)
CXXFLAGS += $(IDIRS)

BUILD_DIR = build

default: server
all: server
.PHONY: clean

# server
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(COMMON_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

server: $(OBJS)
	$(CXX) -o $@ $^ $(CXXLAGS) $(LIBS) #-stdlib=libc++

xsan_server: CXXLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan_server: server

clean:
	rm -f $(BUILD_DIR)/*.o server
