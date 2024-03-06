CXX = g++ # or run 'make CXX=clang++' for clang

ifeq ($(CXX),clang++)
CXXFLAGS += -stdlib=libc++
LDFLAGS += -stdlib=libc++
endif

CXXFLAGS += --std=c++20 -Wall -Wextra -ggdb

SRC_DIR = server_src
3RDP_DIR = 3rdp
COMMON_DIR = common

NLOHMANN_INCLUDE = $(3RDP_DIR)/json/single_include
CPPZMQ_INCLUDE = $(3RDP_DIR)/cppzmq

PROTOBUF_INSTALL = $(3RDP_DIR)/protobuf/build
PROTOC = $(PROTOBUF_INSTALL)/bin/protoc
PROTOBUF_INCLUDE = $(PROTOBUF_INSTALL)/include
PROTOBUF_ABSEIL_DEPS = \
  -labsl_str_format_internal       \
  -labsl_strings                   \
  -labsl_strings_internal          \
  -labsl_log_initialize            \
  -labsl_log_entry                 \
  -labsl_log_flags                 \
  -labsl_log_severity              \
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


MSG_IF_PROTO = $(COMMON_DIR)/message_interface/proto
MSG_IF_GEN_SRC = $(COMMON_DIR)/message_interface/cpp_gen

IDIRS = -I$(SRC_DIR) -I$(COMMON_DIR) -I$(NLOHMANN_INCLUDE) -I$(CPPZMQ_INCLUDE) -I$(PROTOBUF_INCLUDE) -I$(MSG_IF_GEN_SRC)
CXXFLAGS += $(IDIRS)

LDIRS = -L$(PROTOBUF_INSTALL)/lib
LIBS = -lzmq -lprotobuf $(PROTOBUF_ABSEIL_DEPS)
LDFLAGS += $(LDIRS) $(LIBS)

BUILD_DIR = build

default: server
all: server
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

# server
HEADERS = $(wildcard $(SRC_DIR)/*.h) \
		  $(wildcard $(COMMON_DIR)/*.h)

SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

server: $(PROTOBUF_OBJS) $(SERVER_OBJS)
	$(CXX) -o $@ $^ $(CXXLAGS) $(LDFLAGS)

xsan_server: CXXLAGS += -fsanitize=address,undefined # -O1 -fno-omit-frame-pointer
xsan_server: server

clean_build:
	rm -f $(BUILD_DIR)/*.o server

clean: clean_gen_proto clean_build
