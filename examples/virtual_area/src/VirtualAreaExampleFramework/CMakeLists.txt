MENGINE_PROJECT(VirtualAreaExampleGame)

ADD_FILTER(
VirtualAreaExampleGame
    VirtualAreaExampleGame.cpp
    VirtualAreaExampleGame.h
    VirtualAreaExampleGamePlugin.cpp
    VirtualAreaExampleGamePlugin.h
    VirtualAreaExampleSceneEventReceiver.cpp
    VirtualAreaExampleSceneEventReceiver.h
)

INCLUDE_DIRECTORIES(SYSTEM ${THIRDPARTY_DIR}/mosaic/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_VIRTUALAREAEXAMPLEFRAMEWORK)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Game
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} MosaicFramework)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
