MENGINE_PROJECT_EXTRA_LANGUAGE(AppleAppTrackingPlugin Swift)

ADD_FILTER(
src
    AppleAppTrackingInterface.h

    AppleAppTrackingPlugin.h
    AppleAppTrackingPlugin.cpp
    AppleAppTrackingPlugin.def
    
    AppleAppTrackingNative.swift
    
    AppleAppTrackingService.h
    AppleAppTrackingService.mm
    
    ObjC-Bridging-Header.h
)

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_FILTER(
    embedding
        AppleAppTrackingScriptEmbedding.h
        AppleAppTrackingScriptEmbedding.cpp
        )
        
    INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/pybind/include)    
endif()

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_APPLE_APPTRACKING)

set_property(TARGET ${PROJECT_NAME} PROPERTY XCODE_ATTRIBUTE_SWIFT_OBJC_BRIDGING_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/ObjC-Bridging-Header.h")

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
