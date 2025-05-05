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
	
	AppleISO8601Date.h
	AppleISO8601Date.mm

    AppleUserDefaults.h
    AppleUserDefaults.mm

    AppleKeyChain.h
    AppleKeyChain.mm

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

ADD_MENGINE_LIBRARY(Environment)