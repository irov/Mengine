set(MY_LIB_NAME AndroidNativeAdMobPlugin)

ADD_FILTER(
src
    AdMobEventHandler.h
    AndroidNativeAdMobModule.h
    AndroidNativeAdMobModule.cpp
	AndroidNativeAdMobPlugin.h
	AndroidNativeAdMobPlugin.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB)

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})    

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Android)