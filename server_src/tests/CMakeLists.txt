set(GOOGLETEST_INSTALL_CMAKE_CFG_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../../3rdp/googletest/build/lib/cmake/GTest")

find_package(GTest REQUIRED PATHS ${GOOGLETEST_INSTALL_CMAKE_CFG_DIR})

if (GTest_FOUND)
    message(STATUS "GTest package found, version: ${GTest_VERSION}")
endif()


enable_testing()
include(GoogleTest)

set(SERVER_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")


add_executable(serverApp_tests)
# produciton sources
target_sources(serverApp_tests PRIVATE
    "${SERVER_SRC_DIR}/AccessPointsData.cpp"
    "${SERVER_SRC_DIR}/ChangeToProtoMsgConverter.cpp"
    "${SERVER_SRC_DIR}/FileMonitor.cpp"
    "${SERVER_SRC_DIR}/JsonParser.cpp"
    "${SERVER_SRC_DIR}/ServerApp.cpp"
 )
 # UT sources
target_sources(serverApp_tests PRIVATE
    "AccessPointsDataTest.cpp"
    "FileMonitorTest.cpp"
    "JsonParserTest.cpp"
    "Main.cpp"
    "ServerAppE2ETest.cpp"
    "ServerAppTest.cpp"
)

# Copy UT test data file to UT binary directory
configure_file("${SERVER_SRC_DIR}/../sample_data/access_points.json"
    "${CMAKE_CURRENT_BINARY_DIR}/access_points.json" COPYONLY)
configure_file("${SERVER_SRC_DIR}/../sample_data/e2e_test_1AP.json"
    "${CMAKE_CURRENT_BINARY_DIR}/e2e_test_1AP.json" COPYONLY)
configure_file("${SERVER_SRC_DIR}/../sample_data/e2e_test_1APmodified_ch.json"
    "${CMAKE_CURRENT_BINARY_DIR}/e2e_test_1APmodified_ch.json" COPYONLY)
configure_file("${SERVER_SRC_DIR}/../sample_data/e2e_test_2APs.json"
    "${CMAKE_CURRENT_BINARY_DIR}/e2e_test_2APs.json" COPYONLY)
configure_file("${SERVER_SRC_DIR}/../sample_data/e2e_test_3APs.json"
    "${CMAKE_CURRENT_BINARY_DIR}/e2e_test_3APs.json" COPYONLY)


gtest_discover_tests(serverApp_tests)

target_include_directories(serverApp_tests PRIVATE "${SERVER_SRC_DIR}")

target_link_libraries(serverApp_tests PRIVATE GTest::gtest)
target_link_libraries(serverApp_tests PRIVATE GTest::gmock)

target_link_libraries(serverApp_tests PRIVATE common_compile_options)
target_link_libraries(serverApp_tests PRIVATE common_log_interface_lib)
target_link_libraries(serverApp_tests PRIVATE common_msg_obj_lib)
target_link_libraries(serverApp_tests PRIVATE nlohmann_json::nlohmann_json)
