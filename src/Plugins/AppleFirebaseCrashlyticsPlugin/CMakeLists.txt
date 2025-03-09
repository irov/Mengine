MENGINE_PROJECT(AppleFirebaseCrashlyticsPlugin)

OPTION(MENGINE_FIREBASE_CRASHLYTICS_UPLOAD_SYMBOLS "Enable uploading symbols to Firebase Crashlytics" ${MENGINE_BUILD_TYPE_RELEASE})

ADD_FILTER(
src
    AppleFirebaseCrashlyticsPlugin.h
    AppleFirebaseCrashlyticsPlugin.mm

    AppleFirebaseCrashlyticsApplicationDelegate.h
    AppleFirebaseCrashlyticsApplicationDelegate.mm
)

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleFirebaseCrashlyticsApplicationDelegate")

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseCrashlytics" "NO-GIT" "11.8.0")

ADD_MENGINE_SCRIPT_PHASE("Run Firebase Crashlytics" "($){PODS_ROOT}/FirebaseCrashlytics/run" "['($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/($){TARGET_NAME}', '$(SRCROOT)/$(BUILT_PRODUCTS_DIR)/$(INFOPLIST_PATH)']")

if(MENGINE_FIREBASE_CRASHLYTICS_UPLOAD_SYMBOLS)
    MESSAGE(STATUS "Firebase Crashlytics symbols upload is enabled.")
    
    ADD_MENGINE_SCRIPT_PHASE("Upload Firebase Crashlytics Symbols" "($){PODS_ROOT}/FirebaseCrashlytics/run -gsp \"${MENGINE_DEPLOY_PATH}/${MENGINE_APPLICATION_DEPLOY_FIREBASE}\" -p ios \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"" "NO-INPUT-FILES")
endif()
