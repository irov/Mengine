set( MY_LIB_NAME MengeXlsExportPlugin )

ADD_FILTER(
src
	XlsExportPlugin.h
	XlsExportPlugin.cpp 
    XlsExportPlugin.def
    XlsScriptLogger.h
    XlsScriptLogger.cpp
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}/pybind/include
                     ${THIRDPARTY_DIR}/boost
)
					 
ADD_LIBRARY( ${MY_LIB_NAME} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Logger)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)

TARGET_LINK_LIBRARIES( ${MY_LIB_NAME}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}pybind32_static
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}python32_static
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}zlib_static
    )