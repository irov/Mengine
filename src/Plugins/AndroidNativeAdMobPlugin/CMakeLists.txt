set( MY_LIB_NAME AndroidNativeAdMobPlugin )

ADD_FILTER(
src
    AdMobEventHandler.h
    AndroidNativeAdMobModule.h
    AndroidNativeAdMobModule.cpp
	AndroidNativeAdMobPlugin.h
	AndroidNativeAdMobPlugin.cpp
)

if( MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_DLL )
   add_definitions(-DMENGINE_PLUGIN_DLL)
endif()

INCLUDE_DIRECTORIES( 
		${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/libmath/include
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

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
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)