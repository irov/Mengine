MENGINE_PROJECT(AppleFirebaseMessagingPlugin)

ADD_FILTER(
src
    AppleFirebaseMessagingApplicationDelegate.h
    AppleFirebaseMessagingApplicationDelegate.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_MESSAGING)

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseMessaging" "NO-GIT" "12.3.0")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseMessagingApplicationDelegate")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} AppleFirebasePlugin)