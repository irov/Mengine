MENGINE_PROJECT(AmplifierPlugin)

ADD_FILTER(
src
    AmplifierPlugin.h
    AmplifierPlugin.cpp
    AmplifierPlugin.def
    Amplifier.h
    Amplifier.cpp
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_AMPLIFIER)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf)
