set( MY_LIB_NAME AndroidNativeAdMobPlugin )

ADD_FILTER(
src
	AndroidNativeAdMobPlugin.h
	AndroidNativeAdMobPlugin.cpp
	AdMobInterstitialEventHandler.h
	AdMobRewardedVideoEventHandler.h 
)

if( MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_DLL )
   add_definitions(-DMENGINE_PLUGIN_DLL)
endif()

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils
		${THIRDPARTY_DIR}/pybind/include
)

ADD_MENGINE_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

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
	Logger
	Core
	Codec
	Archive
	Factory
	Kernel
	Engine
	AndroidUtils	
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX}
    )

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)