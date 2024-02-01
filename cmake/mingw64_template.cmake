SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")
set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -femulated-tls -fuse-ld=lld -DWIN32 -D_WIN64 -m64")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -femulated-tls -fuse-ld=lld -DWIN32 -D_WIN64 -m64")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fno-strict-aliasing -fomit-frame-pointer -std=c11 -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-strict-aliasing -fomit-frame-pointer -std=c11 -O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fno-strict-aliasing -fomit-frame-pointer -std=c++17 -O0 -UMBCS -D_UNICODE -DUNICODE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-strict-aliasing -fomit-frame-pointer -std=c++17 -O3 -DNDEBUG -UMBCS -D_UNICODE -DUNICODE")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
