MENGINE_PROJECT(MacOS)

ADD_FILTER(
src
    MacOSUtils.h
    MacOSUtils.mm

    MacOSWorkspace.h
    MacOSWorkspace.mm

    MacOSApplication.h
    MacOSApplication.mm

    MacOSEnvironmentService.h
    MacOSEnvironmentService.mm

    MacOSProxyApplicationDelegateInterface.h
    MacOSPlatformServiceExtensionInterface.h
    MacOSRenderPlatformServiceExtensionInterface.h
    MacOSWebViewPlatformServiceExtensionInterface.h
)

ADD_MENGINE_LIBRARY(Environment)
