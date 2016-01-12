set( MY_LIB_NAME MengeXlsExportPlugin )

ADD_FILTER(
src
	XlsExportPlugin.h
	XlsExportPlugin.cpp 
    XlsExportPlugin.def
    XlsScriptLogger.h
    XlsScriptLogger.cpp
)

add_definitions(-DMENGE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
                     ${THIRDPARTY_DIR}/boost
					 ${THIRDPARTY_DIR}/pybind/include
                     ${THIRDPARTY_DIR}/stdex/include
                     
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPYBIND_PYTHON_VERSION=342)
					 
ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pybind3_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python3_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python3_decimal_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}
    )