
add_executable(clientApp)
target_sources(clientApp PRIVATE
    "ApWatchIMsgHandler.cpp"
    "ClientApp.cpp"
    "ClientMain.cpp"
    "MessageSubscriber.cpp"
)

target_include_directories(clientApp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(clientApp PRIVATE common_compile_options)

target_link_libraries(clientApp PRIVATE common_obj_lib)
