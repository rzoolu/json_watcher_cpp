# nlohmann_json: embedded, header only library
# use with: target_link_libraries(foo_target PRIVATE nlohmann_json::nlohmann_json)
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)

# cppzmq: embedded, header only library
# use with: target_link_libraries(foo_target PRIVATE cppzmq)
set(CPPZMQ_BUILD_TESTS OFF CACHE INTERNAL "")
set(CMAKE_SKIP_INSTALL_RULES ON CACHE INTERNAL "")
add_subdirectory(cppzmq)
