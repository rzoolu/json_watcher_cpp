
add_executable(clientApp)
target_sources(clientApp PRIVATE
    "ApWatchIMsgHandler.cpp"
    "ClientApp.cpp"
    "ClientMain.cpp"
)

target_include_directories(clientApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(clientApp PRIVATE common_compile_options)

target_link_libraries(clientApp PRIVATE common_log_interface_lib)
target_link_libraries(clientApp PRIVATE common_msg_obj_lib)
