set( MY_LIB_NAME AndroidNativeUnityAdsPlugin )

ADD_FILTER(
src
	AndroidNativeUnityAdsPlugin.h
	AndroidNativeUnityAdsPlugin.cpp
    AndroidNativeUnityAdsModule.h
    AndroidNativeUnityAdsModule.cpp
	UnityAdsEventHandler.h
)

INCLUDE_DIRECTORIES( 
		${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/libmath/include
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS)

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})

IF(MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_DLL)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
    TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Android)
ELSE()
    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     ${MY_LIB_NAME}
     Engine
    )

    SET( APPLICATION_DEPENDENCIES
     ${APPLICATION_DEPENDENCIES}
     PARENT_SCOPE
    )

    SET( APPLICATION_LIBS
        ${APPLICATION_LIBS}
        Kernel
        Engine
        Android	
        )

    SET( APPLICATION_LIBS
     ${APPLICATION_LIBS}
     PARENT_SCOPE
    )
ENDIF()