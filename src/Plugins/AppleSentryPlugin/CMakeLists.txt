MENGINE_PROJECT(AppleSentryPlugin)

ADD_FILTER(
src
    AppleSentryInterface.h

    AppleSentryPlugin.h
    AppleSentryPlugin.mm
    
    AppleSentryService.h
    AppleSentryService.mm
)

if(MENGINE_TARGET_IOS)
    ADD_FILTER(
    delegate
        AppleSentryApplicationDelegate.h
        AppleSentryApplicationDelegate.mm
    )
elseif(MENGINE_TARGET_MACOS)
    ADD_FILTER(
    delegate
        MacOSSentryApplicationDelegate.h
        MacOSSentryApplicationDelegate.mm
    )
endif()


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_SENTRY)

if(MENGINE_TARGET_IOS)
    ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleSentryApplicationDelegate")
elseif(MENGINE_TARGET_MACOS)
    ADD_MENGINE_APPLE_APPLICATION_DELEGATE("MacOSSentryApplicationDelegate")
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}math${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

ADD_MENGINE_COCOAPOD(Sentry "https://github.com/getsentry/sentry-cocoa.git" "8.29.0")

if(MENGINE_TARGET_IOS)
    if(MENGINE_BUILD_TYPE_RELEASE)
		IF(NOT MENGINE_APPLE_SENTRY_CLI_PATH)
		    MESSAGE(FATAL_ERROR "Sentry upload symbols CLI required setup MENGINE_APPLE_SENTRY_CLI_PATH")
		ENDIF()
		
		IF(NOT MENGINE_APPLE_SENTRY_CLI_ORG)
		    MESSAGE(FATAL_ERROR "Sentry upload symbols CLI required setup MENGINE_APPLE_SENTRY_CLI_ORG")
		ENDIF()
		
		IF(NOT MENGINE_APPLE_SENTRY_CLI_PROJECT)
		    MESSAGE(FATAL_ERROR "Sentry upload symbols CLI required setup MENGINE_APPLE_SENTRY_CLI_PROJECT")
		ENDIF()
		
		IF(NOT MENGINE_APPLE_SENTRY_CLI_AUTH_TOKEN)
		    MESSAGE(FATAL_ERROR "Sentry upload symbols CLI required setup MENGINE_APPLE_SENTRY_CLI_AUTH_TOKEN")
		ENDIF()
		
        ADD_MENGINE_SCRIPT_PHASE("Upload Sentry Symbols" "export SENTRY_ORG=${MENGINE_APPLE_SENTRY_CLI_ORG}\nexport SENTRY_PROJECT=${MENGINE_APPLE_SENTRY_CLI_PROJECT}\nexport SENTRY_AUTH_TOKEN=${MENGINE_APPLE_SENTRY_CLI_AUTH_TOKEN}\n$(${MENGINE_APPLE_SENTRY_CLI_PATH} upload-dif --include-sources \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\" 2>&1 >/dev/null)" "['($){DWARF_DSYM_FOLDER_PATH}/($){DWARF_DSYM_FILE_NAME}/Contents/Resources/DWARF/($){TARGET_NAME}']")
    endif()
endif()
