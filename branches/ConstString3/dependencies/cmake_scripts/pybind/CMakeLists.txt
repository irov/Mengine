
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../pybind )

# OUR SOURCES
set( pybind_sources
${SOURCE_DIR}/src/class_info.cpp
${SOURCE_DIR}/src/class_core.cpp
${SOURCE_DIR}/src/class_type.cpp
${SOURCE_DIR}/src/constructor.cpp
${SOURCE_DIR}/src/function.cpp
${SOURCE_DIR}/src/function_type.cpp
${SOURCE_DIR}/src/functor.cpp
${SOURCE_DIR}/src/functor_type.cpp
${SOURCE_DIR}/src/functor_proxy.cpp
${SOURCE_DIR}/src/member_type.cpp
${SOURCE_DIR}/src/method_adapter.cpp
${SOURCE_DIR}/src/method_type.cpp
${SOURCE_DIR}/src/mutex.cpp
${SOURCE_DIR}/src/system.cpp
${SOURCE_DIR}/src/type_cast.cpp
${SOURCE_DIR}/src/wrap_base.cpp
${SOURCE_DIR}/src/extract.cpp
)

if(NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)

add_definitions(-DPy_BUILD_CORE)

include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../Python)

IF(WIN32)
    include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../Python/PC)
ENDIF(WIN32)

add_library( ${PROJECT_NAME}_static STATIC ${pybind_sources} )

