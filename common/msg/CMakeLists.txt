
set(PROTOBUF_INSTALL_CMAKE_CFG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdp/protobuf/build/lib/cmake")

find_package(absl REQUIRED PATHS ${PROTOBUF_INSTALL_CMAKE_CFG_DIR})
find_package(utf8_range REQUIRED PATHS ${PROTOBUF_INSTALL_CMAKE_CFG_DIR})
find_package(Protobuf REQUIRED PATHS ${PROTOBUF_INSTALL_CMAKE_CFG_DIR})

if (Protobuf_FOUND)
    message(STATUS "Protobuf package found, version: ${Protobuf_VERSION}")
endif()


add_library(common_msg_obj_lib OBJECT)
target_sources(common_msg_obj_lib
    PRIVATE "src/MessagePublisher.cpp"
            "src/MessageSendReceive.cpp"
            "src/MessageSubscriber.cpp"
            "src/ProtoBufGuard.cpp"
 )

set(PROTOC_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto_gen")

protobuf_generate(LANGUAGE cpp
    TARGET common_msg_obj_lib
    PROTOC_OUT_DIR "${PROTOC_OUTPUT_DIR}"
    IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/proto"
    PROTOS "${CMAKE_CURRENT_SOURCE_DIR}/proto/ApWatchI.proto")

target_include_directories(common_msg_obj_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_include_directories(common_msg_obj_lib PUBLIC "${PROTOC_OUTPUT_DIR}")

target_link_libraries(common_msg_obj_lib PRIVATE common_compile_options)

target_link_libraries(common_msg_obj_lib PRIVATE common_log_interface_lib)
target_link_libraries(common_msg_obj_lib PRIVATE cppzmq)
target_link_libraries(common_msg_obj_lib PUBLIC protobuf::libprotobuf-lite)

# UT/googletest binary for common_msg_obj_lib
add_subdirectory(tests)
