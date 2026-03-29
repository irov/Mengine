MENGINE_PROJECT(ApplePilotPlugin)

ADD_FILTER(
src
    ApplePilotPlugin.h
    ApplePilotPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_PILOT)
ADD_MENGINE_APPLE_APPLICATION_DELEGATE("ApplePilotPlugin")

ADD_MENGINE_COCOAPOD(PilotSDK "https://github.com/irov/swiftpilot.git" "1.0.34")
