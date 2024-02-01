if (POLICY CMP0025)
  cmake_policy(SET CMP0025 NEW)
endif ()

SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "Set debug library postfix" FORCE)

set(LLVM_ENABLE_CXX1Y ON CACHE BOOL "" FORCE)

SET(XCODE_EMIT_EFFECTIVE_PLATFORM_NAME OFF)

set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "CMAKE_OSX_ARCHITECTURES" FORCE)
set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0" CACHE STRING "CMAKE_OSX_DEPLOYMENT_TARGET" FORCE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")

set(IOS ON CACHE BOOL "IOS" FORCE)
