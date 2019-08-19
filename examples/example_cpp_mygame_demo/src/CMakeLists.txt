set(MY_LIB_NAME MyGameFramework)

ADD_FILTER(
MyGameFramework
    MyGameFramework.cpp
    MyGameFramework.h
    MyGameFrameworkPlugin.cpp
    MyGameFrameworkPlugin.h
    MySceneEventReceiver.cpp
    MySceneEventReceiver.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_MYGAMEFRAMEWORK)

set_target_properties(${MY_LIB_NAME} PROPERTIES
    FOLDER Frameworks
)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Kernel)
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Engine)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})