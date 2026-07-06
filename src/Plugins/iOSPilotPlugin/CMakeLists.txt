MENGINE_PROJECT_EXTRA_LANGUAGE(iOSPilotPlugin Swift)

ADD_FILTER(
src
    iOSPilotPlugin.h
    iOSPilotPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_PILOT)
ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSPilotPlugin")

ADD_MENGINE_COCOAPOD_SOURCE("https://github.com/livekit/podspecs.git")
ADD_MENGINE_COCOAPOD_SOURCE("https://github.com/irov/swiftpilot-podspecs.git")
ADD_MENGINE_COCOAPOD(PilotSDK "NO-GIT" "1.0.58")

target_compile_options(${PROJECT_NAME} PRIVATE -fcxx-modules)