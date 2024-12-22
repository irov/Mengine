MENGINE_PROJECT(AmplifierPlugin)

ADD_FILTER(
src
    AmplifierInterface.h

    AmplifierPlugin.h
    AmplifierPlugin.cpp
    AmplifierPlugin.def
    Amplifier.h
    Amplifier.cpp
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AmplifierScriptEmbedding.h
        AmplifierScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_AMPLIFIER)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
