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
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Factory)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind3${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python3${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python3_decimal${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    Mincore.lib
    Shlwapi.lib
    )