MENGINE_PROJECT(AppleFirebaseCrashlyticsPlugin)

ADD_FILTER(
src
    AppleFirebaseCrashlyticsInterface.h

    AppleFirebaseCrashlyticsPlugin.h
    AppleFirebaseCrashlyticsPlugin.mm
    
    AppleFirebaseCrashlyticsService.h
    AppleFirebaseCrashlyticsService.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS)

ADD_MENGINE_COCOAPOD("Firebase/Crashlytics" "NO-GIT" "10.19.0")
ADD_MENGINE_SCRIPT_PHASE("Upload Firebase Crashlytics" [=["${PODS_ROOT}/FirebaseCrashlytics/run"]=] [=["${DWARF_DSYM_FOLDER_PATH}/${DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/${TARGET_NAME}"]=])
