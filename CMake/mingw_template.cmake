# SET LIBNAME
SET( MENGINE_LIB_PREFIX "lib" )
SET( MENGINE_LIB_SUFFIX ".a" )

set(CMAKE_C_FLAGS_RELEASE "-static -fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK")
set(CMAKE_CXX_FLAGS_RELEASE "-static -std=c++11 -fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK")

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")