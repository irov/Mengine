MENGINE_PROJECT(iOSFirebaseCrashlyticsPlugin)

OPTION(MENGINE_FIREBASE_CRASHLYTICS_UPLOAD_SYMBOLS "Enable uploading symbols to Firebase Crashlytics" ${MENGINE_BUILD_TYPE_RELEASE})

ADD_FILTER(
src
    iOSFirebaseCrashlyticsInterface.h

    iOSFirebaseCrashlyticsANRMonitor.h
    iOSFirebaseCrashlyticsANRMonitor.mm

    iOSFirebaseCrashlyticsPlugin.h
    iOSFirebaseCrashlyticsPlugin.mm
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IOS_FIREBASE_CRASHLYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("iOSFirebaseCrashlyticsPlugin")

ADD_MENGINE_SWIFT_PACKAGE("FirebaseCrashlytics" "https://github.com/firebase/firebase-ios-sdk.git" "13.0.0")

TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/SourcePackages/checkouts/firebase-ios-sdk/Crashlytics/Crashlytics/Public")

if(MENGINE_FIREBASE_CRASHLYTICS_UPLOAD_SYMBOLS)
    MESSAGE(STATUS "Firebase Crashlytics symbols upload is enabled.")

    get_filename_component(FIREBASE_CONFIG_NAME "${MENGINE_APPLICATION_DEPLOY_FIREBASE}" NAME)

    SET(FIREBASE_CRASHLYTICS_INPUT_FILES "['($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}', '($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/($){PRODUCT_NAME}', '($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}/Contents/Info.plist', '($){TARGET_BUILD_DIR}/($){UNLOCALIZED_RESOURCES_FOLDER_PATH}/${FIREBASE_CONFIG_NAME}', '($){TARGET_BUILD_DIR}/($){EXECUTABLE_PATH}']")

    ADD_MENGINE_SCRIPT_PHASE("Run Firebase Crashlytics" "\"${CMAKE_BINARY_DIR}/SourcePackages/checkouts/firebase-ios-sdk/Crashlytics/run\" -gsp \"($){TARGET_BUILD_DIR}/($){UNLOCALIZED_RESOURCES_FOLDER_PATH}/${FIREBASE_CONFIG_NAME}\"" "${FIREBASE_CRASHLYTICS_INPUT_FILES}")
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} iOSFirebasePlugin)
