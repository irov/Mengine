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
    AndroidAssetFile.h
    AndroidAssetFile.cpp
    AndroidHelper.h
    AndroidHelper.cpp
    AndroidUtils.h
    AndroidUtils.cpp

    AndroidEnvironmentService.h
    AndroidEnvironmentService.cpp
    
    ConstStringHolderJString.h
    ConstStringHolderJString.cpp
    
    AndroidProxyLogger.h
    AndroidProxyLogger.cpp
)

ADD_MENGINE_LIBRARY(Environment)