MENGINE_PROJECT(AppleGeneralDataProtectionRegulationPlugin)

ADD_FILTER(
src
    AppleGeneralDataProtectionRegulationInterface.h
    AppleGeneralDataProtectionRegulationService.h
    AppleGeneralDataProtectionRegulationService.mm

    AppleGeneralDataProtectionRegulationPlugin.h
    AppleGeneralDataProtectionRegulationPlugin.mm
)

ADD_FILTER(
delegate
    AppleGeneralDataProtectionRegulationApplicationDelegate.h
    AppleGeneralDataProtectionRegulationApplicationDelegate.mm
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleGeneralDataProtectionRegulationScriptEmbedding.h
        AppleGeneralDataProtectionRegulationScriptEmbedding.mm
    )
    
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)
endif()

ADD_MENGINE_APPLE_PLUGIN(MENGINE_PLUGIN_APPLE_GENERALDATAPROTECTIONREGULATION)

ADD_MENGINE_APPLE_APPLICATION_DELEGATE("AppleGeneralDataProtectionRegulationApplicationDelegate")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()