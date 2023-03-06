MENGINE_PROJECT(iOS)

ADD_FILTER(
src
    iOSUtils.h
    iOSUtils.mm
    
    iOSDetail.h
    iOSDetail.mm
    
    UIKitProxyApplicationDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)
