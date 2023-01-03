MENGINE_PROJECT(iOS)

ADD_FILTER(
src
    iOSUtils.h
    iOSUtils.mm
    
    iOSDetail.h
    iOSDetail.mm
    
    UIKitProxyApplicationDelegateBase.h
    UIKitProxyApplicationDelegateBase.mm
)

ADD_MENGINE_LIBRARY(Environment)
