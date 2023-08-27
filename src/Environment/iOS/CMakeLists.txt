MENGINE_PROJECT(iOS)

ADD_FILTER(
src
    iOSUtils.h
    iOSUtils.mm
    
    iOSDetail.h
    iOSDetail.mm
    
    UIMainApplicationDelegateInterface.h
    UIProxyApplicationDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)
