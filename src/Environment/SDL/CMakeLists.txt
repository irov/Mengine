MENGINE_PROJECT(SDL)

ADD_FILTER(
src
    SDLIncluder.h
    
    SDLPlatformServiceExtensionInterface.h
    
    SDLMessageBoxLogger.h
    SDLMessageBoxLogger.cpp
)

ADD_MENGINE_LIBRARY(Environment)