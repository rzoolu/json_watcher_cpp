
add_executable(serverApp)
target_sources(serverApp PRIVATE
    "AccessPointsData.cpp"
    "ChangeToProtoMsgConverter.cpp"
    "FileMonitor.cpp"
    "JsonParser.cpp"
    "ServerApp.cpp"
    "ServerMain.cpp"
 )

target_include_directories(serverApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(serverApp PRIVATE common_compile_options)

target_link_libraries(serverApp PRIVATE common_log_interface_lib)
target_link_libraries(serverApp PRIVATE common_msg_obj_lib)
target_link_libraries(serverApp PRIVATE nlohmann_json::nlohmann_json)

# UT/googletest binary for server_src
add_subdirectory(tests)
