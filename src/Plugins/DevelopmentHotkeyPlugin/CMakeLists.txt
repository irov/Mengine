MENGINE_PROJECT(DevelopmentHotkeyPlugin)

ADD_FILTER(
src
    DevelopmentHotkeyPlugin.h
    DevelopmentHotkeyPlugin.def
    DevelopmentHotkeyPlugin.cpp

    DevelopmentHotkeyReloadLocaleSceneChange.h
    DevelopmentHotkeyReloadLocaleSceneChange.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DEVELOPMENTHOTKEY)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
