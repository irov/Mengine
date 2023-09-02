MENGINE_PROJECT(iOS)

ADD_FILTER(
src
    iOSUtils.h
    iOSUtils.mm
    
    iOSDetail.h
    iOSDetail.mm
    
    iOSIncluder.h
    
    iOSEnvironmentService.h
    iOSEnvironmentService.mm
    
    iOSMailCompose.h
    iOSMailCompose.mm
    
    UIMainApplicationDelegateInterface.h
    UIPluginApplicationDelegateInterface.h
)

ADD_MENGINE_APPLE_LIBRARY(Environment)