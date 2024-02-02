set(CMAKE_STATIC_LIBRARY_PREFIX "lib")
set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")

SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "CMAKE_DEBUG_POSTFIX" FORCE)

set(LLVM_ENABLE_CXX1Y ON CACHE BOOL "" FORCE)

set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

set(CMAKE_OSX_ARCHITECTURES "$(ARCHS_STANDARD)" CACHE STRING "CMAKE_OSX_ARCHITECTURES" FORCE)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "CMAKE_OSX_DEPLOYMENT_TARGET" FORCE)
set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym" CACHE STRING "CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT" FORCE)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -x objective-c")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -x objective-c++")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -O3 -DNDEBUG")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g -O3 -DNDEBUG")