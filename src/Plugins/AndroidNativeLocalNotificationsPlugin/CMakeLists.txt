set(MY_LIB_NAME AndroidNativeLocalNotificationsPlugin)

ADD_FILTER(
src
    LocalNotificationsEventHandler.h
    AndroidNativeLocalNotificationsModule.h
    AndroidNativeLocalNotificationsModule.cpp
	AndroidNativeLocalNotificationsPlugin.h
	AndroidNativeLocalNotificationsPlugin.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ANDROID_NATIVE_LOCAL_NOTIFICATIONS)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Android)