mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="%EMSCRIPTEN%/cmake/Modules/Platform/emscripten.cmake" -G"Unix Makefiles" "../../../cmake/Depends_EMSCRIPTEN"
make
cd ..