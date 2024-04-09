### json_watcher_cpp build isntruction

1. Build thrid party dependecies, as desribed in [3rdp/README.md](3rdp/README.md)

2. InstallCZMQ library with headers if needed, e.g. on Ubuntu:
`sudo apt install libczmq4 libczmq-dev`

3. Generate build files cmake, e.g:
`cd build && cmake ../`

4. Build desired target, e.g with make or ninja:
```
make -j4 serverApp
or
ninja serverApp
...
```
5. Sample server usage:
`./build/server_src/serverApp -d -f sample_data/access_points.json`