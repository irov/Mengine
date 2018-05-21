# SET LIBNAME
SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")
set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS_DEBUG "-static -fno-strict-aliasing -fomit-frame-pointer -O0 -D__USE_MINGW_FSEEK -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-static -fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O0 -D__USE_MINGW_FSEEK -D_DEBUG")

set(CMAKE_C_FLAGS_RELEASE "-static -fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-static -fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK -DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")