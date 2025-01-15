MENGINE_PROJECT(iOS)

ADD_FILTER(
src    
    iOSDetail.h
    iOSDetail.mm
	
	iOSLog.h
	iOSLog.mm
    
    iOSIncluder.h
    
    iOSApplication.h
    iOSApplication.mm

    iOSKernelService.h
    iOSKernelService.mm
    
    iOSEnvironmentService.h
    iOSEnvironmentService.mm

    iOSTransparencyConsentParam.h
    iOSTransparencyConsentParam.mm

    iOSSessionIdParam.h
    iOSSessionIdParam.mm

    iOSAdRevenueParam.h
    iOSAdRevenueParam.mm
    
    iOSUIMainApplicationDelegateInterface.h
    
    iOSPluginSessionIdDelegateInterface.h
    iOSPluginAdRevenueDelegateInterface.h
    iOSPluginApplicationDelegateInterface.h
    iOSPluginTransparencyConsentDelegateInterface.h
    iOSPluginLoggerDelegateInterface.h
)

ADD_MENGINE_APPLE_LIBRARY(Environment)