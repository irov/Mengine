MENGINE_PROJECT(iOS)

ADD_FILTER(
src    
    iOSDetail.h
    iOSDetail.mm
	
	iOSLog.h
	iOSLog.mm
    
    iOSApplication.h
    iOSApplication.mm
    
    iOSKernelServiceInterface.h

    iOSKernelService.h
    iOSKernelService.mm
    
    iOSEnvironmentService.h
    iOSEnvironmentService.mm

    iOSConsentFlowUserGeography.h
    
    iOSAppTrackingAuthorization.h
    iOSAppTrackingTransparencyParam.h
    iOSAppTrackingTransparencyParam.mm

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
    
    iOSAnalyticsEventCategory.h
    
    iOSNetwork.h
    iOSNetwork.mm
    
    iOSUIMainApplicationDelegateInterface.h
    
    iOSPluginUserIdDelegateInterface.h
    iOSPluginAdRevenueDelegateInterface.h
    iOSPluginApplicationDelegateInterface.h
    iOSPluginAppTrackingTransparencyDelegateInterface.h
    iOSPluginTransparencyConsentDelegateInterface.h
    iOSPluginLoggerDelegateInterface.h
	iOSPluginConfigDelegateInterface.h
    iOSPluginAnalyticDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)
