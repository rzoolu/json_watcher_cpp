set(GOOGLETEST_INSTALL_CMAKE_CFG_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdp/googletest/build/lib/cmake/GTest")

find_package(GTest REQUIRED PATHS ${GOOGLETEST_INSTALL_CMAKE_CFG_DIR})

if (GTest_FOUND)
    message(STATUS "GTest package found, version: ${GTest_VERSION}")
else()
    message(SEND_ERROR "GTest package not found.")
endif()


enable_testing()
include(GoogleTest)

set(COMMON_MSG_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")


add_executable(common_msg_tests)

 # UT sources
target_sources(common_msg_tests PRIVATE
    "Main.cpp"
    "MessagePublisherTest.cpp"
)


gtest_discover_tests(common_msg_tests)

target_include_directories(common_msg_tests PRIVATE "${COMMON_MSG_SRC_DIR}")

target_link_libraries(common_msg_tests PRIVATE GTest::gtest)
target_link_libraries(common_msg_tests PRIVATE GTest::gmock)

target_link_libraries(common_msg_tests PRIVATE common_compile_options)
target_link_libraries(common_msg_tests PRIVATE common_log_interface_lib)
target_link_libraries(common_msg_tests PRIVATE common_msg_obj_lib)
