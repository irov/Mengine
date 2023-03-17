MENGINE_PROJECT(AppleFirebaseCrashlyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseCrashlyticsInterface.h

    AppleFirebaseCrashlyticsPlugin.h
    AppleFirebaseCrashlyticsPlugin.mm
    
    AppleFirebaseCrashlyticsService.h
    AppleFirebaseCrashlyticsService.mm
)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "10.5.0")
ADD_MENGINE_COCOAPOD("Firebase/Crashlytics" "NO-GIT" "10.5.0")

# "${PODS_ROOT}/FirebaseCrashlytics/upload-symbols" -gsp "${PROJECT_DIR}/../../../Resources/deploy-evilmonkey-ios/deploy-ios-data/GoogleService-Info.plist" -p ios "${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}"

