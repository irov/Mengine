MENGINE_PROJECT(AppleAmazonPlugin)

ADD_FILTER(
src
    AppleAmazonApplicationDelegate.h
    AppleAmazonApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_AMAZON)

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("AmazonPublisherServicesSDK" "NO-GIT" "5.1.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleAmazonApplicationDelegate")