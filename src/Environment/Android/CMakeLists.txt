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
    AndroidActivityHelper.h
    AndroidActivityHelper.cpp
    AndroidApplicationHelper.h
    AndroidApplicationHelper.cpp
    AndroidHelper.h
    AndroidHelper.cpp
    
    AndroidKernelService.h
    AndroidKernelService.cpp

    AndroidEnvironmentService.h
    AndroidEnvironmentService.cpp
    
    AndroidAssetService.h
    AndroidAssetService.cpp

    AndroidSemaphoreListenerInterface.h
    
    ConstStringHolderJString.h
    ConstStringHolderJString.cpp
    
    AndroidLogger.h
    AndroidLogger.cpp
    
    AndroidProxyLogger.h
    AndroidProxyLogger.cpp
)

ADD_MENGINE_LIBRARY(Environment)