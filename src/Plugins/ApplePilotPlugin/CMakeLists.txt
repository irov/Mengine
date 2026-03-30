MENGINE_PROJECT(ApplePilotPlugin)

ADD_FILTER(
src
    ApplePilotPlugin.h
    ApplePilotPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_PILOT)
ADD_MENGINE_APPLE_APPLICATION_DELEGATE("ApplePilotPlugin")

ADD_MENGINE_COCOAPOD_SOURCE("https://github.com/livekit/podspecs.git")
ADD_MENGINE_COCOAPOD_SOURCE("https://github.com/irov/swiftpilot-podspecs.git")
ADD_MENGINE_COCOAPOD(PilotSDK "NO-GIT" "1.0.49")
