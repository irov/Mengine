MENGINE_PROJECT(iOSAmplitudePlugin)

ADD_FILTER(
src
    iOSAmplitudePlugin.h
    iOSAmplitudePlugin.mm
)

INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/SourcePackages/checkouts/Amplitude-iOS/Sources/Amplitude/Public")

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_AMPLITUDE)

ADD_MENGINE_SWIFT_PACKAGE("Amplitude" "https://github.com/amplitude/Amplitude-iOS.git" "8.22.2")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSAmplitudePlugin")
