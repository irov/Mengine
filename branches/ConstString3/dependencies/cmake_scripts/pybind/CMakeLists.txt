
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind)

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/pybind )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( pybind )

# OUR SOURCES
set( pybind_sources
${CMAKE_CURRENT_SOURCE_DIR}/src/class_core.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/class_type.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/constructor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/def.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/def_adapter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/functor.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/functor_proxy.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/member_type.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/method_adapter.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/method_type.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/mutex.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/system.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/type_cast.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/type_cast_default.cpp
${CMAKE_CURRENT_SOURCE_DIR}/src/wrap_base.cpp
)

if (NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)

add_definitions(-DPy_BUILD_CORE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/Python ${CMAKE_SOURCE_DIR}/Python/PC)
add_library( ${pybind_LIB_NAME} STATIC ${pybind_sources} )

