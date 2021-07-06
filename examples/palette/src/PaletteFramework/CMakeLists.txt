MENGINE_PROJECT(PaletteFramework)

ADD_FILTER(
PaletteFramework
    PaletteFramework.cpp
    PaletteFramework.h
    PaletteFrameworkPlugin.cpp
    PaletteFrameworkPlugin.h
    PaletteSceneEventReceiver.cpp
    PaletteSceneEventReceiver.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_PALETTEFRAMEWORK)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Frameworks
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})