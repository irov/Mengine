MENGINE_PROJECT(SDL)

ADD_FILTER(
src
    SDLIncluder.h
    
    SDLPlatformServiceExtensionInterface.h
    
    SDLMessageBoxLogger.h
    SDLMessageBoxLogger.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include)

ADD_MENGINE_LIBRARY(Environment)