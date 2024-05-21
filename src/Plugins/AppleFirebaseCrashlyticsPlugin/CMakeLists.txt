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

ADD_MENGINE_COCOAPOD_GOOGLE_UTILITIES()
ADD_MENGINE_COCOAPOD("FirebaseCrashlytics" "NO-GIT" "10.26.0")

ADD_MENGINE_SCRIPT_PHASE("Run Firebase Crashlytics" "($){PODS_ROOT}/FirebaseCrashlytics/run" "['($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/($){TARGET_NAME}', '$(SRCROOT)/$(BUILT_PRODUCTS_DIR)/$(INFOPLIST_PATH)']")

if(MENGINE_BUILD_TYPE_RELEASE)
	ADD_MENGINE_SCRIPT_PHASE("Upload Firebase Crashlytics Symbols" "($){PODS_ROOT}/FirebaseCrashlytics/run -gsp \"${MENGINE_DEPLOY_PATH}/${MENGINE_APPLICATION_DEPLOY_FIREBASE}\" -p ios \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"" "NO-INPUT-FILES")
endif()