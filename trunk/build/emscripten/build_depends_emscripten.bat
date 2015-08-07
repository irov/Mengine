mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE="%EMSCRIPTEN%/cmake/Modules/Platform/emscripten.cmake" -G"Unix Makefiles" "../../../CMake/Depends_EMSCRIPTEN"
make
cd ..