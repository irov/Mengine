MENGINE_PROJECT(Android)

ADD_FILTER(
src
    AndroidIncluder.h
    AndroidDeclaration.h
    AndroidEnv.h
    AndroidEnv.cpp
    AndroidActivityHelper.h
    AndroidActivityHelper.cpp
    AndroidApplicationHelper.h
    AndroidApplicationHelper.cpp
    AndroidHelper.h
    AndroidHelper.cpp
    
    AndroidKernelServiceInterface.h
    
    AndroidKernelService.h
    AndroidKernelService.cpp

    AndroidEnvironmentService.h
    AndroidEnvironmentService.cpp
    
    AndroidFragmentHelper.h
    AndroidFragmentHelper.cpp
    
    AndroidAssetService.h
    AndroidAssetService.cpp

    AndroidPluginCallbackInterface.h
    AndroidSemaphoreListenerInterface.h
    
    ConstStringHolderJString.h
    ConstStringHolderJString.cpp
    
    AndroidLogger.h
    AndroidLogger.cpp
    
    AndroidPlatformServiceExtensionInterface.h
    
    stack_chk_fail_local.c
)

ADD_MENGINE_LIBRARY(Environment)

find_library(android-lib android)
find_library(log-lib log)
find_library(dl-lib dl)
find_library(c-lib c)
find_library(m-lib m)
find_library(stdcxx-lib stdc++)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${android-lib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${log-lib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${stdcxx-lib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${c-lib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${m-lib})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${dl-lib})