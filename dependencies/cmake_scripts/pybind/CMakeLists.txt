
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(pybind)

SET(TRUE_DIR ../../pybind)
SET(pybind_SOURCE_DIR ${TRUE_DIR})

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(pybind_LIB_NAME lib)
endif(MSVC)
set( pybind_LIB_NAME ${pybind_LIB_NAME}pybind_static )

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

# OUR SOURCES
set(libpybind_sources
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

# SOME NEEDED DEFINITIONS
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

if(NOT MSVC)
	ADD_DEFINITIONS(-D'override = /* */')
endif(NOT MSVC)

include_directories(${pybind_SOURCE_DIR}/include)
add_library( ${pybind_LIB_NAME} STATIC ${libpybind_sources} )

#install_targets(/lib ${pybind_LIB_NAME})
