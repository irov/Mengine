SET(MY_LIB_NAME AmplifierPlugin)

ADD_FILTER(
src
    AmplifierPlugin.h
    AmplifierPlugin.cpp
    AmplifierPlugin.def
    Amplifier.h
    Amplifier.cpp
)

ADD_MENGINE_LIBRARY(Plugins)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf)
