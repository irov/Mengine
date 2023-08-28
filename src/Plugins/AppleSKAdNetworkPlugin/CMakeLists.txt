MENGINE_PROJECT(AppleSKAdNetworkPlugin)

ADD_FILTER(
src
    AppleSKAdNetworkInterface.h
    
    AppleSKAdNetworkPlugin.h
    AppleSKAdNetworkPlugin.mm

    AppleSKAdNetworkApplicationDelegate.h
    AppleSKAdNetworkApplicationDelegate.mm
    
    AppleSKAdNetworkService.h
    AppleSKAdNetworkService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_SKADNETWORK)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleSKAdNetworkApplicationDelegate")

ADD_MENGINE_APPLE_FRAMEWORK(StoreKit)
