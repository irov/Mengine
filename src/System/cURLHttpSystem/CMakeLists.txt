set( MY_LIB_NAME cURLHttpSystem )

ADD_FILTER(
src
	cURLHttpSystem.cpp
	cURLHttpSystem.h
    
    ThreadTaskCurl.h
    ThreadTaskCurl.cpp
    ThreadTaskGetMessage.h
    ThreadTaskGetMessage.cpp
    ThreadTaskPostMessage.h
    ThreadTaskPostMessage.cpp
    ThreadTaskGetAsset.h
    ThreadTaskGetAsset.cpp
    ThreadTaskHeaderData.h
    ThreadTaskHeaderData.cpp
)

add_definitions(-DCURL_STATICLIB)

INCLUDE_DIRECTORIES(         
    ${THIRDPARTY_DIR}/libmath/include
    )

IF(WIN32)
    INCLUDE_DIRECTORIES( 
        ${THIRDPARTY_DIR}/curl/include 
        )
ENDIF()

IF(MENGINE_PLATFORM_ANDROID)
    INCLUDE_DIRECTORIES( 
        ${THIRDPARTY_DIR}/curl-android/armeabi-v7a/include
        )
ENDIF()

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER System
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
)

IF(WIN32)
    SET( APPLICATION_LIBS
     ${APPLICATION_LIBS}
     ${THIRDPARTY_LIB_DIR}/libcurl_imp${MENGINE_LIB_SUFFIX}
        ws2_32.lib
        Crypt32.lib
    )
ENDIF()

IF(MENGINE_PLATFORM_ANDROID)
    SET( APPLICATION_LIBS
     ${APPLICATION_LIBS}
     ${THIRDPARTY_DIR}/curl-android/armeabi-v7a/lib/libcrypto.a
     ${THIRDPARTY_DIR}/curl-android/armeabi-v7a/lib/libcurl.a
     ${THIRDPARTY_DIR}/curl-android/armeabi-v7a/lib/libssl.a
    )
ENDIF()

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)
