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

    iOSConsentFlowUserGeography.h

    iOSTransparencyConsentParam.h
    iOSTransparencyConsentParam.mm

    iOSUserParam.h
    iOSUserParam.mm

    iOSAdRevenueParam.h
    iOSAdRevenueParam.mm
    
    iOSAdFormat.h
    iOSAdFormat.mm
    
    iOSAnalytics.h
    iOSAnalytics.mm
    
    iOSNetwork.h
    iOSNetwork.mm
    
    iOSUIMainApplicationDelegateInterface.h
    
    iOSPluginSessionIdDelegateInterface.h
    iOSPluginAdRevenueDelegateInterface.h
    iOSPluginApplicationDelegateInterface.h
    iOSPluginTransparencyConsentDelegateInterface.h
    iOSPluginLoggerDelegateInterface.h
	iOSPluginConfigDelegateInterface.h
    iOSPluginAnalyticDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)
