SET(MENGINE_LIB_PREFIX "lib")
SET(MENGINE_LIB_SUFFIX ".a")
SET(CMAKE_DEBUG_POSTFIX "")

SET(CMAKE_C_STANDARD 11)

SET(CMAKE_POLICY_DEFAULT_CMP0069 NEW)
SET(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

OPTION(MENGINE_EMSCRIPTEN_ALLOW_MEMORY_GROWTH "Allow WebAssembly memory to grow at runtime" ON)
SET(MENGINE_EMSCRIPTEN_MAXIMUM_MEMORY "4GB" CACHE STRING "Maximum WebAssembly memory when growth is enabled")
SET(MENGINE_EMSCRIPTEN_STACK_SIZE "2097152" CACHE STRING "WebAssembly stack size in bytes")
SET(MENGINE_EMSCRIPTEN_INITIAL_MEMORY "67108864" CACHE STRING "Initial WebAssembly memory in bytes")

SET(MENGINE_EMSCRIPTEN_MIN_WEBGL_VERSION "1" CACHE STRING "Minimum supported WebGL version")
SET(MENGINE_EMSCRIPTEN_MAX_WEBGL_VERSION "1" CACHE STRING "Maximum supported WebGL version")
OPTION(MENGINE_EMSCRIPTEN_GL_ENABLE_GET_PROC_ADDRESS "Enable OpenGL procedure address lookup" OFF)

ADD_COMPILE_OPTIONS(
    -fno-strict-aliasing
    "$<$<COMPILE_LANGUAGE:CXX>:-frtti>"
    -fexceptions
)

ADD_LINK_OPTIONS(
    -fexceptions
    -sSUPPORT_LONGJMP=emscripten
)

IF(MENGINE_USE_ADDRESS_SANITIZE)
    ADD_COMPILE_OPTIONS(
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
    )

    ADD_LINK_OPTIONS(-fsanitize=address,undefined)
ENDIF()
