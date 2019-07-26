SET(MY_LIB_NAME NodeLeakDetectorPlugin)

ADD_FILTER(
src
    NodeLeakDetectorPlugin.h
    NodeLeakDetectorPlugin.cpp
    NodeLeakDetectorPlugin.def
    NodeLeakDetectorServiceInterface.h
    NodeLeakDetectorService.h
    NodeLeakDetectorService.cpp
)

ADD_MENGINE_LIBRARY(Plugins)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)