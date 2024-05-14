
### Current 3rdp submodules revisions:
```
protobuf:
tags/v25.3

googletest:
tags/v1.14.0

nlohmann/json:
tags/v3.11.3

cppzmq:
tags/v4.10.0

ftxui:
tag/v4.1.1
```

Initialize with:
`git submodule update --init --recursive`
in main repository after fresh clone.


### 3rdp build instructions:
1. protobuf:
    ```
    cd 3rdp/protobuf
    mkdir build
    cmake . -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=./build -Dprotobuf_BUILD_TESTS=OFF
    cmake --build . --config Release --target install
    ```
    Use binaries and headers from 3rdp/protubuf/build.

2. googletest & gmock:
    ```
    cd 3rdp/googletest
    mkdir build && cd build
    cmake .. -DCMAKE_CXX_STANDARD=20 -DCMAKE_INSTALL_PREFIX=./
    cmake --build . --config Release --target install
    ```
    Use binaries and headers from 3rdp/googletest/build.

3. cppzmq: header only library embedded to main CMake project with add_subdirectory()

4. json: headers only library embedded to main CMake project with add_subdirectory()

5. ftxui:
    ```
    cd 3rdp/ftxui
    mkdir build && cd build
    cmake .. -DFTXUI_BUILD_EXAMPLES=OFF -DFTXUI_BUILD_DOCS=OFF -DCMAKE_INSTALL_PREFIX=./
    cmake --build . --config Release --target install
    ```