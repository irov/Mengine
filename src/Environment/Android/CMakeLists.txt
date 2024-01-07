MENGINE_PROJECT(Android)

ADD_FILTER(
src
    AndroidIncluder.h
    AndroidEventation.h
    AndroidEventationInterface.h
    AndroidEventationHub.h
    AndroidEventationHub.cpp
    AndroidEnv.h
    AndroidEnv.cpp
    AndroidHelper.h
    AndroidHelper.cpp

    AndroidEnvironmentService.h
    AndroidEnvironmentService.cpp
    
    AndroidSemaphoreListener.h
    AndroidSemaphoreListener.cpp
    
    ConstStringHolderJString.h
    ConstStringHolderJString.cpp
    
    AndroidLogger.h
    AndroidLogger.cpp
    
    AndroidProxyLogger.h
    AndroidProxyLogger.cpp
)

ADD_MENGINE_LIBRARY(Environment)