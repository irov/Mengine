MENGINE_PROJECT(Apple)

ADD_FILTER(
src
    AppleLog.h
    AppleLog.mm
    AppleDetail.h
    AppleDetail.mm
    AppleBundle.h
    AppleBundle.mm
    AppleErrorHelper.h
    AppleErrorHelper.mm
    AppleString.h
    AppleString.mm

    AppleUserDefaults.h
    AppleUserDefaults.mm
    
    AppleAnalytics.h
    AppleAnalytics.mm

    AppleSemaphore.h
    AppleSemaphore.mm
    AppleSemaphoreListenerNSProxy.h
    AppleSemaphoreListenerNSProxy.mm
    AppleSemaphoreService.h
    AppleSemaphoreService.mm
    
    AppleEnvironmentService.h
    AppleEnvironmentService.mm
    
    AppleSemaphoreListenerInterface.h
    
    AppleNSLogger.h
    AppleNSLogger.mm
    
    ConstStringHolderNSString.h
    ConstStringHolderNSString.mm
)

ADD_MENGINE_APPLE_LIBRARY(Environment)