MENGINE_PROJECT(SimpleFramework)

ADD_FILTER(
SimpleFrameworkPlugin
    SimpleFrameworkPlugin.cpp
    SimpleFrameworkPlugin.h
    SimpleFrameworkPlugin.def
)

ADD_FILTER(
SimpleSprite
    SimpleRandomizer.cpp
    SimpleRandomizer.h
)

ADD_FILTER(
SimpleSprite
    SimpleSprite.cpp
    SimpleSprite.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SIMPLEFRAMEWORK Frameworks)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)