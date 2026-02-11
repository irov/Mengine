MENGINE_PROJECT(ETC1Plugin)

ADD_FILTER(
src
    ETC1Plugin.h
    ETC1Plugin.def
    ETC1Plugin.cpp
    
    ImageDecoderETC1.cpp
    ImageDecoderETC1.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_ETC1)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)