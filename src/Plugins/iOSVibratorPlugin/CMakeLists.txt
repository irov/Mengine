MENGINE_PROJECT(iOSVibratorPlugin)

ADD_FILTER(
src
    iOSVibratorInterface.h

    iOSVibratorPlugin.h
    iOSVibratorPlugin.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        iOSVibratorScriptEmbedding.h
        iOSVibratorScriptEmbedding.mm
    )

    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_VIBRATOR)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSVibratorPlugin")
ADD_MENGINE_APPLE_FRAMEWORK(UIKit)
ADD_MENGINE_APPLE_FRAMEWORK(CoreHaptics)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()
