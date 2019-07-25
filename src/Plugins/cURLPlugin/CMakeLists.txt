set(MY_LIB_NAME cURLPlugin)

ADD_FILTER(
src
	cURLPlugin.h
	cURLPlugin.cpp
    cURLPlugin.def
    
    cURLInterface.h
    
    cURLService.cpp
    cURLService.h
    
    cURLThreadTask.cpp
    cURLThreadTask.h
    cURLGetAssetThreadTask.cpp
    cURLGetAssetThreadTask.h
    cURLGetMessageThreadTask.cpp
    cURLGetMessageThreadTask.h
    cURLHeaderDataThreadTask.cpp
    cURLHeaderDataThreadTask.h
    cURLPostMessageThreadTask.cpp
    cURLPostMessageThreadTask.h
	)
    
if(MENGINE_USE_PYTHON_FRAMEWORK)
ADD_FILTER(
embedding
    cURLScriptEmbedding.cpp
    cURLScriptEmbedding.h
    )    

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()

add_definitions(-DCURL_STATICLIB)

IF(NOT MENGINE_TOOLCHAIN_GRADLE)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/curl/include)
ENDIF()

IF(MENGINE_TOOLCHAIN_GRADLE)
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/curl-android/libs/armeabi-v7a/include)
ENDIF()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_CURL)

IF(MENGINE_TARGET_WINDOWS OR MENGINE_TARGET_MINGW)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/libcurl${MENGINE_LIB_SUFFIX})
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ws2_32.lib)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Crypt32.lib)
ENDIF()

IF(MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/libcurl${MENGINE_LIB_SUFFIX})
ENDIF()

IF(MENGINE_TARGET_ANDROID)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/curl-android/libs/armeabi-v7a/lib/libcurl.a)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/curl-android/libs/armeabi-v7a/lib/libssl.a)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_DIR}/curl-android/libs/armeabi-v7a/lib/libcrypto.a)
ENDIF()

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)