MENGINE_PROJECT(AppleAppTrackingPlugin)

ADD_FILTER(
src
    AppleAppTrackingInterface.h

    AppleAppTrackingPlugin.h
    AppleAppTrackingPlugin.cpp
    AppleAppTrackingPlugin.def
    
    AppleAppTrackingNative.h
    AppleAppTrackingNative.m
    AppleAppTrackingNative.swift
    
    IOSImpl-Bridging-Header.h
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

if(MENGINE_USE_SCRIPT_SERVICE)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}pybind${MENGINE_LIB_SUFFIX})
endif()
