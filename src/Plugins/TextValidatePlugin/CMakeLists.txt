MENGINE_PROJECT(TextValidatePlugin)

ADD_FILTER(
src
    TextValidatePlugin.h
    TextValidatePlugin.def
    TextValidatePlugin.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_TEXTVALIDATE)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)