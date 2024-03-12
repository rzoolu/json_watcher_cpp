MAKEFLAGS+="-j $(shell nproc)"

CXX = g++ # or run 'make CXX=clang++' for clang
LD = $(CXX)

ifeq ($(CXX),clang++)
CXXFLAGS += -stdlib=libc++
LDFLAGS += -stdlib=libc++
endif

CXXFLAGS += --std=c++20 -Wall -Wextra -ggdb

SRC_DIR = ..
TEST_DIR = .
MOCKS_DIR = $(TEST_DIR)/mocks

ROOT_DIR = ../..
3RDP_DIR = $(ROOT_DIR)/3rdp
COMMON_DIR = $(ROOT_DIR)/common

GTEST_INCLUDE = $(3RDP_DIR)/googletest/googletest/include
GTEST_LIB = $(3RDP_DIR)/googletest/build/lib

GMOCK_INCLUDE = $(3RDP_DIR)/googletest/googlemock/include
NLOHMANN_INCLUDE = $(3RDP_DIR)/json/single_include
CPPZMQ_INCLUDE = $(3RDP_DIR)/cppzmq

PROTOBUF_INSTALL = $(3RDP_DIR)/protobuf/build
PROTOC = $(PROTOBUF_INSTALL)/bin/protoc
PROTOBUF_INCLUDE = $(PROTOBUF_INSTALL)/include
PROTOBUF_ABSEIL_DEPS = \
  -labsl_log_internal_check_op     \
  -labsl_log_internal_conditions   \
  -labsl_log_internal_message      \
  -labsl_log_internal_nullguard    \
  -labsl_log_internal_proto        \
  -labsl_log_internal_format       \
  -labsl_log_internal_globals      \
  -labsl_log_internal_log_sink_set \
  -labsl_log_sink                  \
  -labsl_raw_logging_internal      \
  -labsl_log_globals               \
  -lutf8_validity                  \
  -labsl_cord                      \
  -labsl_cordz_info                \
  -labsl_cordz_handle              \
  -labsl_cordz_functions           \
  -labsl_cord_internal             \
  -labsl_crc_cord_state            \
  -labsl_crc32c                    \
  -labsl_crc_internal              \
  -labsl_exponential_biased        \
  -labsl_synchronization           \
  -labsl_graphcycles_internal      \
  -labsl_kernel_timeout_internal   \
  -labsl_time                      \
  -labsl_time_zone                 \
  -labsl_int128                    \
  -labsl_examine_stack             \
  -labsl_stacktrace                \
  -labsl_symbolize                 \
  -labsl_demangle_internal         \
  -labsl_debugging_internal        \
  -labsl_malloc_internal           \
  -labsl_throw_delegate            \
  -labsl_strerror                  \
  -labsl_raw_hash_set              \
  -labsl_hash                      \
  -labsl_city                      \
  -labsl_low_level_hash            \
  -labsl_base                      \
  -labsl_spinlock_wait             \
  -labsl_status                    \
  -labsl_statusor                  \
  -labsl_str_format_internal       \
  -labsl_strings                   \
  -labsl_strings_internal          \

MSG_IF_PROTO = $(COMMON_DIR)/message_interface/proto
MSG_IF_GEN_SRC = $(COMMON_DIR)/message_interface/cpp_gen

IDIRS = -I$(SRC_DIR) -I$(COMMON_DIR) -I$(GTEST_INCLUDE) \
		-I$(GMOCK_INCLUDE) -I$(NLOHMANN_INCLUDE) -I$(CPPZMQ_INCLUDE) \
		-I$(PROTOBUF_INCLUDE) -I$(MSG_IF_GEN_SRC)

CXXFLAGS += $(IDIRS)

LDIRS = -L$(GTEST_LIB) -L$(PROTOBUF_INSTALL)/lib
TEST_LIBS = -lgtest -lgmock -lgtest
LIBS = -lzmq -lprotobuf-lite $(PROTOBUF_ABSEIL_DEPS)
LIBS += $(TEST_LIBS)

LDFLAGS += $(LDIRS) $(LIBS)

BUILD_DIR = ./build

default: tests
all: tests
.PHONY: clean

# protobuf messages
$(MSG_IF_GEN_SRC)/%.pb.cc: $(MSG_IF_PROTO)/%.proto
	$(PROTOC) -I$(MSG_IF_PROTO) --cpp_out=$(MSG_IF_GEN_SRC) $<

PROTOBUF_SRCS = $(patsubst $(MSG_IF_PROTO)/%.proto, $(MSG_IF_GEN_SRC)/%.pb.cc, $(wildcard $(MSG_IF_PROTO)/*.proto))

gen_proto_src: $(PROTOBUF_SRCS)

clean_gen_proto:
	rm -f $(MSG_IF_GEN_SRC)/*

PROTOBUF_OBJS = $(patsubst $(MSG_IF_GEN_SRC)/%.pb.cc, $(BUILD_DIR)/%.pb.o, $(PROTOBUF_SRCS))

$(BUILD_DIR)/%.pb.o: $(MSG_IF_GEN_SRC)/%.pb.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

gen_proto_objs: $(PROTOBUF_OBJS)


# tests
HEADERS = $(wildcard $(SRC_DIR)/*.h) $(wildcard $(COMMON_DIR)/*.h)

OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
OBJS += $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(TEST_DIR)/*.cpp))

# remove real main to use test main
TMPOBJS := $(OBJS)
OBJS = $(filter-out $(BUILD_DIR)/ServerMain.o, $(TMPOBJS));

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS) $(PROTOBUF_SRCS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS) $(PROTOBUF_SRCS)
		$(CXX) -c -o $@ $< $(CXXFLAGS)


tests: $(PROTOBUF_OBJS) $(OBJS)
		$(LD) -o $@ $^ $(LDFLAGS)

valgrind: tests
		valgrind -q --leak-check=full --show-leak-kinds=all  ./tests


clean_build:
	rm -f $(BUILD_DIR)/*.o tests

clean: clean_gen_proto clean_build

xsan: CXXFLAGS += -fsanitize=address,undefined
xsan: LDFLAGS += -fsanitize=address,undefined
xsan: tests
		./tests
