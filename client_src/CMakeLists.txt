set(FTXUI_INSTALL_CMAKE_CFG_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../3rdp/ftxui/build/lib/cmake/ftxui")

find_package(ftxui 4.1 REQUIRED PATHS ${FTXUI_INSTALL_CMAKE_CFG_DIR})

if (ftxui_FOUND)
    message(STATUS "ftxui package found, version: ${ftxui_VERSION}")
endif()

add_executable(clientApp)
target_sources(clientApp PRIVATE
    "ApWatchIMsgHandler.cpp"
    "ClientApp.cpp"
    "ClientMain.cpp"
    "ClientUI.cpp"
)

target_include_directories(clientApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(clientApp PRIVATE common_compile_options)

target_link_libraries(clientApp PRIVATE common_log_interface_lib)
target_link_libraries(clientApp PRIVATE common_msg_obj_lib)

target_link_libraries(clientApp PRIVATE ftxui::screen ftxui::dom)
