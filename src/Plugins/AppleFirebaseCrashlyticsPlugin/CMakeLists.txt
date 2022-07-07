MENGINE_PROJECT(AppleFirebaseCrashlyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseCrashlyticsInterface.h

    AppleFirebaseCrashlyticsPlugin.h
    AppleFirebaseCrashlyticsPlugin.cpp

    
    AppleFirebaseCrashlyticsService.h
    AppleFirebaseCrashlyticsService.mm
)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS)

ADD_MENGINE_COCOAPOD("Firebase" "NO-GIT" "9.2.0")
ADD_MENGINE_COCOAPOD("FirebaseCrashlytics" "NO-GIT" "9.2.0")


# sh -> "${BUILD_DIR%/Build/*}/SourcePackages/checkouts/firebase-ios-sdk/Crashlytics/run"
# inputFiles -> ${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/${TARGET_NAME}
