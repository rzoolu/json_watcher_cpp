CXX = g++
CXXFLAGS = -D DEBUG --std=c++17 -Wall -Wextra -ggdb 

SRC_DIR = server_src
3RDP_DIR = 3rdp

IDIRS = -I$(SRC_DIR) -I$(3RDP_DIR)
CXXFLAGS += $(IDIRS)

BUILD_DIR = build

default: server
all: server
.PHONY: clean

# server
HEADERS = $(wildcard $(SRC_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CFLAGS)

server: $(OBJS)
	$(CXX) -o $@ $^ $(CXXLAGS) $(LIBS)

xsan_server: CXXLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan_server: server

clean:
	rm -f $(BUILD_DIR)/*.o server
