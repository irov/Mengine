MENGINE_PROJECT(AppleDevToDevPlugin)

ADD_FILTER(
src
    AppleDevToDevApplicationDelegate.h
    AppleDevToDevApplicationDelegate.mm
)

ADD_MENGINE_COCOAPOD("DTDAnalytics" "NO-GIT" "2.5.1")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleDevToDevApplicationDelegate")