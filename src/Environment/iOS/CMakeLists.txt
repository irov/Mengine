MENGINE_PROJECT(iOS)

ADD_FILTER(
src    
    iOSDetail.h
    iOSDetail.mm
    
    iOSIncluder.h
    
    iOSApplication.h
    iOSApplication.mm

    iOSKernelService.h
    iOSKernelService.mm
    
    iOSEnvironmentService.h
    iOSEnvironmentService.mm
    
    iOSMailCompose.h
    iOSMailCompose.mm
    
    iOSProxyLogger.h
    iOSProxyLogger.mm

    iOSTransparencyConsentParam.h
    iOSTransparencyConsentParam.mm

    iOSAdRevenueParam.h
    iOSAdRevenueParam.mm

    iOSLogRecordParam.h
    iOSLogRecordParam.mm
    
    iOSUIMainApplicationDelegateInterface.h
    
    iOSPluginAdRevenueDelegateInterface.h
    iOSPluginApplicationDelegateInterface.h
    iOSPluginTransparencyConsentDelegateInterface.h
    iOSPluginLoggerDelegateInterface.h
)

ADD_MENGINE_APPLE_LIBRARY(Environment)