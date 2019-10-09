SET(MY_LIB_NAME FactoryLeakDetectorPlugin)

ADD_FILTER(
src
    FactoryLeakDetectorPlugin.h
    FactoryLeakDetectorPlugin.cpp
    FactoryLeakDetectorPlugin.def
    FactoryLeakDetectorServiceInterface.h
    FactoryLeakDetectorService.h
    FactoryLeakDetectorService.cpp
)

ADD_MENGINE_LIBRARY(Plugins)