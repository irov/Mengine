MENGINE_PROJECT(FigmaViewerExampleGame)

ADD_FILTER(
FigmaViewerExampleGame
    FigmaViewerExampleGame.cpp
    FigmaViewerExampleGame.h
    FigmaViewerExampleGamePlugin.cpp
    FigmaViewerExampleGamePlugin.h
    FigmaViewerExampleFileDialog.h
    FigmaViewerExampleSceneEventReceiver.cpp
    FigmaViewerExampleSceneEventReceiver.h
)

if(MENGINE_PLATFORM_MACOS)
    ADD_FILTER(
    FigmaViewerExampleGame
        FigmaViewerExampleFileDialog.mm
    )
else()
    ADD_FILTER(
    FigmaViewerExampleGame
        FigmaViewerExampleFileDialog.cpp
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui/backends)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/figma/sdk/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_FIGMAVIEWEREXAMPLEFRAMEWORK)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Game
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ImGUIFramework)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} FigmaPlugin)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})

if(MENGINE_PLATFORM_MACOS)
    ADD_MENGINE_APPLE_FRAMEWORK(AppKit)
endif()
