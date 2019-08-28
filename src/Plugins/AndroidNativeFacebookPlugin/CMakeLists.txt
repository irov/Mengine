set(MY_LIB_NAME AndroidNativeFacebookPlugin)

ADD_FILTER(
src
	AndroidNativeFacebookPlugin.h
	AndroidNativeFacebookPlugin.cpp
    AndroidNativeFacebookModule.h
	AndroidNativeFacebookModule.cpp
    FacebookEventHandler.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})    

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Android)