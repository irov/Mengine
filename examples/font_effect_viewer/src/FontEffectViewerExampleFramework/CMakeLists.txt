MENGINE_PROJECT(FontEffectViewerExampleGame)

ADD_FILTER(
FontEffectViewerExampleGame
    FontEffectViewerExampleGame.cpp
    FontEffectViewerExampleGame.h
    FontEffectViewerExampleGamePlugin.cpp
    FontEffectViewerExampleGamePlugin.h
    FontEffectViewerExampleFileDialog.h
    FontEffectViewerExampleDescEditor.cpp
    FontEffectViewerExampleDescEditor.h
    FontEffectViewerExampleSceneEventReceiver.cpp
    FontEffectViewerExampleSceneEventReceiver.h
    FontEffectViewerExamplePreviewCanvas.cpp
    FontEffectViewerExamplePreviewCanvas.h
)

if(MENGINE_PLATFORM_MACOS)
    ADD_FILTER(
    FontEffectViewerExampleGame
        FontEffectViewerExampleFileDialog.mm
    )
else()
    ADD_FILTER(
    FontEffectViewerExampleGame
        FontEffectViewerExampleFileDialog.cpp
    )
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/imgui/backends)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/freetype/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_FONTEFFECTVIEWEREXAMPLEFRAMEWORK)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER Game
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Engine)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ImGUIFramework)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} FontEffectPlugin)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} TTFPlugin)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}imgui${MENGINE_LIB_SUFFIX})

if(MENGINE_PLATFORM_MACOS)
    ADD_MENGINE_APPLE_FRAMEWORK(AppKit)
endif()
