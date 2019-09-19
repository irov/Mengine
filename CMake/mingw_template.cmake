# SET LIBNAME
SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")
set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -posix")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -posix")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -fno-strict-aliasing -fomit-frame-pointer -O0 -D_DEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-strict-aliasing -fomit-frame-pointer -O3 -DNDEBUG")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O0 -D_DEBUG -UMBCS -D_UNICODE -DUNICODE")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O3 -DNDEBUG -UMBCS -D_UNICODE -DUNICODE")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")