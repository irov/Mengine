MENGINE_PROJECT(MacOS)

ADD_FILTER(
src
    MacOSUtils.h
    MacOSUtils.mm
    
    MacOSApplication.h
    MacOSApplication.mm
    
    MacOSEnvironmentService.h
    MacOSEnvironmentService.mm
    
    MacOSProxyApplicationDelegateInterface.h
)

ADD_MENGINE_LIBRARY(Environment)