# SET LIBNAME
SET( MENGINE_LIB_PREFIX "lib" )
SET( MENGINE_LIB_SUFFIX ".a" )

set(CMAKE_C_FLAGS_RELEASE "-static -fno-strict-aliasing -fomit-frame-pointer -O2 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0 -D_TIMESPEC_DEFINED -D__USE_MINGW_FSEEK")
set(CMAKE_CXX_FLAGS_RELEASE "-static -std=c++11 -fno-strict-aliasing -fomit-frame-pointer -O2 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0 -D_TIMESPEC_DEFINED -D__USE_MINGW_FSEEK")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")

add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)

