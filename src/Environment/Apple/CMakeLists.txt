MENGINE_PROJECT(Apple)

ADD_FILTER(
src
    AppleLog.h
    AppleLog.mm
	AppleLogRecordParam.h
	AppleLogRecordParam.mm
    AppleDetail.h
    AppleDetail.mm
    AppleBundle.h
    AppleBundle.mm
    AppleString.h
    AppleString.mm
    
    AppleEvent.h
    AppleEvent.mm

    AppleUserDefaults.h
    AppleUserDefaults.mm

    AppleKeyChain.h
    AppleKeyChain.mm
    
    AppleAnalytics.h
    AppleAnalytics.mm

    AppleSemaphore.h
    AppleSemaphore.mm
    AppleSemaphoreListenerNSProxy.h
    AppleSemaphoreListenerNSProxy.mm
    AppleSemaphoreService.h
    AppleSemaphoreService.mm
    
    AppleKernelService.h
    AppleKernelService.mm
    
    AppleSemaphoreListenerInterface.h
    
    AppleFactorablePtr.h
    AppleFactorablePtr.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
ADD_FILTER(
    python
    ApplePythonProvider.h
    ApplePythonProvider.mm
)
endif()

ADD_MENGINE_APPLE_LIBRARY(Environment)

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()