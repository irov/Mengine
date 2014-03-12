CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind32)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../pybind )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/class_core.cpp
${SOURCE_DIR}/src/class_info.cpp
${SOURCE_DIR}/src/class_type_scope.cpp
${SOURCE_DIR}/src/constructor.cpp
${SOURCE_DIR}/src/exception.cpp
${SOURCE_DIR}/src/extract.cpp
${SOURCE_DIR}/src/function.cpp
${SOURCE_DIR}/src/function_type.cpp
${SOURCE_DIR}/src/functor.cpp
${SOURCE_DIR}/src/functor_type.cpp
${SOURCE_DIR}/src/member_type.cpp
${SOURCE_DIR}/src/method_type.cpp
${SOURCE_DIR}/src/system.cpp
${SOURCE_DIR}/src/type_cast.cpp
${SOURCE_DIR}/src/stl_type_cast.cpp
)

if(NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
    ADD_DEFINITIONS(-D"nullptr=0")
    ADD_DEFINITIONS(-D"nullptr_t=void*")    
endif(NOT MSVC)

add_definitions(-DPy_BUILD_CORE)
add_definitions(-DPYBIND_PYTHON_VERSION=320)

include_directories(${SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Python32)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)

IF(WIN32)
    include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../Python32/PC)
ENDIF(WIN32)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

