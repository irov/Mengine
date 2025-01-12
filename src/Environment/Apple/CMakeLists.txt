MENGINE_PROJECT(Apple)

ADD_FILTER(
src
    AppleLog.h
    AppleLog.mm
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
    
    AppleNSLogger.h
    AppleNSLogger.mm
    
    AppleFactorablePtr.h
    AppleFactorablePtr.mm
)

ADD_MENGINE_APPLE_LIBRARY(Environment)