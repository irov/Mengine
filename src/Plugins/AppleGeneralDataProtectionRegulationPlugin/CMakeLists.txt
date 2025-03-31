MENGINE_PROJECT(AppleGeneralDataProtectionRegulationPlugin)

ADD_FILTER(
src
    AppleGeneralDataProtectionRegulationPlugin.h
    AppleGeneralDataProtectionRegulationPlugin.mm
)

ADD_FILTER(
delegate
    AppleGeneralDataProtectionRegulationApplicationDelegate.h
    AppleGeneralDataProtectionRegulationApplicationDelegate.mm
)

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    ADD_FILTER(
    embedding
        AppleGeneralDataProtectionRegulationScriptEmbedding.h
        AppleGeneralDataProtectionRegulationScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_GENERALDATAPROTECTIONREGULATION)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleGeneralDataProtectionRegulationApplicationDelegate")

if(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} Python)
endif()