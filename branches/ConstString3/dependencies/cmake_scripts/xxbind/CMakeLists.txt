
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(xxbind)

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../xxbind )


if (NOT MSVC)
    ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)  

set( xxbind_sources
${SOURCE_DIR}/src/class_adapter_helper.cpp
${SOURCE_DIR}/src/exception.cpp
${SOURCE_DIR}/src/type_cast.cpp
)

set( pybind_sources
${SOURCE_DIR}/pybind/pyclass_type.cpp
${SOURCE_DIR}/pybind/pyfacade_script.cpp
${SOURCE_DIR}/pybind/pyfunction_type.cpp
${SOURCE_DIR}/pybind/pyhelper.cpp
${SOURCE_DIR}/pybind/pymember_type.cpp
${SOURCE_DIR}/pybind/pymethod_type.cpp
${SOURCE_DIR}/pybind/pytype_cast_default.cpp
)

if(NOT MSVC)
	ADD_DEFINITIONS(-D"override = /* */")
endif(NOT MSVC)

add_definitions(-DPy_BUILD_CORE)
#add_definitions(-DPy_USING_UNICODE)

include_directories(${SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../Python)

IF(WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Python/PC)
ENDIF(WIN32)

add_library( ${PROJECT_NAME}_static STATIC ${pybind_sources} )

