MENGINE_PROJECT(MacOS)

ADD_FILTER(
src
    MacOSUtils.h
    MacOSUtils.mm
    
    MacOSProxyApplicationDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)