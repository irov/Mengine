CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../pybind )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/base.cpp
${SOURCE_DIR}/src/kernel.cpp
${SOURCE_DIR}/src/kernel_python.cpp
${SOURCE_DIR}/src/adapter_interface.cpp
${SOURCE_DIR}/src/class_type_scope.cpp
${SOURCE_DIR}/src/debug.cpp
${SOURCE_DIR}/src/exception.cpp
${SOURCE_DIR}/src/extract.cpp
${SOURCE_DIR}/src/function_python.cpp
${SOURCE_DIR}/src/functor_python.cpp
${SOURCE_DIR}/src/member_python.cpp
${SOURCE_DIR}/src/method_python.cpp
${SOURCE_DIR}/src/pod_python.cpp
${SOURCE_DIR}/src/function.cpp
${SOURCE_DIR}/src/functor.cpp
${SOURCE_DIR}/src/logger.cpp
${SOURCE_DIR}/src/pickle.cpp
${SOURCE_DIR}/src/stl_type_cast.cpp
${SOURCE_DIR}/src/system.cpp
${SOURCE_DIR}/src/type_cast.cpp
${SOURCE_DIR}/src/helper.cpp
${SOURCE_DIR}/src/object.cpp
${SOURCE_DIR}/src/list.cpp
${SOURCE_DIR}/src/tuple.cpp
${SOURCE_DIR}/src/dict.cpp
${SOURCE_DIR}/src/dict_iterator.cpp
${SOURCE_DIR}/src/bindable.cpp
${SOURCE_DIR}/src/module.cpp
)

if( CMAKE_BUILD_TYPE MATCHES Debug )
    add_definitions(-DPYBIND_CALL_DEBUG)
endif()    

if(NOT MSVC)
    if(NOT EMSCRIPTEN)
        ADD_DEFINITIONS(-D"override = /* */")
        ADD_DEFINITIONS(-D"nullptr=0")
        ADD_DEFINITIONS(-D"nullptr_t=void*")     
    endif()
endif()

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPYBIND_PYTHON_VERSION=278)

include_directories(${SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Python)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Python/PC)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

