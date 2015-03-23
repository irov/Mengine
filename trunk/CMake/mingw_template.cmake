# SET LIBNAME
SET( MENGINE_LIB_PREFIX "lib" )
SET( MENGINE_LIB_SUFFIX ".a" )

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -fno-strict-aliasing -fomit-frame-pointer -march=i686 -O3 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -static -static-libgcc -static-libstdc++ -fno-strict-aliasing -fomit-frame-pointer -march=i686 -O3 -D_SECURE_SCL=0 -D_HAS_ITERATOR_DEBUGGING=0")

add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)




