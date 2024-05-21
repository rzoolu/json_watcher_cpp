### json_watcher_cpp build instruction

Requires C++20 compatible compiler and standard library.

1. `git clone & git submodule update --init --recursive`
2. Build thrid party dependecies, as desribed in [3rdp/README.md](3rdp/README.md)

3. InstallCZMQ library with headers if needed, e.g. on Ubuntu:
`sudo apt install libczmq4 libczmq-dev`

4. Generate build files cmake, e.g:
`cd build && cmake ../`

5. Build desired target, e.g with make or ninja:
```
make -j4 serverApp
or
ninja serverApp
...
```
6. Sample server usage:
`./build/server_src/serverApp -d -f sample_data/access_points.json`
