set(MY_LIB_NAME AndroidNativeDevToDevPlugin)

ADD_FILTER(
src
    AndroidNativeDevToDevModule.h
    AndroidNativeDevToDevModule.cpp
	AndroidNativeDevToDevPlugin.h
	AndroidNativeDevToDevPlugin.cpp 
    DevToDevEventHandler.h
)

INCLUDE_DIRECTORIES( 
		${THIRDPARTY_DIR}/pybind/include
        ${THIRDPARTY_DIR}/libmath/include
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Android)
