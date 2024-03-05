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
    AppleSemaphoreService.h
    AppleSemaphoreService.mm
    
    AppleEnvironmentService.h
    AppleEnvironmentService.mm
    
    AppleNSLogger.h
    AppleNSLogger.mm
    
    ConstStringHolderNSString.h
    ConstStringHolderNSString.mm
)

ADD_MENGINE_APPLE_LIBRARY(Environment)