MENGINE_PROJECT(SDL2)

ADD_FILTER(
src
    SDL2Includer.h
    SDL2MainIncluder.h
    
    SDL2PlatformServiceExtensionInterface.h
    
    SDL2MessageBoxLogger.h
    SDL2MessageBoxLogger.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL2/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Environment)