SET(ANDROID_SYSROOT ${ANDROID_NDK}/sysroot)

MESSAGE("ANDROID_PLATFORM: ${ANDROID_PLATFORM}")
MESSAGE("ANDROID_ARM_NEON: ${ANDROID_ARM_NEON}")
MESSAGE("ANDROID_SYSROOT: ${ANDROID_SYSROOT}")
MESSAGE("ANDROID_ABI: ${ANDROID_ABI}")
MESSAGE("ANDROID_STL: ${ANDROID_STL}")
MESSAGE("ANDROID_TOOLCHAIN: ${ANDROID_TOOLCHAIN}")

MESSAGE("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

SET(CMAKE_SYSTEM_NAME Android)

SET(MENGINE_LIB_PREFIX "lib" CACHE STRING "MENGINE_LIB_PREFIX" FORCE)
SET(MENGINE_LIB_SUFFIX ".a" CACHE STRING "MENGINE_LIB_SUFFIX" FORCE)
SET(MENGINE_SO_PREFIX "lib" CACHE STRING "MENGINE_SO_PREFIX" FORCE)
SET(MENGINE_SO_SUFFIX ".so" CACHE STRING "MENGINE_SO_SUFFIX" FORCE)

SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set debug library postfix" FORCE)

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11" )
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -frtti -fexceptions -Wno-register" )

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

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0" )
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O3 -DNDEBUG" )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0" )
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -DNDEBUG" )

MESSAGE("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
MESSAGE("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

MESSAGE("CMAKE_C_FLAGS_DEBUG: ${CMAKE_C_FLAGS_DEBUG}")
MESSAGE("CMAKE_C_FLAGS_RELEASE: ${CMAKE_C_FLAGS_RELEASE}")

MESSAGE("CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
MESSAGE("CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")