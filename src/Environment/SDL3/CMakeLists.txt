MENGINE_PROJECT(SDL3)

ADD_FILTER(
src
    SDL3Includer.h
    SDL3MainIncluder.h
    
    SDL3PlatformServiceExtensionInterface.h
    
    SDL3MessageBoxLogger.h
    SDL3MessageBoxLogger.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include/include-config-$<LOWER_CASE:$<CONFIG>>)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL3/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL3/include)

ADD_MENGINE_LIBRARY(Environment)



