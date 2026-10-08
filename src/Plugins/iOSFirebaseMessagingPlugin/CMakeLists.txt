MENGINE_PROJECT(iOSFirebaseMessagingPlugin)

ADD_FILTER(
src
    iOSFirebaseMessagingInterface.h

    iOSFirebaseMessagingPlugin.h
    iOSFirebaseMessagingPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_MESSAGING)

ADD_MENGINE_SWIFT_PACKAGE("FirebaseMessaging" "https://github.com/firebase/firebase-ios-sdk.git" "13.0.0")

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/SourcePackages/checkouts/firebase-ios-sdk/FirebaseMessaging/Sources/Public")

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebaseMessagingPlugin")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
