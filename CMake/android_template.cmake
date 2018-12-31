SET(ANDROID_SYSROOT ${ANDROID_NDK}/sysroot)

SET(MENGINE_LIB_PREFIX "lib" CACHE STRING "MENGINE_LIB_PREFIX" FORCE)
SET(MENGINE_LIB_SUFFIX ".a" CACHE STRING "MENGINE_LIB_SUFFIX" FORCE) 
SET(MENGINE_SO_SUFFIX ".so" CACHE STRING "MENGINE_SO_SUFFIX" FORCE)

SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set debug library postfix" FORCE)

SET(CMAKE_C_FLAGS "-std=c11" )
SET(CMAKE_CXX_FLAGS "-std=c++17 -frtti -fexceptions -Wno-register" )

if (${ANDROID_ABI} STREQUAL "x86_64")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/x86_64-linux-android")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/x86_64-linux-android")
elseif (${ANDROID_ABI} STREQUAL "x86")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/i686-linux-android")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/i686-linux-android")
elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/aarch64-linux-android")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/aarch64-linux-android")
elseif (${ANDROID_ABI} STREQUAL "armeabi-v7a")    
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/arm-linux-androideabi")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isystem ${ANDROID_SYSROOT}/usr/include/arm-linux-androideabi")
endif()

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -D_DEBUG" )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_DEBUG} -O3 -DNDEBUG" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -D_DEBUG" )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O3 -DNDEBUG" )