MENGINE_PROJECT_EXTRA_LANGUAGE(AppleDatadogPlugin Swift)

ADD_FILTER(
src
    AppleDatadogApplicationDelegate.h
    AppleDatadogApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_DATADOG)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleDatadogApplicationDelegate")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("DatadogCore" "NO-GIT" "3.1.0")
ADD_MENGINE_COCOAPOD("DatadogLogs" "NO-GIT" "3.1.0")

target_compile_options(${PROJECT_NAME} PRIVATE -fcxx-modules)
