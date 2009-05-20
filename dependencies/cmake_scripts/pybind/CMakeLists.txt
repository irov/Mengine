
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind)

SET(TRUE_DIR ../../pybind)
SET(pybind_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( pybind )

# OUR SOURCES
set( pybind_sources
${TRUE_DIR}/src/class_core.cpp
${TRUE_DIR}/src/class_scope.cpp
${TRUE_DIR}/src/class_type.cpp
${TRUE_DIR}/src/constructor.cpp
${TRUE_DIR}/src/def.cpp
${TRUE_DIR}/src/def_proxy.cpp
${TRUE_DIR}/src/functor.cpp
${TRUE_DIR}/src/functor_proxy.cpp
${TRUE_DIR}/src/method_proxy.cpp
${TRUE_DIR}/src/method_type.cpp
${TRUE_DIR}/src/mutex.cpp
${TRUE_DIR}/src/system.cpp
${TRUE_DIR}/src/type_cast.cpp
${TRUE_DIR}/src/wrap_base.cpp
)

if(NOT MSVC)
	ADD_DEFINITIONS(-D'override = /* */')
endif(NOT MSVC)

add_definitions(-DPy_BUILD_CORE)

include_directories(${pybind_SOURCE_DIR}/include ${pybind_SOURCE_DIR}/../Python ${pybind_SOURCE_DIR}/../Python/PC)
add_library( ${pybind_LIB_NAME} STATIC ${pybind_sources} )
