# SET LIBNAME
SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")
set(CMAKE_DEBUG_POSTFIX "")

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread")

set(CMAKE_C_FLAGS_DEBUG "-fno-strict-aliasing -fomit-frame-pointer -O0 -D__USE_MINGW_FSEEK -D_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O0 -D__USE_MINGW_FSEEK -D_DEBUG")

set(CMAKE_C_FLAGS_RELEASE "-fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-fext-numeric-literals -fno-strict-aliasing -fomit-frame-pointer -O2 -D__USE_MINGW_FSEEK -DNDEBUG")

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread")