set( MY_LIB_NAME XlsExportPlugin )

ADD_FILTER(
src
	XlsExportPlugin.h
	XlsExportPlugin.cpp 
    XlsExportPlugin.def
    XlsScriptLogger.h
    XlsScriptLogger.cpp
)

add_definitions(-DMENGINE_PLUGIN_DLL)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}/pybind/include
                     ${THIRDPARTY_DIR}/libmath/include
                     ${THIRDPARTY_DIR}/stdex/include
                     
)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_definitions(-DMS_NO_COREDLL)
add_definitions(-DPy_NO_ENABLE_SHARED)
add_definitions(-DPYBIND_PYTHON_VERSION=342)
					 
ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind3${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python3${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}python3_decimal${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}tinyini${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX}
    Version.lib
    Shlwapi.lib
    Ws2_32.lib
    Crypt32.lib
    )
    
if(MSVC)
    TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}    
        ${THIRDPARTY_DIR}/openssl/libcrypto.lib
        ${THIRDPARTY_DIR}/openssl/libssl.lib
    )
endif()