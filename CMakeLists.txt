cmake_minimum_required(VERSION 3.21)

project(json_watcher_cpp LANGUAGES CXX C)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
      FORCE)
endif()

add_library(common_compile_options INTERFACE)

option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)

if(ENABLE_SANITIZER_ADDRESS)
    target_compile_options(common_compile_options INTERFACE -fsanitize=address)
    target_link_options(common_compile_options INTERFACE -fsanitize=address)
endif()

target_compile_features(common_compile_options INTERFACE cxx_std_20)
target_compile_options(common_compile_options INTERFACE -Wall -Wextra)

add_subdirectory(3rdp)
add_subdirectory(common)
add_subdirectory(client_src)
add_subdirectory(server_src)
