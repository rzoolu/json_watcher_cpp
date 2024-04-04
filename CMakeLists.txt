cmake_minimum_required(VERSION 3.21)

project(json_watcher_cpp LANGUAGES CXX C)

add_library(common_compile_options INTERFACE)
target_compile_features(common_compile_options INTERFACE cxx_std_20)
target_compile_options(common_compile_options INTERFACE -Wall -Wextra)

add_subdirectory(3rdp)
add_subdirectory(common)
add_subdirectory(client_src)
add_subdirectory(server_src)
