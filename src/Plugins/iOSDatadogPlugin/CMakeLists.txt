MENGINE_PROJECT_EXTRA_LANGUAGE(iOSDatadogPlugin Swift)

ADD_FILTER(
src
    iOSDatadogPlugin.h
    iOSDatadogPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_DATADOG)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSDatadogPlugin")

ADD_MENGINE_SWIFT_PACKAGE("DatadogCore" "https://github.com/DataDog/dd-sdk-ios.git" "3.19.0")
ADD_MENGINE_SWIFT_PACKAGE("DatadogLogs" "https://github.com/DataDog/dd-sdk-ios.git" "3.19.0")

target_compile_options(${PROJECT_NAME} PRIVATE -fcxx-modules)
