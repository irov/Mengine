MENGINE_PROJECT(FontEffectPlugin)

ADD_FILTER(
src
    FontEffectPlugin.h
    FontEffectPlugin.cpp
    FontEffectPlugin.def

    FontEffectPluginInterface.h
    FontEffectDesc.h

    FontEffectHelper.h
    FontEffectHelper.cpp

    FontEffectSerialization.h
    FontEffectSerialization.cpp


    FontEffectDataInterface.h
    FontEffectData.h
    FontEffectData.cpp
    FontEffectDataflow.h
    FontEffectDataflow.cpp

    FontEffectRaster.h
    FontEffectRaster.cpp

    FontEffectBase.h
    FontEffectBase.cpp
    FontEffectFile.h
    FontEffectFile.cpp

    FontEffectService.h
    FontEffectService.cpp
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        FontEffectScriptEmbedding.cpp
        FontEffectScriptEmbedding.h
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jpp/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/jansson/include)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_FONTEFFECT)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
