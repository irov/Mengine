MENGINE_PROJECT(iOSAmazonPlugin)

ADD_FILTER(
src
    iOSAmazonPlugin.h
    iOSAmazonPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_AMAZON)

ADD_MENGINE_SWIFT_PACKAGE("AmazonPublisherServicesSDK" "https://github.com/amzn/swift-package-manager-amazon-aps.git" "5.6.6")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSAmazonPlugin")
